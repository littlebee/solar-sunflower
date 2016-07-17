###

  webpack setup and external grunt tasks borrowed from:
    https://github.com/felixhao28/using-coffee-react-for-frontend-dev-walkthrough

###

Path = require('path')
_ = require('lodash')


BUMBLE_DOCS_SCRIPTS = './node_modules/bumble-docs/bin/'
bumbleScriptCommand = (scriptFile, args="")-> 
  return "coffee #{Path.join(BUMBLE_DOCS_SCRIPTS, scriptFile)} #{args}"

PI_ADDRESS = "192.168.2.2"

module.exports = (grunt, initConfig={}) ->
  
  # load plugins
  # this loads all of the grunt-... packages in package.json.  clever
  require('load-grunt-tasks')(grunt, {config: 'node_modules/bumble-build/package'})
  pkg = grunt.file.readJSON("package.json")

  # configuration
  grunt.initConfig
    pkg: pkg

    # args to initConfig method are the tasks
    clean:
      distrib: ["web/distrib/*"]
      arduino: ['arduino/app/**/*.o', 'arduino/app/**/*.hex']
        
    
    shell:
      npmInstall:
        command: 'npm install'
      
      test:
        command: 'node_modules/bumble-test/bin/testRunner.coffee'
        
      buildArduino:
        command: 'cd arduino/app && make'
        
      deployArduino:
        command: 'cd arduino/app && make upload'
        failOnError: false
        
      piPostDeploy:
        # running npm install takes a while and because of it's silly ascii animation
        # gives no feedback via ssh while running. 
        # command: 'ssh pi@PI_ADDRESS "cd /home/pi/sunflower && npm install"'
        command: "ssh pi@#{PI_ADDRESS} \"cd /home/pi/sunflower && scripts/piPostDeploy\""
        
      stopApiService: 
        command: "sudo /etc/init.d/sunflower-api stop"
        
      startApiService: 
        command: "sudo /etc/init.d/sunflower-api start"
        

    rsync: 
      options: 
        args: ["--timeout=2"],
        exclude: [".git*","*.scss","node_modules"],
        recursive: true
      pi: 
        options:  
          src: "./",
          dest: "pi@#{PI_ADDRESS}:/home/pi/sunflower",
          ssh: true,
        recursive: true


    availabletasks:
      tasks:
        options:
          filter: 'include'
          tasks: ['clean', 'build', 'buildArduino', 'buildWeb', 'deploy', 'deployArduino', 'deployPi', 'watch']
          descriptions: 
            clean: "Remove all compiled files."
            build: "Builds web app, REST service and arduino source"
            buildArduino: "Just build the arduino sources"
            buildWeb: "Just build the web app distribution"
            deploy: "Deploys app (rsync) to connected PI and arduino app to any connected arduinos"
            deployArduino: "Deploy compiled arduino sources to all connected arduinos"
            deployPi: "RemoteSync pi/sunflower to pi@192.168.1.1"
            watch: "Watch for changing files, builds and deploys them.  see scripts/watch for background watch " +
              "task used by Pi to automatically build and deploy arduino code to petal controllers"
            

    
    watch:
      buildArduino:
        files: ["arduino/**/*.ino", "arduino/**/*.cpp", "arduino/**/*.c", "arduino/**/*.h"]
        tasks: ["buildArduino"]
        
      deployArduino:
        files: ["arduino/**/*.o", "arduino/**/*.hex}"]
        tasks: ["deployArduino"]
      
      buildWeb: 
        files: ["web/**/*"]
        tasks: ["buildWeb"]
      
      deployPI:
        files: ["**/*"]
        tasks: ["deployPi"]
        options: 
          # higher debounce to let the above finish before uploading to pi
          debounceDelay: 500,
        

    webpack:
      distrib: require("./webpack.config.coffee")
      optimize: require("./webpack.optimized.config.coffee")


  # tasks
  grunt.registerTask 'build', ['buildWeb', 'buildArduino']
  grunt.registerTask 'buildWeb', ['webpack:distrib', 'webpack:optimize']
  grunt.registerTask 'buildArduino', ['shell:buildArduino']
  grunt.registerTask 'test', ["shell:test"]
  # uses forceOn to downgrade failures in deploying to warnings 
  grunt.registerTask 'deploy', ['forceOn', 'deployPi', 'deployArduino', 'forceOff'] 
  
  
  grunt.registerTask 'deployArduino', 'deploys all arduino bins to all connected arduinos', ->
    console.log "This may fail / warn if no arduinos are connected, and that's okay."
    
    # you have to stop the service which keeps an open connection to arduinos or else they 
    # reset on each connection.
    grunt.task.run 'shell:stopApiService' if process.env.USER == "pi"
    grunt.task.run 'shell:deployArduino'
    grunt.task.run 'shell:startApiService' if process.env.USER == "pi"
    
  grunt.registerTask 'deployPi', 'deploys code to the Raspberry Pi unless running on the pi itself', ->
    if process.env.USER == "pi"
      console.log "deployPi task can't be run on the pi itself, that's insanity. "
      return false;
    grunt.task.run ['rsync', 'shell:piPostDeploy']       
    
  
  grunt.registerTask 'default', ['availabletasks']
  
  grunt.loadNpmTasks('grunt-rsync');
  
  grunt.registerTask 'forceOff', 'Forces the force flag off', ->
    grunt.option 'force', false
    return 
    


  grunt.registerTask 'forceOn', 'Forces the force flag on', ->
    grunt.option 'force', true
    return 
  

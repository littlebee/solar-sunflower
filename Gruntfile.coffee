###

  webpack setup and external grunt tasks borrowed from:
    https://github.com/felixhao28/using-coffee-react-for-frontend-dev-walkthrough

###

Path = require('path')
_ = require('lodash')


BUMBLE_DOCS_SCRIPTS = './node_modules/bumble-docs/bin/'
bumbleScriptCommand = (scriptFile, args="")-> 
  return "coffee #{Path.join(BUMBLE_DOCS_SCRIPTS, scriptFile)} #{args}"


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
        
      deployArduinos:
        command: 'cd arduino/app && make upload'
        failOnError: false


    rsync: 
      options: 
        args: ["--timeout=2"],
        exclude: [".git*","*.scss","node_modules"],
        recursive: true
      pi: 
        options: 
          src: "./",
          dest: "pi@192.168.1.1:/home/pi/sunflower",
          ssh: true,
        recursive: true


    availabletasks:
      tasks:
        options:
          filter: 'include'
          tasks: ['clean', 'build', 'deploy', 'watch']
          descriptions: 
            clean: "Remove all compiled files. Use `grunt clean build` to rebuild everything from scratch"
            build: "Builds web app, REST service and arduino source"
            deploy: "Deploys app (rsync) to connected PI and arduino app to any connected arduinos"
            watch: "Watch for changing files, builds and deploys them.  see scripts/watch for background watch " +
              "task used by Pi to automatically build and deploy arduino code to petal controllers"
            

    
    watch:
      buildArduino:
        files: ["arduino/**/*.{ino,cpp,c,h}"]
        tasks: ["buildArduino"]
        
      deployArduinos:
        files: ["arduino/**/*.{o,hex}"]
        tasks: ["deployArduinos"]
      
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
  grunt.registerTask 'deploy', ['forceOn', 'rsync', 'deployArduinos', 'forceOff'] 
  grunt.registerTask 'deployArduinos', ['shell:deployArduinos']
  # uses forceOn to downgrade failures in deploying to warnings 
  grunt.registerTask 'deployPi', ['forceOn', 'rsync', 'forceOff']   
  
  grunt.registerTask 'default', ['availabletasks']
  
  grunt.loadNpmTasks('grunt-rsync');
  
  grunt.registerTask 'forceOff', 'Forces the force flag off', ->
    grunt.option 'force', false
    return 


  grunt.registerTask 'forceOn', 'Forces the force flag on', ->
    grunt.option 'force', true
    return 
  

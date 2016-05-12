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
        
      build_arduino:
        command: 'cd arduino/app && make'
        
      deploy_arduinos:
        command: 'cd arduino/app && make upload'
        failOnError: false


    rsync: 
      options: 
        args: ["--verbose", "--timeout=2"],
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
      deployToPI:
        files: ["**/*"]
        tasks: ["deploy"]


    webpack:
      distrib: require("./webpack.config.coffee")
      optimize: require("./webpack.optimized.config.coffee")


  # tasks
  grunt.registerTask 'build_web', ['webpack:distrib', 'webpack:optimize']
  grunt.registerTask 'build_arduino', ['shell:build_arduino']
  grunt.registerTask 'build', ['build_web', 'build_arduino']
  grunt.registerTask 'test', ["shell:test"]
  grunt.registerTask 'deploy', ['forceOn', 'rsync', 'shell:deploy_arduinos', 'forceOff'] 
  grunt.registerTask 'default', ['availabletasks']
  
  grunt.loadNpmTasks('grunt-rsync');
  
  grunt.registerTask 'forceOff', 'Forces the force flag off', ->
    grunt.option 'force', false
    return 


  grunt.registerTask 'forceOn', 'Forces the force flag on', ->
    grunt.option 'force', true
    return 
  

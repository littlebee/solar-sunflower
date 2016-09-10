#!/usr/bin/env coffee

APP_NAME = 'sunflower-api'

Logger = require('bunyan')
log = new Logger.createLogger
  name: APP_NAME
  serializers: 
    req: Logger.stdSerializers.req

Restify = require 'restify'
server = Restify.createServer
  name: APP_NAME,
  version: '0.1.0',
  log: log

    
Petals = require './app/models/petals'
petals = new Petals()
petals.fetch()

requestCount = 0

debugger

onError = (err, req, next) =>
  req.log.error err, "ERROR"
  return next(new restify.errors.InternalServerError(err))
    

server.pre (req, res, next) ->
  req.log.info({ req: req }, 'REQUEST');
  next();


server.use (req, res, next) ->
  requestCount++
  next()

server.use(Restify.queryParser())

server.on 'uncaughtException',  (req, res, route, err) ->
  req.log.error err, 'uncaughtException'
  res.send err
  

server.get '/', (req, res, next) ->
  # TODO
  res.send {
    status: "great"
    version: "0.1.0"
    requests: requestCount
  }
  next()
  
  
server.get '/petal', (req, res, next) ->
  petals.fetch 
    success: ->
      console.log "fetched #{petals.length} petals"
      res.send petals.models
      next()
    error: (collection, err) ->
      onError(err, req, next)  
      

server.get '/petal/cmd/:command', (req, res, next) ->
  petals.queueArduinoCommand req.params.command, 
    data: req.params.args
    success: ->
      petals.fetch 
        success: ->
          res.send petals
          next()
        error: (collection, err) -> 
          onError(err, req, next)
    error: (collection, err) -> 
      onError(err, req, next)


server.get '/petal/:id', (req, res, next) ->
  petals.fetch 
    success: ->
      console.log "fetched #{petals.length} petals"
      res.send petals.get(req.params.id)
      next()
    error: (collection, err) ->
      onError(err, req, next)  
  
  
server.get '/petal/:id/cmd/:command', (req, res, next) ->
  petal = petals.get(req.params.id)
  onError("petal not found", req, next) unless petal?
  
  petal.queueArduinoCommand req.params.command, 
    data: req.params.args
    success: ->
      petal.fetch 
        success: ->
          res.send petal
          next()
        error: (collection, err) -> 
          onError(err, req, next)
    error: (collection, err) -> 
      onError(err, req, next)
    
      
      
      
server.listen 8086, ->
  console.log '%s listening at %s', server.name, server.url
  return
  
  
  

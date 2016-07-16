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

    
Petals = require './models/petals'
petals = new Petals()

requestCount = 0


server.pre (req, res, next) ->
  req.log.info({ req: req }, 'REQUEST');
  next();


server.use (req, res, next) ->
  requestCount++
  next()


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
      req.log.error err, "ERROR"
      return next(new restify.errors.InternalServerError(err))
  

server.listen 8080, ->
  console.log '%s listening at %s', server.name, server.url
  return
  
  
  

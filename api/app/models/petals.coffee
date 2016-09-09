

Backbone = require 'backbone'
Bstr = require 'bumble-strings'
_ = require 'underscore'
SerialPort = require 'serialport'

Petal = require './petal'

module.exports = class Petals extends Backbone.Collection
  
  model: Petal
  
  constructor: (@options={}) ->
    super
    @isFetching = false
    # array of options passed to fetch() when it's already fetching
    @queuedFetches = [] 
    # array of {timestamp: ms, error: object}
    @errors = []   
      
  
  fetch: (options={}) ->
    if @isFetching 
      # queueFetches don't cause a refetch, but will have their
      # success and error handlers called when the current fetch 
      # finishes
      @queuedFetches.push options
      return true;
      
    @isFetching = true;
    
    @_walkSerialPorts(options)
    
    
  _walkSerialPorts: (options={}) ->
    SerialPort.list (err, ports) =>
      return if @_handleError(err)
      
      numPetalsFetching = 0
      complete = () =>
        numPetalsFetching -= 1
        return unless numPetalsFetching <= 0
        @_onPetalFetchComplete(options)
        
      success = (model, resp) =>
        @_onPetalFetchSuccess(model, resp, options)
        complete()
        
      error = (model, err) =>
        @_handleError err, model: model
        complete()
        
      for port in ports 
        # TODO : this might be problematic on knock off boards
        unless port.manufacturer? && Bstr.weaklyHas(port.manufacturer, "arduino")
          console.log "Petals.fetch skipping port #{JSON.stringify(port)} (not an arduino)"
          continue
        petal = @get(port.comName) 
        unless petal?
          petal = new Petal(id: port.comName)
          
        numPetalsFetching += 1
        # we only add it to the collection if it responds with a valid state
        # see success handler above
        petal.fetch success: success, error: error
            
      unless numPetalsFetching > 0
        error null, "No arduinos found."
        
        
  _onPetalFetchSuccess: (model, resp, options={}) =>
    existingModel = @get(model.id)
    if existingModel?
      if model.state == "unknown" 
        console.log "Petals: removing existing model #{existingModel.id}. fetched state == 'unknown'"
        @remove(existingModel)
      # we don't need to set the existing model attributes because it 
      # already did that when told to fetch
    else 
      if model.state == "unknown"
        console.log "Petals.fetch skipping port #{model.id} (fetched state=='unknown')"
      else
        # this should be the only place models are added so that we don't
        # have invalid petal models
        @add model 
      
        
  _onPetalFetchComplete: (options={}) =>
    @isFetching = false
    if @errors.length > 0
      @_callFetchCallbacks('error', @errors, options)
    else  
      @_callFetchCallbacks('success', "ok", options)
    @trigger 'reset'


  # also calls options success or error and @queuedFetches error or success
  # whichOne = "success" "error"
  _callFetchCallbacks: (whichOne, resp, options={}) ->
      options[whichOne]?(@, resp)
      while @queuedFetches.length > 0
        queuedOptions = @queuedFetches.shift()
        continue unless queuedOptions?
        queuedOptions[whichOne]?(@, resp)
    
        
  _handleError: (err, options={}) ->
    return false unless err
    console.error("ERROR:Petals:#{options.model?.id || 'all'} ", err)
    options.error?(@, err) || @options.error?(@, err)
    @errors.push {timestamp: Date.now(), error: err, model: model.attributes}

    return true    
    
    
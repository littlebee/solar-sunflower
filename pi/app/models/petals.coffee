

Backbone = require 'backbone'
Bstr = require 'bumble-strings'
SerialPort = require 'serialport'

Petal = require './petal'

module.exports = class Petals extends Backbone.Collection
  
  model: Petal
  
  fetch: (options) ->
    @isFetching = true;
    
    SerialPort.list (err, ports) =>
      if err
        console.error "SerialPort.list failed with error:", err
        options.error?(err)
        return
      
      numPetalsFetching = 0
      errors = []
      models = []
      complete = () =>
        numPetalsFetching -= 1
        return unless numPetalsFetching <= 0
        @isFetching = false
        @reset models
        if errors.length > 0
          options.error?(@, errors)
        else  
          options.success?(@, "ok")
        
      success = (model, resp) =>
        models.push model if model.state != "unknown"
        complete()
        
      error = (model, err) =>
        console.error "Unable to fetch petal @ #{@id}."
        errors.push err
        complete()
        
      for port in ports 
        # TODO : this might be problematic on knock off boards
        unless port.manufacturer? && Bstr.weaklyHas(port.manufacturer, "arduino")
          console.log "Petals.fetch skipping port " + JSON.stringify(port)
          continue
        petal = new Petal(id: port.comName)
        numPetalsFetching += 1
        petal.fetch success: success, error: error
            
      unless numPetalsFetching > 0
        error null, "No arduinos found."
        
        
      
    
    
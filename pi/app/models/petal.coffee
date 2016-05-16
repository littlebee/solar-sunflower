
Backbone = require('backbone')
SerialPort = require('serialport')
_ = require("underscore")
Bstr = require('bumble-strings')

Parsers = SerialPort.parsers
SerialPort = SerialPort.SerialPort

module.exports = class Petal extends Backbone.Model
  
  # we are using backbone models and collctions because it's a familiar MVC library, with get and set
  # methods and so that are models are similar in use to the web/app models that talk to this API
  # This class fetches from a    
  url: -> throw "this is not a network model and whatever method called me should have not been used or overridden"
  
  defaults:
    petalId: -1
    state: "unknown"
  
  #  expects id attribute to be the serial port device name
  constructor: (attributes, options) ->
    super
    
    
  # override base - fetch petal status information from associated com port   
  fetch: (options={}) ->
    @executeArduinoCommand "status", options
        
      
  executeArduinoCommand: (command, options={}) ->
    port = new SerialPort @id, 
      baudrate: 9600
      parser: Parsers.readline('\n')

    port.on 'open', (err) ->
      @_handleError(err) if err
    
    port.on 'data', (resp) =>
      console.log "response from arduino", resp
      if Bstr.weaklyEqual(resp, "ready")
        console.log "sending \"#{command}\" to #{@id}"
        port.write "#{command}\n", (err, bytesWritten) =>
          @_handleError(err, options) if err
      else
        @set @parse(resp) 
        options.success?(@, resp)
        @trigger 'sync', @
        port.close()
        
  parse: (resp) ->
    resp = resp.split(' ')
    return {
      petalId: resp[0]
      state: resp[1]
      direction: resp[2]
      panelCurrentSensor: resp[3]
      calibratedHighSensorValue: resp[4]
      calibratedHighSensorMs: resp[5]
      _calibratedDurationMs: resp[6]
    }
        
  _handleError: (err, options) =>
    console.log('Error: ', err.message)
    options.error?(@, err)
    port.close();
  
  
    
    
    
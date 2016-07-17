
Backbone = require('backbone')
SerialPort = require('serialport')
_ = require("underscore")
Bstr = require('bumble-strings')

Parsers = SerialPort.parsers
SerialPort = SerialPort.SerialPort

# This class fetches from an arduino controlling the petal and it's lighting
module.exports = class Petal extends Backbone.Model
  
  # we are using backbone models and collections because it's a familiar MVC library, with get and set
  # methods and so that are models are similar in use to the web/app models that talk to this API
  
  url: -> throw "this is not a network model and whatever method called me should have not been used or overridden"
  
  defaults:
    petalId: -1
    petalState: "unknown"
  
  #  expects id attribute to be the serial port device name
  constructor: (attributes, @options) ->
    super
    @serialPort = null
    
    # array of {command: string, options: object } lifo order
    #   that queueArduinoCommand was called
    @commandQueue = [] 
    @draining = false
    
    @_initializeSerialPort()
    
    
  destroy: () ->
    @closeSerialPort()
    
    
  closeSerialPort: () ->
    @port?.close()
    @port = null
  
    
  # override base - fetch petal status information from associated com port   
  fetch: (options={}) ->
    @queueArduinoCommand "status", options
        
  
  # commands are executed asynchronously and you probably want a success 
  # handler in object.  Options are standard backbone options,  success, error, etc.
  queueArduinoCommand: (command, options={}) ->
    @commandQueue.push {command: command, options: options}
    @drainQueue()  
    
    
  drainQueue: () ->
    return if @draining or not @ready
    @_chainedDrain()
    
  
  # This plus the onData hander serializes commands to the arduino. We wait
  # for a response before sending the next command.
  #
  # Every command on the arduino is expected to respond with a JSON 
  # object of attributes for the model
  _chainedDrain: () ->
    @draining = @commandQueue.length > 0
    @currentCmdObj = null
    return unless @draining
    
    @currentCmdObj = @commandQueue.shift()
    console.log "sending \"#{@currentCmdObj.command}\" to #{@id}"
    @port.write "#{@currentCmdObj.command}\n", (err, bytesWritten) =>
      if @_handleError(err, @currentCmdObj.options)
        @_chainedDrain()  # only on error 


  parse: (resp) ->
    return JSON.parse(resp)
    
  
  _initializeSerialPort: () ->
    @port = new SerialPort @id, 
      baudrate: 9600
      parser: Parsers.readline('\n')

    @port.on 'open', (err) =>
      @_handleError(err) 
    
    # this works because we never git unsolicited
    # data except on connect which we get a "ready"
    # on the serial port that tells us it's reset and ready
    @port.on 'data', (resp) =>
      console.log "response from arduino", resp
      if resp.match /ready/i
        @ready = true
        @drainQueue()
      else
        @set @parse(resp) 
        # so the node serialPort lib doesn't set the resource port
        # as in use and another com port connection to this port, 
        # like from the arduino serial port monitor, may
        @currentCmdObj?.options?.success?(@, resp)
        @trigger 'sync', @
        # this has the effect of serializing all commands and responses and is 
        # probably not strictly needed.  Call _chainedDrain instead of drainQueue
        # to continue draining if already @draining
        @_chainedDrain()       
        

  _handleError: (err, options={}) =>
    return false unless err
    console.error("ERROR:Petal:#{@id} ", err)
    options.error?(@, err) || @options.error?(@, err)
    return true
    
  
  
    
    
    
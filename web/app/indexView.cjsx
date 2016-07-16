

#React = require('react')

###
  This is the main view component for the solar sunflower web UI.
  
  This is also the file that webpack points at for dependencies 
###
module.exports = class IndexView extends React.Component
  @displayName: "sunflower.IndexView"
  
  
  render: ->
    <div style={fontSize: 30}>"I am.  I always have been."</div>
    
    
  

### pi/ directory

The application running on the raspberry pi for the solar sunflower project has the 
following reponsibilities:

 - open the flower in the morning
 - close the flower at night
 - close the flower when winds are too high
 - provide REST API to front end

This branch contains the codes for:
- REST API - pi/app/api.coffee
- schedule runner (TBD) and associated things
- maintainance and test scripts for the  

The code in this branch does not include any of the web interface, html or css.  
See web/* for React web app that talks to the REST API
 


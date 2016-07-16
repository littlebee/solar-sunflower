
### solar-sunflower WEB UI

This tree contains the source for React web UI accessible on / distributed from the Raspberry PI.  

To control the operation of the flower and get information about the state of the flower, logs, etc., the web app communicates with a REST service running on the Raspberry PI.   See /pi/app  for the REST service code.
 
When the user connects to the wifi hotspot running on the PI and authenticates, their browser is sent to http://192.168.1.1 to which nginx running on the PI serves up web/index.html.  index.html in turn loads vendor scipts like react, backbone and our webpack distribution file (see web/app/dist/*).  index.html also kicks off React rendering web/app/indexView.cjsx. 

 
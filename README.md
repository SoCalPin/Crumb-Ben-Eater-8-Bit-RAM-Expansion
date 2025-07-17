I am using Crumb Simulator to learn more about digital electronic design. I have successfully built Ben Eaters 8 Bit computer in Crumb.  
Seeing there are no larger RAM chips available yet on Crumb, I decided to simulate RAM memory by using the Crumb Arduino Nano.  This is 
a work in progress and I am actively seeking input on how to improve it.

Note that although working, in its current state, the clock must be very slow to accommodate the speed of the Arduino Nano simulation.
Room for improvement:
     1. Make it work with a faster simulated clock frequency
     2. Reduce the number of components
     3. Once 1 and 2 are resolved, expand the Program Counter and Memory Register, etc. to allow for more address space (from 16 bytes to 2048 bytes or more)
     
Please feel free to download and improve upon it.  Also, if you do improve it, please notify me so I can improve my design as well.  
Through collaboration, while we wait for Crumb's next version we can continue to work towards a version of "SAP2".

Please do ignore the "bolt on" ugliness of the design in Crumb.  My goal was to essentially replace the existing RAM chips with a "plug and play" replacement... 
I came pretty close leaving the existing Ben Eater version wiring in place.  



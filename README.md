The virtual sprayer transmitter (VSTX) is part of the virtual sprayer project.  The following describes the interfaces for tshis module and how it operates.

Inputs
------
Light break detector (digital input)
Vibration detector (analog input)
Bypass button (digital input)

Outputs
-------
Wireless transmitter (serial output)
Light break detector (digital output - 38 KHz)
x2 - LED strips (serial output)

Day in the Life
----------------
The following details the operation of the virtual sprayer transmitter (VSTX).  

1. The VSTX monitors the light break detector and if the light beam is broken, it sends a message via the wireless transmitter.  The pattern that is  'A5A5_9ACE_A5A5_1357'.
2. When the lightbeam is broken, the VSTX sends a light pattern to the LED strips.
3. The VSTX monitors the vibration detector and when a vibration is detected, a pattern is sent to the 
4. If the bypass button is asserted, the light break will not be monitired.  This allows the light break to be broken without sending of a message via the wireless transmitter.
# Purpose

# CircuitPython USB capabilities

# States

1. Production state : create a personalised USB device.
	- if keyboard state, the content of kbd_data.txt is send
	- if generic state, a USB device with personalised vendor_name, product ID, serial number
	to switch to configuration state, boot with button pressed
2. Configuration state : the Pico is in "normal state". Boot with button unprssed 
	

# Switch state
1. Production state : if button is presses at boot, switch to configuration state
2. Configuration state : modify config.txt to set the option production_state=1


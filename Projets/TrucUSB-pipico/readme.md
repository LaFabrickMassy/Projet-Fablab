# Purpose

# CircuitPython USB capabilities

# Modes

1. Production mode : create a personalised USB device.
	- if keyboard funcionality, the content of kbd_data.txt is send
	- if generic functionality, a USB device with personalised vendor_name, product ID, serial number
	to switch to configuration mode, boot with button pressed
2. Configuration mode : the Pico is in "normal state". 
	

# Switch between modes
1. Production mode : if button is presses at boot, switch to configuration state
2. Configuration mode : modify config.txt to set the option production_mode=1 and rename script file to main.py

# Sequences
## Production mode
- Check button state
	- if button pressed
		- in configuration.txt, change option production_mode to 0
		- blink led quickly during 10s
		- reboot
- Check configuration.txt	
	- if production_mode == 0
		- rename main.py to main.py.no_run
		- blink led during 10s
		- reboot
	- disable all USB devices created by circuitpython (list TBD)
	- if functionality == 'keyboard'
		- if option keyboard_keys is empty or non existent
			- blink led quickly and wait
		- else
			- create a USB keyboard device
			- read keyboard_keys
			- transform keybard_keys to a list of key values
			- send key values thought the USB keyboard device
			- blink led slowly and wait
	- if functionality == 'device'
		- if option device

## Configuration mode
In configuration state, as the script file has extension .py.no-run, the code is not launched

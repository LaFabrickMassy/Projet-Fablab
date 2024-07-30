# Purpose

TrucUSB is a devices designed to test USB identification (vendor_ID and device_ID), and
to simulate keyboard sequences.
It is configured in on of two modes:
- USB identification : the vendor_ID and the device_ID can be set to any string
- Key generator : once plugged, it simulates a keyboard and a programmable sequence of
keys is send by the simulated keyboard. This mode is useful to automate process where repeating
keyboard sequences are needed.

# CuircuitPython and USB

## boot sequence

Boot sequence launchs the `boot.py` script. This script is in charge to setup USB devices :
disable Curcuitpython default devices such as CIRCUITPYTHON drive, serial port, repl, 
change USB devices configuration or creating new devices.

After executing boot.py, Circuitpython sends the USB descriptors to the host. 
The host sets up the connections to the devices while Circuitpython launchs the 
code.py script.

As there is no REPL nor serial connection to the host during boot, errors during boot.py execution
are written to the boot_out.txt.

For more information about boot sequence, USB and errors, see 
(https://learn.adafruit.com/customizing-usb-devices-in-circuitpython/usb-setup-timing)

## CircuitPython default USB devices

Normal boot presents 5 USB devices:
- Communication (the serial interface)
- Mass storage (the internal memory seen as a removable drive CIRCUITPYTHON)
- HID
- Audio
- USB Composite device

## Disablig default devices

### Mass storage

***WARNING : this disable the CIRCUITPYTHON drive, don't do this***
```
import storage
storage.disable_usb_drive()
```

### Serial console

```
import usb_cdc
usb_cdc.disable()   # Disable both serial devices.
usb_cdc.enable(console=True, data=False)   # Enable just console (the default setting)
usb_cdc.enable(console=True, data=True)    # Enable console and data
usb_cdc.enable(console=False, data=False)  # Disable both, same as usb_cdc.disable()
```

### HID

```
import usb_hid
usb_hid.disable()
```

## Bypassing boot.py

If the mass storage is disabled in `boot.py`, we no longer have acces to CIRCUITPY drive, 
so it is impossible to modify the scripts.
On Pi Pico, the BOOTSEL button status is not available in the scripts.

Possible solutions are:
- keep serial console to access to REPL and allow to erase/rename/modify boot.py (not tested)
- read a GPIO status and disable devices only if the GPIO is up or down. 
This needs to connect a GPIO to V+ or GND and set a pull up or pull down.

# TrucUSB Modes

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
	- disable all USB devices created by circuitpython : Communication, Mass storage, HID, Audio and USB composite device
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
		- if option device is empty or non existent
			- blink led quickly and wait
		- else
			- read device class, vendor_id, device_id from config.txt
			- Create the USB device
			
## Configuration mode
In configuration state, as the script file has extension .py.no-run, the code is not launched

# Annex A - keyboard_keys definition syntax
Keycodes are available at https://www.win.tue.nl/~aeb/linux/kbd/scancodes-14.html. 
Keycodes are defined for a US layout.

keyboard_keys is a string containing key symbols, such as letters, digits or special characters, 
escape sequences and pause.

Escape sequences begins with the backslash symbol. Available escape sequances are:
- \\: backslash key
- \{, \}: curly bracket keys
- \', \": simple and double quote
- \w: the "Windows" key
- \DDD, \xXX: the key symbol ddd (in decimal) or XX (in hexa). 
If the first character after the \ is a digit, the value is dd. 
If the first character after the \ is a 'x' or a 'X', the value is 0xXX.
- \n: the Enter key
- \t: the Tab key
- \<, \>, \^, \v: left, right, up and down keys

Pause sequence is {nnnn}, describing a nnnn milliseconds pause (approximately, depending on software)

**Exemple** The following sequence "\wr{500}cmd.exe\n{1000}dir" sends the Windows+r commands, waits for 500ms, 
sends "cmd.exe", waits for 1s and enter the "dir" command.



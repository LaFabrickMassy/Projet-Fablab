# Purpose

TrucUSB is a device designed to test USB identification (vendor_ID and device_ID), and
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

# TrucUSB usage

## Modes

TrucUSB has two modes : production and configuration modes. 

In production mode, it creates the USB devices and, if key generator is activated, sends the keys.

In configuration mode, specific configurations are disabled. Default CIRCUITPY drive, serial port and REPL are usable.
It is possible to modify the configuration only in this mode.

## Disable production mode

Connecting GPIO13 to the ground during boot, using a jumper for example, disable the production mode. 
GPIO number is parametrable in config.py with CONFIG_GPIO.

## Configuration

The configuration is set in config.py. This file defines the following variables:
- MODE_GPIO: GPIO number used to enter in configuration mode (integer)
- CONFIG_USB_VENDOR: vendor_id of USB devices (string)
- CONFIG_USB_PRODUCT: device_id of USB devices (string)
- KEY_SEQUENCE: keys sent (string). See [Key sequences](#key-sequences). If this variable is not set,
no keys are send.

## Key sequences

### Keyboard layouts

USB keyboards send keys numbers and not symbols (letters, digits,...). 
A key sends the same code wether the keyboard is a US QWERTY or a French AZERTY. The operating system
applies locale translation to get a "A" when the "Q" key is pressed if the OS is configured with an
AZERTY keyboard.

Keycodes are defined in chapter 10 of [HID Usage Table](https://usb.org/sites/default/files/hut1_3_0.pdf)
Keycodes match the sign printed on the kyboard only on a US keyboard.

### KEY_SEQUENCE syntax

The KEY_SEQUENCE string is composed of unitary symbols such as letters (capitalized or not), digits,
symbols (dot, comma, colon, ...), modifiers and escaped symbols. 

A key can be modified by a 'modifier' such as shift, control, ... A capital A is `<shift>a'

- lowercase letters: a=04, ..., z=1D
- uppercase letters: an uppercase letter sends the lowercase letter modified by right shift.
- digits: 1=1E, ..., 0=27
- specific keys:
	- space: <spc> or <space>
	- keyboard enter: <enter>
	- numpad enter: <npenter>
	- tab: <tab>
	- home: <home>
	- end: <end>
	- page up: <pgup>
	- page down: <pgdn>
- modifers:
	- left shift: <shift> or <lshift>
	- right shift: <rshift>
	- left control: <ctrl> or <lctrl>
	- right control: <rctrl>
	- left alt: <alt>
	- right alt: <ralt> or <altgr>
	- windows: <win>






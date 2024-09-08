'''
    # read configuration file
    configuration = ReadConfiguration()
    if configuration["mode"] == "keyboard":
        RunKeyboard(configuration["KeyboardData"])
'''
# SPDX-FileCopyrightText: 2021 Kattni Rembor for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""Example for Pico. Blinks the built-in LED."""
import time
import board
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

from config.py import *

# time between to keys sent as keyboard
if CONFIG_KEY_TIME:
    key_time = CONFIG_KEY_TIME
else:
    key_time = 0.2
    
led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

button = digitalio.DigitalInOut(board.GP13)
button.switch_to_input(pull=digitalio.Pull.UP)

kbd = Keyboard(usb_hid.devices)


lc_letters = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
              'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
digits = ['0','1','2','3','4','5','6','7','8','9']

####################################################################
#
####################################################################
def BlinkError():
    sleep=0.1
    while True:
        led.value = True
        time.sleep(sleep)
        led.value = False
        time.sleep(sleep)

####################################################################
#
####################################################################
def BlinkNormal():
    while True:
        if button.value:
            sleep = 0.5
        else:
            sleep = 1
        led.value = True
        time.sleep(sleep)
        led.value = False
        time.sleep(sleep)

####################################################################
#
####################################################################
def kbd_send(*arg)
    print(arg)
    
####################################################################
# Sendkeys to keyboard according to keydef
####################################################################
def SendKeys(kbd, keydef):
    keys_to_send = []
    while keydef:
        k = keydef[0]
        if k in lc_letters:
            # lowercase letters
            keys_to_send.append(ord(k)-ord('a')+Keycode.A)
            kbd_send(*keys_to_send)
            time.sleet(key_time)
            keys_to_send = []
        elif lower(k) in lc_letters:
            # uppercase letters
            keys_to_send.append(Keycode.LEFT_SHIFT)
            keys_to_send.append(ord(k)-ord('a')+Keycode.A)
            kbd_send(*keys_to_send)
            time.sleet(key_time)
            keys_to_send = []
        if k in digits:
            # DIGITS
            if k == '0':
                keys_to_send.append(Keycode.ZERO)
            else:
                keys_to_send.append(ord(k)-ord('1')+Keycode.ONE)
            kbd_send(*keys_to_send)
            time.sleet(key_time)
            keys_to_send = []
        elif keydef.lower().startswith("<shift>")
            # SHIFT
            keys_to_send.append(Keycode.LEFT_SHIFT)
            keydef = keydef[7:]
        elif keydef.lower().startswith("<ctrl>")
            # CONTROL
            keys_to_send.append(Keycode.LEFT_CONTROL)
            keydef = keydef[6:]
        elif keydef.lower().startswith("<alt>")
            # ALT
            keys_to_send.append(Keycode.LEFT_ALT)
            keydef = keydef[5:]
        elif keydef.lower().startswith("<win>")
            # ALT
            keys_to_send.append(Keycode.APPLICATION)
            keydef = keydef[5:]
        elif keydef.lower().startswith("<tab>")
            # TAB
            keys_to_send.append(Keycode.TAB)
            kbd_send(*keys_to_send)
            time.sleet(key_time)
            keys_to_send = []
            keydef = keydef[5:]
        elif keydef.lower().startswith("<enter>")
            # ENTER
            keys_to_send.append(Keycode.ENTER)
            kbd_send(*keys_to_send)
            time.sleet(key_time)
            keys_to_send = []
            keydef = keydef[7:]
        else:
            raise ValueError("Unknown key at position {}".format(i))
            
####################################################################
#
####################################################################
def KeyboardMode():
    if CONFIG_KEYBOARD_KEYS:
        SendKeys(CONFIG_KEYBOARD_KEYS)
    BlinkError()

####################################################################
#
####################################################################
def Production():
    if CONFIG_MODE:
        if CONFIG_MODE.lower() == "keyboard":
            KeyboardMode()

if CONFIG_DEBUG:
    production_mode = (button.value == False)
else:
    production_mode = (button.value == True)

if production_mode:
    Production()

BlinkNormal()
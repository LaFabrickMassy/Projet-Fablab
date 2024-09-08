import digitalio, board, storage
import usb_hid, usb_cdc
import supervisor

from config import *

button = digitalio.DigitalInOut(board.GP13)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.UP

#################################################################
#
#################################################################
def DisableUnwantedUSB():
    usb_cdc.disable() # serial port
    """
    usb_hid.disable() # HID
    storage.disable_usb_drive() # CIRCUITPY drive
    # audio device
    # Pico composite device
    """

#################################################################
#
#################################################################
def KeyboardMode():
    raise NameError("KeyboardMode")

#################################################################
#
#################################################################
def USBDeviceMode():
    if CONFIG_USB_VENDOR:
        usb_vendor = CONFIG_USB_VENDOR
    else:
        usb_vendor = "Raspberry Foundation"

    if CONFIG_USB_PRODUCT:
        usb_product = CONFIG_USB_PRODUCT
    else:
        usb_product = "Pi Pico"

    supervisor.set_usb_identification(usb_vendor, usb_product)

#################################################################
#
#################################################################
def Production():
    DisableUnwantedUSB()

    if CONFIG_MODE:
        if CONFIG_MODE.lower() == "keyboard":
            KeyboardMode()
        elif CONFIG_MODE.lower() == "usbdevice":
            USBDeviceMode();

#################################################################
#
# main
#
# DEBUG mode : production mode if button pressed (to ground)
# normal mode : production mode if button not pressed (left up)
if CONFIG_DEBUG:
    production_mode = (button.value == False)
else:
    production_mode = (button.value == True)

if production_mode:
    Production()

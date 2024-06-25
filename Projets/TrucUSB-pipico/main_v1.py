import usb_hid
from adafruit_hid.mouse import Mouse

mouse = Mouse(usb_hid.devices)
mouse.move(10,0)
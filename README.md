# OSExt
OSExt (temporary name, would like help to name it), aims to be an parallel OS for the TI nspire.<br>

finished:
- bringing back the miniclock in the top right, without flickering this time
- making drivers for the lcd, keypad and touchpad

Planned for the future (in order of priority):
short-term:
- starting work on a kernel api for programs to use

mid-term:
- making a desktop
- making a proper file brower, the integrated one is too slow
- making the desktop use the wayland protocol
- adding a settings program for the settings of OSExt, as it should now have enough settings that it's worth doing that
- make OSExt replace an existing ndless installation, because it interferes with the lcd compatibility feature of ndless anyways
- expose the same functions as ndless does, do keep existing programs working
- porting newlib to my kernel, and add my kernel api to it

long-term:
- make a usb driver and fat32 driver
- make a swap partition driver, so memory can be swapped to a usb device,<br>
so a program is (in theory) only limited to the ~3GB of the address space the kernel doesn't use
- drivers for usb hubs, mouse and kerboard
- usb device driver, so the calculators can communicate with each other while programs are running
- using the device driver, make a distributed computing server, so work can be outsourced to other calculators running OSExt connected by USB








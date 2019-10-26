# OSExt
A small project I started to enhance the functionality of the TI-Nspire os, because it turns out linux doesn't support my new hardware revision.
(see wiki for updated informations)
look in config.h to configure the project and run make to compile
look into the module folders for readmes

current/planned modules:
Clock:
A miniclock in the top left of the screen. I made this because Levak's NClock miniclock doesn't work on newer os versions.
Currently it's flickering but I'm working on it.

Desktop:
I plan to make a full, expandable graphical dektop for the nspire, but havn't started to work on it jet.

Shell:
I also plan to make a full shell for the nspire, to wich programms can send commands wia custom system calls. Also not started jet :)

usb:
My ultimate goal is to make a working usb wifi driver for the nspire, but I didin't find any good documentation on it and can't even get to get my functions called, let alone send the right data back or read the received data.

disablenavnet:
This is mainly planned for the security module. It allows to shut down the os navnet services for receiving files, os updates, send device data, essentially disables the os's ability to communicate with other calculators. The current version oly allows to disable the services, not re-enable them. to do that you would have to reboot.

security:
This module will be like the login manager on linux. It will ask you for a password whenfirst loaded (for example as a startup programm), after some time of inactivity and before entering press-to-test-mode, because that disables startup programms and would <del>enable you to give your calculator to other people</del> allow anyone to execute code on it (by installing ndless).



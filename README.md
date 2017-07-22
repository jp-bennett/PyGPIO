# PyGPIO
Python extension for GPIO
This extension allows accessing the kernel GPIO Character device interface.  I've been testing this extension against Fedora 26 on a Raspberry Pi, Kernel 4.11.

This was created partially as a response to the Raspberry Pi library not working on a vanilla kernel, as well as the deprecation of the sysfs gpio driver.

This extension is based on the gpio-utils in the Linux source.

At this time, there are four functions, gpio.init, gpio.set_values, gpio.get_values, and gpio.release.  

gpio.init takes 4 arguments, the gpiochip to target, a numeric direction indicator, and two lists.  The first list defines which gpio pins to request, while the second list sets the default values for those pins.

This function returns a file descriptor as an int.
Example use:
fd = gpio.init("gpiochip0", 1, [20, 21], [1, 0])

gpio.set_values  takes two args, the file descriptor set earlier, and a list of values to set the pins to.
Example:
gpio.set_values(fd, [0, 1])

gpio.get_values takes the file descriptor and the number of pins to check.
Example:
gpio.get_values(fd, 2)

And finally gpio.release takes just the file descriptor, and releases the gpio pins.
Example:
gpio.release(fd)

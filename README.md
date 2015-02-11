# Driving OLEDs based on the SSD1322 chipset

I recently started working with a [Newhaven NHD-3.12-25664UCB2 OLED display](http://www.newhavendisplay.com/nhd31225664ucb2-p-3622.html). With trial, error, and [some very helpful example code](http://www.newhavendisplay.com/NHD_forum/index.php/topic,64.0.html) I managed to get this display working with an Arduino (so far) using 3-Wire (software), 4-Wire (software), and 4-Wire via SPI (hardware) signalling. I increased the performance of the software-based protocol demos significantly by using low-level I/O port writes, but the SPI mode is FAR superior to either (and is probably as good as it gets for the Mega - the Due should have a lot more headroom in this regard).

This *should* work for various SSD1322 displays (e.g., others in the Newhaven NHD-x.xx-25664 series).

Setup: I used an Arduino Mega 2560 with the latest Arduino 1.5.8 IDE. I used a 74LVC245 for voltage translaton. The display was wired per the serial specs for it, with the level shifted Arduino pins going to the corresponding board pins. The reference voltage for the '245 and the display was 3.3V. (Never apply 5V directly to this display!)

**Further reading:**

Specifications: http://www.newhavendisplay.com/specs/NHD-3.12-25664UCB2.pdf

Chipset app notes: http://www.newhavendisplay.com/app_notes/SSD1322.pdf

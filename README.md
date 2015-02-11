# Driving OLEDs based on the SSD1322 chipset

I recently started working with a [Newhaven NHD-3.12-25664UCB2 OLED display](http://www.newhavendisplay.com/nhd31225664ucb2-p-3622.html). With trial, error, and [some very helpful example code](http://www.newhavendisplay.com/NHD_forum/index.php/topic,64.0.html) I managed to get this display working with an Arduino (so far) using 3-Wire and 4-Wire signalling. I also managed to increase the performance of the demo significantly by using low-level I/O port writes.

This should work for any regular SSD-1322 display, but at least for Newhaven NHD-x.xx-25664 OLED displays.

**Further reading:**

Specifications: http://www.newhavendisplay.com/specs/NHD-3.12-25664UCB2.pdf

Chipset app notes: http://www.newhavendisplay.com/app_notes/SSD1322.pdf





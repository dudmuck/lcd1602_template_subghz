# nordic sidewalk template_subghz with added 16x2 I2C-LCD
Uses [Nordic sidewalk SDK](https://github.com/nrfconnect/sdk-sidewalk) with [16x2 LCD](https://wiki.52pi.com/index.php?title=Z-0234).  For purpose to see the difference of network coverage FSK vs LoRa.

This is a zephy freestanding application.  Be sure to build this from the shell started by nordic's toolchain manager, so``ZEPHYR_BASE``environment is defined. 

be sure you have first built and run the template_subghz sample as supplied by nordic.
## modifications
* Button 3 (short press) hello message enables ACK.  The RSSI and SNR of the ACK is printed on LCD when received.
* Button 1 (short press) changes to FSK sidewalk mode if currently in LoRa sidewalk mode,  If out of range, it wont show going into FSK until within range to hear FSK signal from amazon.
* Button 4 (short press) changes to LoRa sidewalk mode if currently in FSK mode


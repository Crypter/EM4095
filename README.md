# EM4095
Simple reader sketch for Arduino IDE for ESP8266. Can be ported, just requires changing the timings.

To run this crap:  
-You need ESP8266  
-The ESP8266 module needs to run at 160MHz  
-It can read 125KHz cards that use EM4100 / EM4200 protocols and Manchester encoding at 64 cycles per bit (which is 98% of the cards used in the world)  
-RF module that uses EM4095 chip (I used this one - https://www.mikroe.com/rfid-reader-board)  

Wiring it up:  
-Just use the DEMOD_OUT pin on any input, the rest is power and the other of the pins are connected to ground. CLK is not connected at all.  

"IT DOESN'T WORk!!1! FIX IT!"  
-No.

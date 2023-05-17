# Espressif Distant Alarm
This project is a fun experiment on multitasking using FreeRTOS, Bluetooth communication, string processing, simple pixel graphics, bitmap fonts, as well as the basic electronics involved. The core of the system is the Espressif ESP32 system on a chip microcontroller, whose function is to get push notification data from a smartphone via Bluetooth. With a display, buzzer, mini vibration motor, and pink LEDs, getting messages from special people becomes even more joyful. 
![breadboard](https://github.com/dawidogg/eda/assets/39916800/89cb7d79-562a-459c-aa0a-fa71b13d7024)

# Inspiration
I always liked these round little pagers given in restaurants, which ring as soon as the order is done. The tunes they play, the flashes, the vibration, really bring the moment of gratification. Instead of stealing one, I decided I can make my own with the functions I want, that is announcing immediately if someone important has texted. The idea for Espressif Distant Alarm came to me long before I knew what a microcontroller was.
![pagers](https://github.com/dawidogg/eda/assets/39916800/614dd135-7bec-4381-b1b5-8fd747c0954a)

# Circuit
I am planning on drawing an explicit schematic later on when designing a PCB. In general, the circuit is simple, there are just some parts attached to the microcontroller:
- 1.8 inch 160x128 RGB display (6 pins, SPI communication);
- 74HC595N serial-in, parallel-out 8-bit shift register, with five pink LEDs attached to the outputs Q1-Q5 (3 pins);
- N-channel MOSFET which triggers a mini vibration motor (1 pin);
- Mercury tilt sensor with the other end connected to the ground (1 pin);
- Buzzer (1 pin).

The circuit is powered by a rechargeable 3.7 V battery, and there is a charging module on the breadboard with a Type-C input.

# Microcontroller program
To be able to use Bluetooth and Adafruit display libraries, I decided to go with the Arduino platform instead of the native to my microcontroller ESP-IDF. Another library, or a whole operating system I used is FreeRTOS, which enabled me to create parallel tasks. The main file initializes modules and has crucial functions to respond to the inputs and interface with other components. There are separate tasks for display, buzzer with motor, and LEDs, which work synchronously. The display module must print text and constantly animate lines, without blocking or interrupting the buzzer tune or other things. Bluetooth module has its task too, and it sends interrupts to the main when data is coming. Communication between tasks was not done with the rich tools an RTOS can offer, such as queues, semaphores, mutexes, etc. I used only task notify functions (specific to FreeRTOS, alternative for binary semaphores) and global variables for this. 

The display was the most complex thing to work on. First, I didn't like how Adafruit allowed only 128 ASCII characters for printing. People on my phone have either Turkish or Russian names, so I needed to add these alphabets somehow. I tried many options, and the solution I came up with was to edit a .BDF font, and squeeze into 128 glyphs the characters I needed by deleting unnecessary symbols, even the lowercase letters. Adafruit provided a Python script to convert a .BDF font into a C++ header file. Now that I had all glyphs I wanted in my project environment, I wrote a module to translate a regular string into a string that can be processed by the Adafruit library, mapping non-ASCII characters to ASCII. Overall a terrible solution, but I could not have done better with the tool I had. The other thing I didn't like in Adafruit library was that the text wrapping function was too primitive, it was just translating characters to a new line whenever there was no space left (actually it was even wasting one space, very annoying), without any sense of words. I wrote an algorithm to fit words without breaking them if unnecessary. Also, my algorithm calculates printing positions to make text perfectly aligned to the center. On the text's background, there are animated lines, optimized to avoid redrawing the same pixels and writing over the text. As for the startup screen, there is a simple animation, something I can describe as a rotating slice of pie. The round shape was achieved using triangles.

All other features are better observed from the examples but before that...

# Android application
My aim in this project is to learn microcontroller programming, and learning mobile development is out of this scope. For this reason, I allowed myself to just use [someone's code](https://github.com/franzandel/NotificationListenerTutorial) and modify it with ChatGPT. The code originally was a tutorial to show how Android's notification listener service works, and what kind of data can be extracted the moment a push notification arrives. The data I needed was just the title, which is the name of a person if the application is a messenger. I created a list of apps I want to trigger Espressif Distant Alarm, which includes WhatsApp, Instagram, Facebook, Discord, Telegram, Gmail, and SMS application. If the notification arrived from one of these, I send the title to my device via Bluetooth. The current implementation includes only this, therefore I have no many files to share. I included NotificationService.kt file as a reference for someone who would like to contribute to the project with an application. A simple GUI for managing the whitelist of persons, some additional processing done otherwise by the microcontroller, and maybe even programmable features like changing the pattern of ringing.

# Examples
Startup screen, when the power is switched on (or when there is a runtime error and the device reboots, yikes).
https://github.com/dawidogg/eda/assets/39916800/6e3b9538-dd47-426c-be24-bbf1a201cbcd

Bluetooth connection. The user can interact with the device using only one action, that is tilting it. To confirm a pairing request, one should tilt the device, and to reject, one should keep the device still for 7 seconds.
https://github.com/dawidogg/eda/assets/39916800/65f7c5a8-1f81-41d5-8387-c9f4f783f10c

Incoming message. To shut down the alarm, a tilt is enough.
https://github.com/dawidogg/eda/assets/39916800/26615b7e-c18d-4f91-a189-f911829ffe18

# Contribution
This project lacks PCB design, 3D enclosure design, and mobile application, and therefore open to your contribution. Contact me if you want to work together on the remaining parts.  

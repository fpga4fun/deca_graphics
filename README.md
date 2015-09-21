This is a graphics project for Arrow/Terasic Altera MAX10 Deca board.
The graphics is implemented as the frame buffer in VIP (Altera Video and Image Processing IP Suite).

Quartus 15.0 for Linux Update 2, the most recent version as of 09/2015, is used for hardware design and HMI software demonstration.

Deca has a 50MHz external crystal. The system clock is set to 100MHz, while DDR3 is driven in 300MHz.
The maximum HDMI resolution the system can support is 1280x800.

Up to two Frame Reader IPs can be used in the design for Deca board because of resource limitation.
When the third frame reader is added to the design, Quartus Analysis & Synthesis (namely quartus_map) will crash.

The design is using 640x480 and 800x480 pixels frame readers, although 1024x768 pixels frame reader is verified working.

Some key decisions used in the design:
DDR3 memory bus data width is 64 bit.
VIP Frame Buffer port width is set to 256.
VFRs' CPR (color plane sequencer) is not set to havle control packed width.

Ting Cao
09/20/2015




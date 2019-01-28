APP = app_usbloop1.elf
BOARD = uno
USBO = busby.o cdc.o
POOL1 = nee
POOL2 = nee
POOL3 = nee
POOL4 = nee

ifeq ($(BOARD), mega)
PART = m2560
MMCU = atmega2560
BSP = mega.o misc.o
USBOPT =
POOL1 = ja
POOL2 = nee
POOL3 = ja
POOL4 = ja
else ifeq ($(BOARD), ocho)
PART = m8
MMCU = atmega8
BSP = uno.o misc.o
USBOPT =
POOL1 = nee
POOL2 = nee
POOL3 = nee
POOL4 = nee
else ifeq ($(BOARD), leonardo)
PART = m32u4
MMCU = atmega32u4
BSP = leonardo.o misc.o
USBOPT = $(USBO)
POOL1 = nee
POOL2 = ja
POOL3 = ja
POOL4 = ja
else ifeq ($(BOARD), teensy20pp)
PART = usb1286
MMCU = at90usb1286
BSP = teensy20pp.o misc.o
POOL1 = ja
POOL2 = nee
POOL3 = ja
POOL4 = ja
else ifeq ($(BOARD), zestien)
PART = 16u2
MMCU = atmega16u2
BSP = zestien.o misc.o
USBOPT = $(USB0)
POOL1 = nee
POOL2 = ja
POOL3 = nee
POOL4 = nee
else ifeq ($(BOARD), uno)
PART = m328p
MMCU = atmega328p
BSP = uno.o misc.o
USBOPT = 
POOL1 = ja
POOL2 = nee
POOL3 = ja
POOL4 = ja
else
PART = m328p
MMCU = atmega328p
BSP = uno.o misc.o
USBOPT = 
POOL1 = ja
POOL2 = nee
POOL3 = ja
POOL4 = ja
endif

TARGETS = app_blink1.elf app_blink3.elf \
    app_calc2.elf app_pi1.elf app_servoknob1.elf \
    app_blink4.hex app_analog1.elf \
    app_megaboot4.hex app_sdmbr1.elf \
    app_miniboot1.hex app_optiboot1.hex app_heliosboot1.hex app_test2.elf \
    app_test1.elf app_i2cscan1.elf app_uartloop1.elf app_segment1.elf app_uartloop2.elf \
    app_lcdtest2.elf app_lcdtest3.elf app_lcdtest1.elf app_pcf8563test2.elf \
    app_ringtone1.elf app_speckb1.elf app_stepper1.elf

ifeq ($(POOL1), ja)
TARGETS += app_pong1.elf app_fourinone.elf app_pirate1.elf app_vga2.elf
endif

ifeq ($(POOL2), ja)
TARGETS += app_usbtest1.elf app_usbsd2.elf \
    app_serialusb1.elf app_usbloop1.elf \
    app_usbsound1.elf app_usbmidi1.elf app_usbjoy1.elf app_usbkb1.elf \
    app_usbpiano1.elf app_rndis1.elf app_rndis2.elf \
    app_rndisping1.elf app_rndisbridge1.elf app_groen1.elf app_ledmatrix1.elf
endif

ifeq ($(POOL3), ja)
TARGETS += app_ts2.elf app_calc1.elf app_ds1302test1.elf \
    app_dfkeyb1.elf app_ts1.elf \
    app_aditbox.elf app_capsense2.elf
endif

ifeq ($(POOL4), ja)
TARGETS += app_webtest1w5100.elf app_nslookup1.elf app_telnetloop1.elf \
    app_chatserver1.elf app_websdfat2.elf app_bbs1.elf app_websdfat1.elf \
    app_webtest1enc28.elf app_minos1.elf app_minos2.elf app_webclient2.elf app_sdls1.elf \
    app_infrared1.elf app_ps2kb2.elf app_sound1.elf app_vga1.elf
endif

%.o: %.cpp
	avr-g++ -O2 -Wall -Wno-strict-aliasing -mmcu=$(MMCU) -std=c++11 -c -o $@ $<

%.elf: %.o
	avr-g++ -mmcu=$(MMCU) -o $@ $^

%.hex: %.asm
	avra $< > /dev/null

%.hex:
	avr-objcopy -O ihex -R .flash $< $@

.PHONY: all

all: $(TARGETS)

app_miniboot1.hex: app_miniboot1.asm
app_blink4.hex: app_blink4.asm
app_heliosboot1.hex: app_heliosboot1.asm
app_megaboot4.hex: app_megaboot4.asm
app_optiboot1.hex: app_optiboot1.asm

app_webtest1enc28.elf: app_webtest1enc28.o uart.o enc28j60tcp.o stream.o webtest1.o \
    enc28j60ip.o dhcp.o eth.o enc28j60udp.o enc28j60hw.o

app_webtest1w5100.elf: app_webtest1w5100.o w5100ip.o uart.o pinport.o stream.o dhcp.o \
    w5100udp.o w5100tcp.o w5100hw.o misc.o eth.o webtest1.o $(USBOPT)

app_telnetloop1.elf: app_telnetloop1.o uart.o enc28j60tcp.o pinport.o \
    enc28j60ip.o dhcp.o enc28j60udp.o enc28j60hw.o misc.o stream.o eth.o

app_bbs1.elf: app_bbs1.o dns.o uart.o enc28j60tcp.o pinport.o enc28j60ip.o dhcp.o \
    enc28j60udp.o enc28j60hw.o misc.o stream.o fatty.o zd2card.o eth.o

app_websdfat1.elf: app_websdfat1.o enc28j60tcp.o enc28j60ip.o dhcp.o enc28j60udp.o \
    enc28j60hw.o eth.o zd2card.o fatty.o pinport.o uart.o stream.o misc.o webserver.o

app_websdfat2.elf: app_websdfat2.o w5100ip.o uart.o pinport.o stream.o dhcp.o w5100udp.o \
    w5100tcp.o w5100hw.o zd2card.o fatty.o misc.o eth.o webserver.o

app_chatserver1.elf: app_chatserver1.o w5100ip.o w5100tcp.o \
    w5100hw.o w5100udp.o dhcp.o misc.o pinport.o eth.o stream.o uart.o

app_nslookup1.elf: app_nslookup1.o dns.o uart.o enc28j60tcp.o \
    enc28j60ip.o dhcp.o enc28j60udp.o enc28j60hw.o stream.o eth.o

app_webclient2.elf: app_webclient2.o w5100tcp.o w5100ip.o w5100hw.o w5100udp.o \
    dhcp.o dns.o eth.o stream.o uart.o

app_minos1.elf: app_minos1.o fatty.o zd2card.o pinport.o xmodem.o ymodem.o \
    stream.o md5sum.o uart.o misc.o

app_minos2.elf: app_minos2.o fatty.o zd2card.o pinport.o xmodem.o ymodem.o \
    stream.o md5sum.o uart.o misc.o w5100hw.o w5100ip.o dhcp.o w5100tcp.o \
    w5100udp.o eth.o

app_aditbox.elf: app_aditbox.o analog.o button.o tft.o pinport.o uart.o misc.o
app_analog1.elf: app_analog1.o pinport.o uart.o stream.o misc.o
app_blink1.elf: app_blink1.o
app_blink3.elf: app_blink3.o pinport.o
app_calc1.elf: app_calc1.o analog.o button.o tft.o calc.o pinport.o misc.o
app_calc2.elf: app_calc2.o calc.o pinport.o uart.o misc.o
app_capsense2.elf: app_capsense2.o capsense.o pinport.o uart.o stream.o misc.o $(USBOPT)
app_dfkeyb1.elf: app_dfkeyb1.o pinport.o uart.o stream.o misc.o analog.o lcd.o
app_ds1302test1.elf: app_ds1302test1.o pinport.o uart.o stream.o misc.o lcd.o
app_fourinone.elf: app_fourinone.o analog.o pinport.o
app_groen1.elf: app_groen1.o
app_i2cscan1.elf: app_i2cscan1.o i2c.o pinport.o uart.o misc.o stream.o $(USBOPT)
app_infrared1.elf: app_infrared1.o infrared.o pinport.o uart.o stream.o misc.o
app_lcdtest1.elf: app_lcdtest1.o lcd.o pinport.o uart.o misc.o
app_lcdtest2.elf: app_lcdtest2.o
app_lcdtest3.elf: app_lcdtest3.o
app_ledmatrix1.elf: app_ledmatrix1.o
app_ledmatrix2.elf: app_ledmatrix2.o pinport.o $(BSP)
app_pcf8563test2.elf: app_pcf8563test2.o i2c.o pinport.o uart.o misc.o stream.o $(USBOPT)
app_pi1.elf: app_pi1.o uart.o stream.o $(USBOPT)
app_pirate1.elf: app_pirate1.o vgax.o
app_pong1.elf: app_pong1.o
app_ps2kb2.elf: app_ps2kb2.o keyboard.o pinport.o uart.o stream.o misc.o $(USBOPT)
app_ringtone1.elf: app_ringtone1.o pinport.o uart.o stream.o misc.o
app_rndis1.elf: app_rndis1.o busby.o
app_rndis2.elf: app_rndis2.o
app_rndisping1.elf: app_rndisping1.o busby.o
app_rndisbridge1.elf: app_rndisbridge1.o busby.o w5100hw.o
app_servoknob1.elf: app_servoknob1.o
app_sdls1.elf: app_sdls1.o fatty.o zd2card.o pinport.o uart.o stream.o misc.o $(USBOPT)
app_sdmbr1.elf: app_sdmbr1.o zd2card.o pinport.o uart.o stream.o misc.o $(USBOPT)
app_segment1.elf: app_segment1.o
app_serialusb1.elf: app_serialusb1.o pinport.o uart.o stream.o $(USBOPT) $(BSP)
app_sound1.elf: app_sound1.o
app_speckb1.elf: app_speckb1.o uart.o
app_stepper1.elf: app_stepper1.o
app_test1.elf: app_test1.o
app_test2.elf: app_test2.o
app_ts1.elf: app_ts1.o analog.o pinport.o uart.o misc.o stream.o
app_ts2.elf: app_ts2.o analog.o pinport.o uart.o stream.o $(BSP)
app_uartloop1.elf: app_uartloop1.o pinport.o uart.o misc.o
app_uartloop2.elf: app_uartloop2.o pinport.o uart.o misc.o
app_usbloop1.elf: app_usbloop1.o pinport.o uart.o $(USBOPT) $(BSP)
app_usbmidi1.elf: app_usbmidi1.o busby.o pinport.o uart.o $(BSP)
app_usbsd2.elf: app_usbsd2.o usbsd.o zd2card.o busby.o pinport.o uart.o $(BSP)
app_usbtest1.elf: app_usbtest1.o pinport.o uart.o stream.o $(USBOPT) $(BSP)
app_usbjoy1.elf: app_usbjoy1.o analog.o misc.o usbjoy.o busby.o pinport.o uart.o $(BSP)
app_usbkb1.elf: app_usbkb1.o usbkb.o busby.o pinport.o uart.o $(BSP)
app_usbpiano1.elf: app_usbpiano1.o capsense.o busby.o pinport.o uart.o $(BSP)
app_usbsound1.elf: app_usbsound1.o busby.o pinport.o uart.o misc.o
app_vga1.elf: app_vga1.o
app_vga2.elf: app_vga2.o vga.o

app_bbs1.o: app_bbs1.cpp stream.h dhcp.h enc28j60udp.h enc28j60ip.h \
    enc28j60hw.h misc.h types.h

app_minos1.o: app_minos1.cpp fatty.h fatty_util.h misc.h types.h \
    stream.h md5sum.h xmodem.h ymodem.h storage.h pinport.h

app_minos2.o: app_minos2.cpp fatty.h fatty_util.h misc.h stream.h types.h \
    md5sum.h xmodem.h ymodem.h storage.h pinport.h

app_webtest1enc28.o: app_webtest1enc28.cpp misc.h board.h stream.h dhcp.h \
    enc28j60ip.h enc28j60udp.h enc28j60hw.h types.h webtest1.h uart.h \
    uno.h mega.h leonardo.h storage.h pinport.h enc28j60tcp.h \
    eth.h cdc.h busby.h

app_webtest1w5100.o: app_webtest1w5100.cpp webtest1.h stream.h board.h \
    cdc.h busby.h board.h mega.h uno.h leonardo.h \
    misc.h storage.h pinport.h types.h w5100ip.h w5100hw.h eth.h client.h \
    udp.h w5100udp.h w5100tcp.h uart.h dhcp.h

app_websdfat1.o: app_websdfat1.cpp fatty.h webserver.h dhcp.h types.h storage.h \
    enc28j60udp.h enc28j60tcp.h board.h misc.h stream.h zd2card.h pgmspees.h \
    cdc.h uart.h busby.h

app_websdfat2.o: app_websdfat2.cpp dhcp.h webserver.h \
    w5100tcp.h fatty.h zd2card.h board.h uno.h mega.h leonardo.h misc.h \
    stream.h types.h w5100udp.h udp.h w5100hw.h \
    w5100ip.h eth.h client.h pgmspees.h uart.h cdc.h busby.h storage.h pinport.h

app_telnetloop1.o: app_telnetloop1.cpp enc28j60tcp.h enc28j60udp.h stream.h misc.h \
    dhcp.h board.h uno.h leonardo.h mega.h teensy20pp.h pinport.h types.h

app_aditbox.o: app_aditbox.cpp misc.h
app_analog1.o: app_analog1.cpp misc.h
app_dfkeyb1.o: app_dfkeyb1.cpp misc.h
app_blink1.o: app_blink1.cpp board.h uno.h leonardo.h mega.h misc.h storage.h pinport.h types.h
app_blink3.o: app_blink3.cpp misc.h
app_calc1.o: app_calc1.cpp misc.h
app_calc2.o: app_calc2.cpp misc.h
app_chatserver1.o: app_chatserver1.cpp
app_ds1302test1.o: app_ds1302test1.cpp misc.h
app_fourinone.o: app_fourinone.cpp
app_groen1.o: app_groen1.cpp
app_i2cscan1.o: app_i2cscan1.cpp misc.h
app_i2cscan2.o: app_i2cscan2.cpp misc.h
app_infrared1.o: app_infrared1.cpp
app_lcdtest1.o: app_lcdtest1.cpp
app_lcdtest2.o: app_lcdtest2.cpp
app_lcdtest3.o: app_lcdtest3.cpp
app_ledmatrix1.o: app_ledmatrix1.cpp
app_ledmatrix2.o: app_ledmatrix2.cpp
app_pcf8563test2.o: app_pcf8563test2.cpp misc.h
app_ringtone1.o: app_ringtone1.cpp
app_rndis1.o: app_rndis1.cpp busby.h
app_rndis2.o: app_rndis2.cpp
app_rndisping1.o: app_rndisping1.cpp busby.h misc.h
app_rndisbridge1.o: app_rndisbridge1.cpp busby.h enc28j60hw.h
app_segment1.o: app_segment1.cpp
app_sdls1.o: app_sdls1.cpp zd2card.h fatty.h stream.h
app_sdmbr1.o: app_sdmbr1.cpp zd2card.h
app_serialusb1.o: app_serialusb1.cpp misc.h
app_servoknob1.o: app_servoknob1.cpp
app_sound1.o: app_sound1.cpp
app_speckb1.o: app_speckb1.cpp
app_stepper1.o: app_stepper1.cpp
app_test1.o: app_test1.cpp
app_test2.o: app_test2.cpp
app_ts1.o: app_ts1.cpp
app_ts2.o: app_ts2.cpp analog.h misc.h
app_usbloop1.o: app_usbloop1.cpp misc.h busby.h
app_usbsd2.o: app_usbsd2.cpp busby.h misc.h
app_usbtest1.o: app_usbtest1.cpp
app_uartloop1.o: app_uartloop1.cpp misc.h
app_uartloop2.o: app_uartloop2.cpp misc.h
app_usbjoy1.o: app_usbjoy1.cpp
app_usbpiano1.o: app_usbpiano1.cpp busby.h
app_usbsound1.o: app_usbsound1.cpp
app_vga1.o: app_vga1.cpp
app_vga2.o: app_vga2.cpp
app_webclient2.o: app_webclient2.cpp
analog.o: analog.cpp analog.h
busby.o: busby.cpp busby.h schip.h pgmspees.h int.h types.h board.h leonardo.h uno.h
button.o: button.cpp button.h analog.h tft.h
calc.o: calc.cpp calc.h types.h
capsense.o: capsense.cpp capsense.h
eth.o: eth.cpp eth.h misc.h types.h
enc28j60hw.o: enc28j60hw.cpp
enc28j60ip.o: enc28j60ip.cpp enc28j60ip.h types.h
enc28j60tcp.o: enc28j60tcp.cpp enc28j60tcp.h types.h
enc28j60udp.o: enc28j60udp.cpp enc28j60udp.h types.h
fatty.o: fatty.cpp fatty.h stream.h uart.h cdc.h busby.h types.h
helios.o: helios.cpp helios.h misc.h
i2c.o: i2c.cpp misc.h types.h
infrared.o: infrared.cpp infrared.h types.h
keyboard.o: keyboard.cpp keyboard.h board.h uno.h leonardo.h mega.h pinport.h types.h
lcd.o: lcd.cpp lcd.h
mega.o: mega.cpp misc.h types.h storage.h pinport.h
md5sum.o: md5sum.cpp md5sum.cpp stream.h uart.h types.h
misc.o: misc.cpp misc.h types.h storage.h pinport.h
pinport.o: pinport.cpp pinport.h types.h
stream.o: stream.cpp stream.h uart.h cdc.h busby.h types.h
tft.o: tft.cpp tft.h board.h leonardo.h uno.h mega.h teensy20pp.h types.h pinport.h
teensy20pp.o: teensy20pp.cpp teensy20pp.h types.h pinport.h
uart.o: uart.cpp uart.h types.h misc.h storage.h pinport.h
uno.o: uno.cpp uno.h misc.h types.h storage.h pinport.h
usbkb.o: usbkb.cpp usbkb.h misc.h usbhid.h types.h
usbsd.o: usbsd.cpp usbsd.h busby.h types.h
vga.o: vga.cpp vga.h misc.h storage.h pinport.h types.h
vgax.o: vgax.cpp vgax.h
w5100hw.o: w5100hw.cpp w5100hw.h
w5100tcp.o: w5100tcp.cpp w5100tcp.h
w5100ip.o: w5100ip.cpp w5100ip.h w5100hw.h eth.h
w5100udp.o: w5100udp.cpp w5100udp.h
webserver.o: webserver.cpp webserver.h
webtest1.o: webtest1.cpp webtest1.h
xmodem.o: xmodem.cpp xmodem.h stream.h busby.h cdc.h types.h board.h
ymodem.o: ymodem.cpp ymodem.h stream.h types.h
zd2card.o: zd2card.cpp zd2card.h

arduino: $(APP)
	avrdude -c arduino -p $(PART) -P /dev/ttyACM0 -U $<

wiring: $(APP)
	avrdude -c wiring -p $(PART) -P /dev/ttyACM0 -U $<

download: $(APP)
	avrdude -c stk500 -p $(PART) -P /dev/ttyUSB0 -U $<

clean:
	rm -Rvf *.o *.hex *.elf *.obj *.cof

rebuild: clean all


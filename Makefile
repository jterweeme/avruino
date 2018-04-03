APP = app_usbloop1.elf
BOARD = mega
USBO = busby.o cdc.o
POOL1 = nee
POOL2 = nee

ifeq ($(BOARD), helios)
PART = m32u4
MMCU = atmega32u4
BSP = helios.o misc.o
BOARDDEF = HELIOS
USBOPT = $(USBO)
POOL2 = ja
else ifeq ($(BOARD), mega)
PART = m2560
MMCU = atmega2560
BSP = mega.o misc.o
BOARDDEF = MEGA
USBOPT =
POOL1 = ja
else ifeq ($(BOARD), ocho)
PART = m8
MMCU = atmega8
BSP = uno.o misc.o
BOARDDEF = UNO
USBOPT =
else ifeq ($(BOARD), leonardo)
PART = m32u4
MMCU = atmega32u4
BSP = leonardo.o misc.o
BOARDREF = LEONARDO
USBOPT = $(USBO)
POOL2 = ja
else
PART = m328p
MMCU = atmega328p
BSP = uno.o misc.o
BOARDDEF = UNO
USBOPT = 
POOL1 = ja
endif

TARGETS = app_aditbox.elf app_capsense2.elf app_sdmbr1.elf app_sdls1.elf \
    app_blink1.elf app_test1.elf app_i2cscan1.elf \
    app_pcf8563test2.elf app_lcdtest1.elf app_pi1.elf \
    app_test2.elf app_calc1.elf app_calc2.elf app_ts2.elf \
    app_lcdtest2.elf app_infrared1.elf app_lcdtest3.elf app_ps2kb2.elf \
    app_analog2.elf app_ringtone1.elf app_uartloop1.elf app_segment1.elf app_uartloop2.elf \
    app_blink3.elf app_megaboot4.hex app_analogweb1.elf app_websdfat1.elf \
    app_optiboot1.hex app_heliosboot1.hex \

ifeq ($(POOL1), ja)
TARGETS += app_ds1302test1.elf app_analog1.elf app_ts1.elf app_vga1.elf \
    app_vga2.elf app_blink4.hex app_sound1.elf app_pong1.elf \
    app_fourinone.elf app_pirate1.elf app_minos1.elf
endif

ifeq ($(POOL2), ja)
TARGETS += app_groen1.elf app_usbtest1.elf app_usbsd2.elf app_usbsound2.hex \
    app_ledmatrix1.elf app_serialusb1.elf app_usbloop1.elf \
    app_usbsound1.elf app_usbmidi1.elf app_usbjoy1.elf app_usbkb1.elf \
    app_usbpiano1.elf app_rndis1.elf app_rndis2.elf \
    app_rndisping1.elf app_rndisbridge1.elf
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

app_blink4.hex: app_blink4.asm
app_heliosboot1.hex: app_heliosboot1.asm
app_megaboot4.hex: app_megaboot4.asm
app_optiboot1.hex: app_optiboot1.asm
app_usbsound2.hex: app_usbsound2.asm

app_analogweb1.elf: app_analogweb1.o arp.o dns.o uip_server.o \
    uart.o uip_client.o pinport.o \
    uip.o dhcp.o uip_ethernet.o uip_udp.o enc28j60.o uip_timer.o mempool.o $(BSP)

app_websdfat1.elf: app_websdfat1.o arp.o dns.o uip_server.o uip_client.o \
    uip.o dhcp.o uip_ethernet.o uip_udp.o enc28j60.o uip_timer.o mempool.o \
    zd2card.o fatty.o pinport.o uart.o stream.o $(BSP)

app_minos1.elf: app_minos1.o fatty.o zd2card.o pinport.o xmodem.o ymodem.o \
stream.o md5sum.o uart.o $(BSP)

app_aditbox.elf: app_aditbox.o analog.o button.o tft.o pinport.o uart.o $(BSP)
app_analog1.elf: app_analog1.o pinport.o uart.o $(BSP)
app_analog2.elf: app_analog2.o pinport.o uart.o $(BSP)
app_blink1.elf: app_blink1.o
app_blink3.elf: app_blink3.o pinport.o uart.o $(BSP)
app_calc1.elf: app_calc1.o analog.o button.o tft.o calc.o pinport.o uart.o $(BSP)
app_calc2.elf: app_calc2.o calc.o pinport.o uart.o $(BSP)
app_capsense2.elf: app_capsense2.o capsense.o pinport.o uart.o $(USBOPT) $(BSP)
app_ds1302test1.elf: app_ds1302test1.o pinport.o uart.o $(BSP)
app_fourinone.elf: app_fourinone.o analog.o
app_groen1.elf: app_groen1.o
app_i2cscan1.elf: app_i2cscan1.o i2c.o pinport.o uart.o $(USBOPT) $(BSP)
app_infrared1.elf: app_infrared1.o infrared.o pinport.o uart.o stream.o $(BSP)
app_lcdtest1.elf: app_lcdtest1.o pinport.o uart.o $(BSP)
app_lcdtest2.elf: app_lcdtest2.o
app_lcdtest3.elf: app_lcdtest3.o
app_ledmatrix1.elf: app_ledmatrix1.o
app_ledmatrix2.elf: app_ledmatrix2.o pinport.o $(BSP)
app_pcf8563test2.elf: app_pcf8563test2.o i2c.o pinport.o uart.o $(USBOPT) $(BSP)
app_pi1.elf: app_pi1.o pinport.o uart.o $(USBOPT) $(BSP)
app_pirate1.elf: app_pirate1.o vgax.o
app_pong1.elf: app_pong1.o
app_ps2kb2.elf: app_ps2kb2.o keyboard.o pinport.o uart.o $(USBOPT) $(BSP)
app_ringtone1.elf: app_ringtone1.o pinport.o uart.o $(BSP)
app_rndis1.elf: app_rndis1.o busby.o
app_rndis2.elf: app_rndis2.o
app_rndisping1.elf: app_rndisping1.o busby.o
app_rndisbridge1.elf: app_rndisbridge1.o busby.o enc28j60.o mempool.o
app_sdls1.elf: app_sdls1.o fatty.o zd2card.o pinport.o uart.o stream.o $(USBOPT) $(BSP)
app_sdmbr1.elf: app_sdmbr1.o zd2card.o pinport.o uart.o stream.o $(USBOPT) $(BSP)
app_segment1.elf: app_segment1.o
app_serialusb1.elf: app_serialusb1.o pinport.o uart.o stream.o $(USBOPT) $(BSP)
app_sound1.elf: app_sound1.o
app_test1.elf: app_test1.o pinport.o uart.o $(BSP)
app_test2.elf: app_test2.o
app_ts1.elf: app_ts1.o analog.o pinport.o uart.o $(BSP)
app_ts2.elf: app_ts2.o analog.o pinport.o uart.o stream.o $(BSP)
app_uartloop1.elf: app_uartloop1.o pinport.o uart.o $(BSP)
app_uartloop2.elf: app_uartloop2.o pinport.o uart.o $(BSP)
app_usbloop1.elf: app_usbloop1.o pinport.o uart.o $(USBOPT) $(BSP)
app_usbmidi1.elf: app_usbmidi1.o busby.o pinport.o uart.o $(BSP)
app_usbsd2.elf: app_usbsd2.o usbsd.o zd2card.o busby.o pinport.o uart.o $(BSP)
app_usbtest1.elf: app_usbtest1.o pinport.o uart.o $(USBOPT) $(BSP)
app_usbjoy1.elf: app_usbjoy1.o analog.o misc.o usbjoy.o busby.o pinport.o uart.o $(BSP)
app_usbkb1.elf: app_usbkb1.o usbkb.o busby.o pinport.o uart.o $(BSP)
app_usbpiano1.elf: app_usbpiano1.o capsense.o busby.o pinport.o uart.o $(BSP)
app_usbsound1.elf: app_usbsound1.o busby.o pinport.o uart.o $(BSP)
app_vga1.elf: app_vga1.o
app_vga2.elf: app_vga2.o vga.o

app_aditbox.o: app_aditbox.cpp misc.h
app_analog1.o: app_analog1.cpp misc.h
app_analog2.o: app_analog2.cpp misc.h
app_analogweb1.o: app_analogweb1.cpp
app_blink1.o: app_blink1.cpp misc.h
app_blink3.o: app_blink3.cpp misc.h
app_calc1.o: app_calc1.cpp misc.h
app_calc2.o: app_calc2.cpp misc.h
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

app_minos1.o: app_minos1.cpp fatty.h misc.h types.h \
stream.h md5sum.h xmodem.h ymodem.h storage.h pinport.h

app_pcf8563test2.o: app_pcf8563test2.cpp misc.h
app_ringtone1.o: app_ringtone1.cpp
app_rndis1.o: app_rndis1.cpp busby.h
app_rndis2.o: app_rndis2.cpp
app_rndisping1.o: app_rndisping1.cpp bogota.h
app_rndisbridge1.o: app_rndisbridge1.cpp bogota.h busby.h enc28j60.h
app_segment1.o: app_segment1.cpp
app_sdls1.o: app_sdls1.cpp zd2card.h fatty.h stream.h
app_sdmbr1.o: app_sdmbr1.cpp zd2card.h
app_serialusb1.o: app_serialusb1.cpp helios.h misc.h
app_sound1.o: app_sound1.cpp
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
app_websdfat1.o: app_websdfat1.cpp fatty.h uip_server.h
analog.o: analog.cpp analog.h
bogota.o: bogota.cpp bogota.h busby.h
busby.o: busby.cpp
button.o: button.cpp button.h analog.h tft.h
calc.o: calc.cpp calc.h types.h
capsense.o: capsense.cpp capsense.h
fatty.o: fatty.cpp fatty.h
helios.o: helios.cpp helios.h misc.h
i2c.o: i2c.cpp misc.h
infrared.o: infrared.cpp infrared.h
keyboard.o: keyboard.cpp
mega.o: mega.cpp misc.h types.h storage.h pinport.h
mempool.o: mempool.cpp mempool.h uip.h types.h
md5sum.o: md5sum.cpp md5sum.cpp stream.h uart.h types.h
misc.o: misc.cpp misc.h types.h storage.h pinport.h
pinport.o: pinport.cpp pinport.h types.h
stream.o: stream.cpp stream.h
tft.o: tft.cpp tft.h board.h
uart.o: uart.cpp uart.h types.h misc.h storage.h pinport.h
uno.o: uno.cpp uno.h misc.h types.h storage.h pinport.h
usbkb.o: usbkb.cpp usbkb.h misc.h usbhid.h
usbsd.o: usbsd.cpp usbsd.h busby.h
vga.o: vga.cpp vga.h
vgax.o: vgax.cpp vgax.h
xmodem.o: xmodem.cpp xmodem.h stream.h types.h
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


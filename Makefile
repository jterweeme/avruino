APP = app_usbloop1.elf
BOARD = uno
USBO = busby.o

ifeq ($(BOARD), helios)
PART = m32u4
MMCU = atmega32u4
BSP = helios.o misc.o
BOARDDEF = HELIOS
USBOPT = $(USBO)
else ifeq ($(BOARD), mega)
PART = m2560
MMCU = atmega2560
BSP = mega.o misc.o
BOARDDEF = MEGA
USBOPT =
else ifeq ($(BOARD), ocho)
PART = m8
MMCU = atmega8
BSP = uno.o misc.o
BOARDDEF = UNO
USBOPT =
else
PART = m328p
MMCU = atmega328p
BSP = uno.o misc.o
BOARDDEF = UNO
USBOPT = 
endif

TARGETS = app_blink1.elf app_hello.elf app_test1.elf app_timer1.elf app_i2cscan1.elf \
    app_pcf8563test1.elf app_pcf8563test2.elf app_blink2.elf app_lcdtest1.elf \
    app_test2.elf app_calc1.elf app_calc2.elf \
    app_lcdtest2.elf app_infrared1.elf app_lcdtest3.elf \
    app_analog2.elf app_ringtone1.elf app_uartloop1.elf app_segment1.elf app_uartloop2.elf \
    app_blink3.elf app_megaboot4.hex \
    app_optiboot1.hex app_heliosboot1.hex

ifeq ($(BOARD), uno)
TARGETS += app_ds1302test1.elf app_analog1.elf app_sd1.elf app_sd2.elf app_sd3.elf app_sd4.elf \
    app_vga1.elf app_vga2.elf app_lyrics1.elf app_blink4.hex 
endif

ifeq ($(BOARD), helios)
TARGETS += app_groen1.elf app_i2cscan2.elf app_usbtest1.elf app_wifi1.elf \
    app_ledmatrix1.elf app_ledmatrix2.elf app_serialusb1.elf app_usbloop1.elf
endif

%.o: %.cpp
	avr-g++ -Os -mmcu=$(MMCU) -std=c++11 -c -o $@ $<

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

app_analog1.elf: app_analog1.o $(BSP)
app_analog2.elf: app_analog2.o $(BSP)
app_blink1.elf: app_blink1.o
app_blink2.elf: app_blink2.o $(BSP)
app_blink3.elf: app_blink3.o $(BSP)
app_calc1.elf: app_calc1.o analog.o button.o tft.o calc.o $(BSP)
app_calc2.elf: app_calc2.o calc.o $(BSP)
app_ds1302test1.elf: app_ds1302test1.o $(BSP)
app_groen1.elf: app_groen1.o
app_hello.elf: app_hello.o $(BSP)
app_i2cscan1.elf: app_i2cscan1.o $(BSP)
app_i2cscan2.elf: app_i2cscan2.o $(USBOPT) $(BSP)
app_infrared1.elf: app_infrared1.o infrared.o $(BSP)
app_lcdtest1.elf: app_lcdtest1.o $(BSP)
app_lcdtest2.elf: app_lcdtest2.o
app_lcdtest3.elf: app_lcdtest3.o
app_ledmatrix1.elf: app_ledmatrix1.o
app_ledmatrix2.elf: app_ledmatrix2.o $(BSP)
app_lyrics1.elf: app_lyrics1.o sd.o $(BSP)
app_pcf8563test1.elf: app_pcf8563test1.o $(USBOPT) $(BSP)
app_pcf8563test2.elf: app_pcf8563test2.o $(USBOPT) $(BSP)
app_ringtone1.elf: app_ringtone1.o $(BSP)
app_sd1.elf: app_sd1.o $(UBSOPT) $(BSP)
app_sd2.elf: app_sd2.o
app_sd3.elf: app_sd3.o sd.o $(BSP)
app_sd4.elf: app_sd4.o sd.o $(BSP)
app_segment1.elf: app_segment1.o
app_serialusb1.elf: app_serialusb1.o $(USBOPT) $(BSP)
app_test1.elf: app_test1.o $(BSP)
app_test2.elf: app_test2.o
app_timer1.elf: app_timer1.o $(BSP)
app_usbloop1.elf: app_usbloop1.o $(USBOPT) $(BSP)
app_usbtest1.elf: app_usbtest1.o $(USBOPT) $(BSP)
app_uartloop1.elf: app_uartloop1.o $(BSP)
app_uartloop2.elf: app_uartloop2.o $(BSP)
app_vga1.elf: app_vga1.o
app_vga2.elf: app_vga2.o vga.o
app_wifi1.elf: app_wifi1.o misc.o $(USBOPT) $(BSP)

app_analog1.o: app_analog1.cpp misc.h
app_analog2.o: app_analog2.cpp misc.h
app_blink1.o: app_blink1.cpp misc.h
app_blink2.o: app_blink2.cpp misc.h
app_blink3.o: app_blink3.cpp misc.h
app_calc1.o: app_calc1.cpp misc.h
app_calc2.o: app_calc2.cpp misc.h
app_ds1302test1.o: app_ds1302test1.cpp misc.h
app_groen1.o: app_groen1.cpp
app_hello.o: app_hello.cpp misc.h
app_i2cscan1.o: app_i2cscan1.cpp misc.h
app_i2cscan2.o: app_i2cscan2.cpp misc.h
app_infrared1.o: app_infrared1.cpp
app_lcdtest1.o: app_lcdtest1.cpp
app_lcdtest2.o: app_lcdtest2.cpp
app_lcdtest3.o: app_lcdtest3.cpp
app_ledmatrix1.o: app_ledmatrix1.cpp
app_ledmatrix2.o: app_ledmatrix2.cpp
app_lyrics1.o: app_lyrics1.cpp
app_pcf8563test1.o: app_pcf8563test1.cpp misc.h
app_pcf8563test2.o: app_pcf8563test2.cpp misc.h
app_ringtone1.o: app_ringtone1.cpp
app_segment1.o: app_segment1.cpp
app_sd1.o: app_sd1.cpp misc.h board.h
app_sd2.o: app_sd2.cpp
app_sd3.o: app_sd3.cpp
app_sd4.o: app_sd4.cpp
app_serialusb1.o: app_serialusb1.cpp helios.h misc.h
app_test1.o: app_test1.cpp
app_test2.o: app_test2.cpp
app_timer1.o: app_timer1.cpp misc.h
app_usbloop1.o: app_usbloop1.cpp misc.h busby.h
app_usbtest1.o: app_usbtest1.cpp
app_uartloop1.o: app_uartloop1.cpp misc.h
app_uartloop2.o: app_uartloop2.cpp misc.h
app_vga1.o: app_vga1.cpp
app_vga2.o: app_vga2.cpp
app_wifi1.o: app_wifi1.cpp misc.h
analog.o: analog.cpp analog.h
busby.o: busby.cpp
button.o: button.cpp
calc.o: calc.cpp
DeviceStandardReq.o: DeviceStandardReq.cpp
endpoint.o: endpoint.cpp
events.o: events.cpp
helios.o: helios.cpp helios.h misc.h
infrared.o: infrared.cpp infrared.h
mega.o: mega.cpp misc.h
misc.o: misc.cpp misc.h
sd.o: sd.cpp
tft.o: tft.cpp tft.h
uno.o: uno.cpp uno.h misc.h
vga.o: vga.cpp vga.h

arduino: $(APP)
	avrdude -c arduino -p $(PART) -P /dev/ttyACM0 -U $<

download: $(APP)
	avrdude -c stk500 -p $(PART) -P /dev/ttyUSB0 -U $<

clean:
	rm -Rvf *.o *.hex *.elf *.obj *.cof

rebuild: clean all


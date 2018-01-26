APP = app_usbloop1.elf
BOARD = leonardo
USBO = busby.o cdc.o

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
else ifeq ($(BOARD), leonardo)
PART = m32u4
MMCU = atmega32u4
BSP = leonardo.o misc.o
BOARDREF = LEONARDO
USBOPT = $(USBO)
else
PART = m328p
MMCU = atmega328p
BSP = uno.o misc.o
BOARDDEF = UNO
USBOPT = 
endif

TARGETS = app_aditbox.elf \
    app_blink1.elf app_hello.elf app_test1.elf app_timer1.elf app_i2cscan1.elf \
    app_pcf8563test2.elf app_blink2.elf app_lcdtest1.elf \
    app_test2.elf app_calc1.elf app_calc2.elf app_sdod1.elf app_ts2.elf \
    app_lcdtest2.elf app_infrared1.elf app_lcdtest3.elf app_ps2kb2.elf \
    app_analog2.elf app_ringtone1.elf app_uartloop1.elf app_segment1.elf app_uartloop2.elf \
    app_blink3.elf app_megaboot4.hex \
    app_optiboot1.hex app_heliosboot1.hex

ifeq ($(BOARD), uno)
TARGETS += app_ds1302test1.elf app_analog1.elf app_ts1.elf \
    app_vga1.elf app_vga2.elf app_blink4.hex app_sound1.elf
endif

ifeq ($(BOARD), helios)
TARGETS += app_groen1.elf app_usbtest1.elf app_wifi1.elf \
    app_ledmatrix1.elf app_ledmatrix2.elf app_serialusb1.elf app_usbloop1.elf
endif

ifeq ($(BOARD), leonardo)
TARGETS += app_groen1.elf app_usbtest1.elf app_usbsd2.elf app_usbsound2.hex \
    app_ledmatrix1.elf app_ledmatrix2.elf app_serialusb1.elf app_usbloop1.elf \
    app_usbsound1.elf app_pi1.elf app_midi1.elf app_usbjoy1.elf app_usbkb1.elf
endif

%.o: %.cpp
	avr-g++ -O2 -Wall -mmcu=$(MMCU) -std=c++11 -c -o $@ $<

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

app_aditbox.elf: app_aditbox.o analog.o button.o tft.o $(BSP)
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
app_i2cscan1.elf: app_i2cscan1.o i2c.o $(USBOPT) $(BSP)
app_i2cscan2.elf: app_i2cscan2.o i2c.o $(USBOPT) $(BSP)
app_infrared1.elf: app_infrared1.o infrared.o $(BSP)
app_lcdtest1.elf: app_lcdtest1.o $(BSP)
app_lcdtest2.elf: app_lcdtest2.o
app_lcdtest3.elf: app_lcdtest3.o
app_ledmatrix1.elf: app_ledmatrix1.o
app_ledmatrix2.elf: app_ledmatrix2.o $(BSP)
app_pcf8563test2.elf: app_pcf8563test2.o i2c.o $(USBOPT) $(BSP)
app_pi1.elf: app_pi1.o $(USBOPT) $(BSP)
app_ps2kb2.elf: app_ps2kb2.o keyboard.o $(USBOPT) $(BSP)
app_ringtone1.elf: app_ringtone1.o $(BSP)
app_sdod1.elf: app_sdod1.o zd2card.o $(BSP)
app_segment1.elf: app_segment1.o
app_serialusb1.elf: app_serialusb1.o $(USBOPT) $(BSP)
app_sound1.elf: app_sound1.o
app_test1.elf: app_test1.o $(BSP)
app_test2.elf: app_test2.o
app_timer1.elf: app_timer1.o $(BSP)
app_ts1.elf: app_ts1.o analog.o $(BSP)
app_ts2.elf: app_ts2.o analog.o $(BSP)
app_usbloop1.elf: app_usbloop1.o $(USBOPT) $(BSP)
app_midi1.elf: app_midi1.o busby.o $(BSP)
app_usbsd2.elf: app_usbsd2.o usbsd.o zd2card.o busby.o $(BSP)
app_usbtest1.elf: app_usbtest1.o $(USBOPT) $(BSP)
app_uartloop1.elf: app_uartloop1.o $(BSP)
app_uartloop2.elf: app_uartloop2.o $(BSP)
app_usbjoy1.elf: app_usbjoy1.o analog.o misc.o usbjoy.o busby.o $(BSP)
app_usbkb1.elf: app_usbkb1.o usbkb.o busby.o $(BSP)
app_usbsound1.elf: app_usbsound1.o busby.o $(BSP)
app_vga1.elf: app_vga1.o
app_vga2.elf: app_vga2.o vga.o
app_wifi1.elf: app_wifi1.o misc.o $(USBOPT) $(BSP)

app_aditbox.o: app_aditbox.cpp misc.h
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
app_pcf8563test2.o: app_pcf8563test2.cpp misc.h
app_ringtone1.o: app_ringtone1.cpp
app_segment1.o: app_segment1.cpp
app_sdod1.o: app_sdod1.cpp zd2card.h
app_serialusb1.o: app_serialusb1.cpp helios.h misc.h
app_sound1.o: app_sound1.cpp
app_test1.o: app_test1.cpp
app_test2.o: app_test2.cpp
app_timer1.o: app_timer1.cpp misc.h
app_ts1.o: app_ts1.cpp
app_ts2.o: app_ts2.cpp analog.h misc.h
app_usbloop1.o: app_usbloop1.cpp misc.h busby.h
app_usbsd2.o: app_usbsd2.cpp busby.h misc.h
app_usbtest1.o: app_usbtest1.cpp
app_uartloop1.o: app_uartloop1.cpp misc.h
app_uartloop2.o: app_uartloop2.cpp misc.h
app_usbjoy1.o: app_usbjoy1.cpp
app_usbsound1.o: app_usbsound1.cpp
app_vga1.o: app_vga1.cpp
app_vga2.o: app_vga2.cpp
app_wifi1.o: app_wifi1.cpp misc.h
analog.o: analog.cpp analog.h
busby.o: busby.cpp
button.o: button.cpp
calc.o: calc.cpp
helios.o: helios.cpp helios.h misc.h
i2c.o: i2c.cpp misc.h
infrared.o: infrared.cpp infrared.h
keyboard.o: keyboard.cpp
mega.o: mega.cpp misc.h
misc.o: misc.cpp misc.h
sd.o: sd.cpp
tft.o: tft.cpp tft.h
uno.o: uno.cpp uno.h misc.h
usbkb.o: usbkb.cpp usbkb.h
usbsd.o: usbsd.cpp usbsd.h busby.h
vga.o: vga.cpp vga.h
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


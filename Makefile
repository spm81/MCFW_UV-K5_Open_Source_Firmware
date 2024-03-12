#=================================================================#
# FIRMWARE BUILD OPTIONS #  1: Enable  # 0: Disable               #
#=================================================================#

TARGET = firmware


#======== STOCK QUANSHENG FERATURES ========#
ENABLE_AIRCOPY 							:= 1
# 3856 bytes
ENABLE_FMRADIO							:= 1
# 84 bytes
ENABLE_FLASHLIGHT_SOS       			:= 1
ENABLE_MISSED_CALL_NOTIFICATION_AND_BLINKING_LED := 1
ENABLE_UART                 			:= 1
ENABLE_UART_CAT             			:= 0
# Bause we can cut more... - 4108 bytes
ENABLE_DTMF_CALLING         			:= 0
#DTMF REMOTEKILL
ENABLE_REMOTEKILL						:= 0
# 1750Hz & 1050Hz FN1 FN2 Tones
ENABLE_DTMF_SIDETONES				    := 1
ENABLE_TX1750 							:= 0
# Keep this in stock options, and add option in mods for extra rogers
ENABLE_ROGER_DEFAULT					:= 0
ENABLE_ROGER_MOTOTRBO					:= 0
ENABLE_ROGER_TPT						:= 0
ENABLE_ROGER_MOTOTRBOT40				:= 0
ENABLE_ROGER_MOTOTRBOTLKRT80			:= 0
ENABLE_ROGER_ROGERCOBRAAM845			:= 1
ENABLE_ROGER_POLICE_ITA					:= 0
ENABLE_ROGER_UV5RC						:= 0
ENABLE_ROGER_MARIO						:= 0
ENABLE_MDC                  			:= 0

#============== MODIFICATIONS =============#
# AM Modulation Fix - 544 bytes
ENABLE_AM_FIX 								:= 1
# Apply fix to Spectrum - 40 bytes
ENABLE_AM_FIX_ON_SPECTRUM					:= 0
ENABLE_SQUELCH_MORE_SENSITIVE				:= 1
# Restore FM in 1 second after RX - 0 bytes
ENABLE_FMRADIO_FAST_RESTORE 				:= 1
# Scan List Editor
ENABLE_SCANLIST								:= 1

# Battery percentage at the Welcome Message - 12 bytes
ENABLE_VOLTAGE_PERCENTAGE_WELCOME_MESSAGE	:= 1
# Battery percentage - 296 bytes
ENABLE_STATUS_BATTERY_PERC 		 			:= 1
# Show current while charging - 136 bytes Thanks Tunas1337
ENABLE_BATTERY_CHARGING						:= 1
# Invert LCD Colors
ENABLE_LCD_INVERT_OPTION					:= 0 
#ENABLE_LCD_CONTRAST_OPTION 			 	:= 0 # WIP
# Mic Gain Bar while TXing - 255 bytes
ENABLE_MIC_PLUS_GAIN_BAR_TX 				:= 1
# Enable Vox 1920ms(max) delay - 0 bytes
ENABLE_VOX_MAX_DELAY						:= 1
ENABLE_NOSCANTIMEOUT    	    			:= 1
ENABLE_KEEPNAMEONSAVE       				:= 1
ENABLE_FASTER_CHANNEL_SCAN  				:= 1
# Enable Timeout beep at the end of timeout - 44 bytes
ENABLE_TIMEOUT_ROGERBEEP_NOTIFICATION		:= 1
# CW Modulation
ENABLE_CW                   				:= 1

#=============== EXTRA: MESSENGER ===============# 
ENABLE_MESSENGER            				:= 1
ENABLE_MESSENGER_MORE_ONE_LINE				:= 1
# 124 bytes
ENABLE_MESSENGER_SHOW_RX_FREQ				:= 1
# 124 (+20) bytes
ENABLE_MESSENGER_SHOW_RX_TX_FREQ			:= 0
# 156 bytes
ENABLE_MESSENGER_UART						:= 1
# 3408 bytes
ENABLE_MESSENGER_ENCRYPTION             	:= 1
# 140 bytes
ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION 	:= 1

# -------------- EXTRA: SPECTRUM --------------
ENABLE_SPECTRUM            		 			:= 1
ENABLE_SPECTRUM_NUNU           				:= 0
SPECTRUM_AUTOMATIC_SQUELCH  				:= 1
SPECTRUM_EXTRA_VALUES       				:= 1
# 3568 bytes
ENABLE_ALL_REGISTERS        				:= 0
# 0 bytes
ENABLE_MATOZ_KEYS           				:= 1

# https://github.com/nicsure/QuanshengDock
# ---- EXTRA: DOCK ----
# 2204 bytes
ENABLE_DOCK 	                  			:= 0

#Thanks to KD8CEC for sharing his code / We have to check the code better, i just code & paste it to the right places...
# 1476 bytes
ENABLE_LIVESEEK_MHZ_KEYPAD					:= 0


# ---- DEBUGGING ----
# ---- COMPILER/LINKER OPTIONS ----
ENABLE_COMPRESS								:= 1
ENABLE_OVERLAY 								:= 0
ENABLE_SWD 									:= 0


















BSP_DEFINITIONS := $(wildcard hardware/*/*.def)
BSP_HEADERS := $(patsubst hardware/%,bsp/%,$(BSP_DEFINITIONS))
BSP_HEADERS := $(patsubst %.def,%.h,$(BSP_HEADERS))

OBJS =
# Startup files
OBJS += start.o
OBJS += init.o
ifeq ($(ENABLE_OVERLAY),1)
OBJS += sram-overlay.o
endif
OBJS += external/printf/printf.o

# Drivers
OBJS += driver/adc.o
ifeq ($(ENABLE_UART),1)
OBJS += driver/aes.o
endif
ifeq ($(ENABLE_MESSENGER),1)
	OBJS += app/messenger.o
endif
OBJS += driver/backlight.o
ifeq ($(ENABLE_FMRADIO),1)
OBJS += driver/bk1080.o
endif
OBJS += driver/bk4819.o
ifeq ($(filter $(ENABLE_AIRCOPY) $(ENABLE_UART),1),1)
OBJS += driver/crc.o
endif
OBJS += driver/eeprom.o
ifeq ($(ENABLE_OVERLAY),1)
OBJS += driver/flash.o
endif
OBJS += driver/gpio.o
OBJS += driver/i2c.o
OBJS += driver/keyboard.o
OBJS += driver/spi.o
OBJS += driver/st7565.o
OBJS += driver/system.o
OBJS += driver/systick.o
ifeq ($(ENABLE_UART),1)
OBJS += driver/uart.o
endif

# Main
OBJS += app/action.o
ifeq ($(ENABLE_AM_FIX),1)
OBJS += am_fix.o
endif
ifeq ($(ENABLE_AM_FIX_ON_SPECTRUM),1)
OBJS += am_fix.o
endif
ifeq ($(ENABLE_AIRCOPY),1)
OBJS += app/aircopy.o
endif
OBJS += app/app.o
OBJS += app/dtmf.o
OBJS += app/finput.o
ifeq ($(ENABLE_FMRADIO),1)
OBJS += app/fm.o
endif
OBJS += app/generic.o
OBJS += app/main.o
OBJS += app/menu.o
OBJS += app/appmenu.o
OBJS += app/scanner.o
ifeq ($(ENABLE_SPECTRUM), 1)
OBJS += app/spectrum.o
endif
ifeq ($(ENABLE_SPECTRUM_NUNU), 1)
OBJS += app/spectrumnunu.o
endif
ifeq ($(ENABLE_UART),1)
OBJS += app/uart.o
endif
ifeq ($(ENABLE_SCANLIST),1)
OBJS += apps/scanlist.o
endif

OBJS += audio.o
OBJS += bitmaps.o
OBJS += board.o
OBJS += dcs.o
OBJS += font.o
OBJS += frequencies.o
OBJS += functions.o
OBJS += helper/battery.o
OBJS += helper/boot.o
OBJS += helper/measurements.o
OBJS += misc.o
OBJS += radio.o
OBJS += scheduler.o
OBJS += settings.o
ifeq ($(ENABLE_AIRCOPY),1)
OBJS += ui/aircopy.o
endif
OBJS += ui/battery.o
ifeq ($(ENABLE_FMRADIO),1)
OBJS += ui/fmradio.o
endif
ifeq ($(ENABLE_MESSENGER),1)
	OBJS += ui/messenger.o
endif
ifeq ($(ENABLE_MESSENGER_ENCRYPTION),1)
	OBJS += external/chacha/chacha.o
	OBJS += helper/crypto.o
endif
ifeq ($(ENABLE_LIVESEEK_MHZ_KEYPAD),1)
OBJS += app/ceccommon.o
endif
OBJS += ui/helper.o
OBJS += ui/inputbox.o
OBJS += ui/lock.o
OBJS += ui/main.o
OBJS += ui/menu.o
OBJS += ui/appmenu.o
OBJS += ui/rssi.o
OBJS += ui/scanner.o
OBJS += ui/status.o
OBJS += ui/ui.o
OBJS += ui/welcome.o
OBJS += version.o
OBJS += main.o

ifeq ($(OS),Windows_NT)
	TOP := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
	RM = del /Q
	FixPath = $(subst /,\,$1)
	WHERE = where
	K5PROG = utils/k5prog/k5prog.exe -F -YYYYY -p /dev/com3 -b
else
	TOP := $(shell pwd)
	RM = rm -f
	FixPath = $1
	WHERE = which
	K5PROG = utils/k5prog/k5prog -F -YYYYY -p /dev/ttyUSB3 -b
endif


ifneq (, $(shell $(WHERE) python))
	MY_PYTHON := python
else ifneq (, $(shell $(WHERE) python3))
	MY_PYTHON := python3
endif

ifdef MY_PYTHON
	HAS_CRCMOD := $(shell $(MY_PYTHON) -c "import crcmod" 2>&1)
endif

ifndef MY_PYTHON
$(info )
$(info !!!!!!!! PYTHON NOT FOUND, *.PACKED.BIN WON'T BE BUILT)
$(info )
else ifneq (,$(HAS_CRCMOD))
$(info )
$(info !!!!!!!! CRCMOD NOT INSTALLED, *.PACKED.BIN WON'T BE BUILT)
$(info !!!!!!!! run: pip install crcmod)
$(info )
endif

AS = arm-none-eabi-gcc
CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

GIT_HASH := $(shell git rev-parse --short HEAD)

ASFLAGS = -c -mcpu=cortex-m0
ifeq ($(ENABLE_OVERLAY),1)
ASFLAGS += -DENABLE_OVERLAY
endif

#CFLAGS = -Os -Wall -Werror -mcpu=cortex-m0 -fno-builtin -fshort-enums -fno-delete-null-pointer-checks -std=c11 -MMD
CFLAGS = -Os -Wall -Werror -mcpu=cortex-m0 -fno-delete-null-pointer-checks -std=c11 -MMD
CFLAGS += -DPRINTF_INCLUDE_CONFIG_H
CFLAGS += -DGIT_HASH=\"$(GIT_HASH)\"
ifeq ($(ENABLE_COMPRESS),1)
CFLAGS += --specs=nosys.specs --specs=nano.specs
CFLAGS += -flto=auto
CFLAGS += -ftree-vectorize -funroll-loops
endif



ifeq ($(ENABLE_DOCK),1)
	CFLAGS += -DENABLE_DOCK
endif
ifeq ($(ENABLE_LCD_INVERT_OPTION),1)
	CFLAGS += -DENABLE_LCD_INVERT_OPTION
endif
ifeq ($(ENABLE_AIRCOPY),1)
	CFLAGS += -DENABLE_AIRCOPY
endif
ifeq ($(ENABLE_DTMF_CALLING),1)
	CFLAGS += -DENABLE_DTMF_CALLING
endif
ifeq ($(ENABLE_REMOTEKILL),1)
	CFLAGS += -DENABLE_REMOTEKILL
endif
ifeq ($(ENABLE_DTMF_SIDETONES),1)
	CFLAGS += -DENABLE_DTMF_SIDETONES
endif
ifeq ($(ENABLE_MIC_PLUS_GAIN_BAR_TX),1)
	CFLAGS += -DENABLE_MIC_PLUS_GAIN_BAR_TX
endif
ifeq ($(ENABLE_VOX_MAX_DELAY),1)
	CFLAGS  += -DENABLE_VOX_MAX_DELAY
endif
ifeq ($(ENABLE_FLASHLIGHT_SOS),1)
	CFLAGS  += -DENABLE_FLASHLIGHT_SOS
endif
ifeq ($(ENABLE_AM_FIX),1)
	CFLAGS  += -DENABLE_AM_FIX
endif
ifeq ($(ENABLE_AM_FIX_ON_SPECTRUM),1)
	CFLAGS  += -DENABLE_AM_FIX_ON_SPECTRUM
endif
ifeq ($(ENABLE_SQUELCH_MORE_SENSITIVE),1)
	CFLAGS  += -DENABLE_SQUELCH_MORE_SENSITIVE
endif
ifeq ($(ENABLE_FMRADIO),1)
	CFLAGS += -DENABLE_FMRADIO
endif
ifeq ($(ENABLE_FMRADIO_FAST_RESTORE),1)
	CFLAGS += -DENABLE_FMRADIO_FAST_RESTORE
endif
ifeq ($(ENABLE_TIMEOUT_ROGERBEEP_NOTIFICATION),1)
	CFLAGS += -DENABLE_TIMEOUT_ROGERBEEP_NOTIFICATION
endif
ifeq ($(ENABLE_ROGER_DEFAULT),1)
	CFLAGS += -DENABLE_ROGER_DEFAULT
endif
ifeq ($(ENABLE_ROGER_MOTOTRBO),1)
	CFLAGS += -DENABLE_ROGER_MOTOTRBO
endif
ifeq ($(ENABLE_ROGER_TPT),1)
	CFLAGS += -DENABLE_ROGER_TPT
endif
ifeq ($(ENABLE_ROGER_MOTOTRBOT40),1)
	CFLAGS += -DENABLE_ROGER_MOTOTRBOT40
endif
ifeq ($(ENABLE_ROGER_MOTOTRBOTLKRT80),1)
	CFLAGS += -DENABLE_ROGER_MOTOTRBOTLKRT80
endif
ifeq ($(ENABLE_ROGER_ROGERCOBRAAM845),1)
	CFLAGS += -DENABLE_ROGER_ROGERCOBRAAM845
endif
ifeq ($(ENABLE_ROGER_POLICE_ITA),1)
	CFLAGS += -DENABLE_ROGER_POLICE_ITA
endif
ifeq ($(ENABLE_ROGER_UV5RC),1)
	CFLAGS += -DENABLE_ROGER_UV5RC
endif
ifeq ($(ENABLE_ROGER_MARIO),1)
	CFLAGS += -DENABLE_ROGER_MARIO
endif
ifeq ($(ENABLE_MDC),1)
	CFLAGS += -DENABLE_MDC
endif
ifeq ($(ENABLE_OVERLAY),1)
	CFLAGS += -DENABLE_OVERLAY
endif
ifeq ($(ENABLE_SPECTRUM),1)
	CFLAGS += -DENABLE_SPECTRUM
endif
ifeq ($(ENABLE_SPECTRUM_NUNU),1)
	CFLAGS += -DENABLE_SPECTRUM_NUNU
endif
ifeq ($(ENABLE_SWD),1)
	CFLAGS += -DENABLE_SWD
endif
ifeq ($(ENABLE_TX1750),1)
	CFLAGS += -DENABLE_TX1750
endif
ifeq ($(ENABLE_VOLTAGE_PERCENTAGE_WELCOME_MESSAGE),1)
	CFLAGS += -DENABLE_VOLTAGE_PERCENTAGE_WELCOME_MESSAGE
endif
ifeq ($(ENABLE_STATUS_BATTERY_PERC),1)
	CFLAGS += -DENABLE_STATUS_BATTERY_PERC
endif
ifeq ($(ENABLE_LCD_CONTRAST_OPTION),1)
	CFLAGS  += -DENABLE_LCD_CONTRAST_OPTION
endif
ifeq ($(ENABLE_UART),1)
	CFLAGS += -DENABLE_UART
endif
ifeq ($(ENABLE_SCANLIST),1)
	CFLAGS += -DENABLE_SCANLIST
endif



#LDFLAGS += -z noexecstack -mcpu=cortex-m0 -nostartfiles -Wl,-L,linker -Wl,-T,firmware.ld -Wl,--gc-sections
LDFLAGS = -mcpu=cortex-m0 -nostartfiles -Wl,-T,firmware.ld
## 2 LDFLAGS = -mcpu=cortex-m0 -nostartfiles -Wl,-T,firmware.ld,--gc-sections

ifeq ($(ENABLE_COMPRESS),1)
LDFLAGS += --specs=nano.specs
LDFLAGS += -flto
endif
##LDFLAGS += --specs=nosys.specs --specs=nano.specs 

ifeq ($(ENABLE_MISSED_CALL_NOTIFICATION_AND_BLINKING_LED),1)
	CFLAGS += -DENABLE_MISSED_CALL_NOTIFICATION_AND_BLINKING_LED
endif
ifeq ($(ENABLE_NOSCANTIMEOUT),1)
	CFLAGS += -DENABLE_NOSCANTIMEOUT
endif
ifeq ($(ENABLE_KEEPNAMEONSAVE),1)
	CFLAGS += -DENABLE_KEEPNAMEONSAVE
endif
ifeq ($(ENABLE_FASTER_CHANNEL_SCAN),1)
	CFLAGS  += -DENABLE_FASTER_CHANNEL_SCAN
endif
ifeq ($(ENABLE_ALL_REGISTERS),1)
	CFLAGS += -DENABLE_ALL_REGISTERS
endif
ifeq ($(ENABLE_UART_CAT),1)
	CFLAGS += -DENABLE_UART_CAT
endif
ifeq ($(SPECTRUM_AUTOMATIC_SQUELCH),1)
	CFLAGS += -DSPECTRUM_AUTOMATIC_SQUELCH
endif
ifeq ($(SPECTRUM_EXTRA_VALUES),1)
	CFLAGS += -DSPECTRUM_EXTRA_VALUES
endif
ifeq ($(ENABLE_MATOZ_KEYS),1)
	CFLAGS += -DENABLE_MATOZ_KEYS
endif
ifeq ($(ENABLE_MESSENGER),1)
	CFLAGS += -DENABLE_MESSENGER
endif
ifeq ($(ENABLE_MESSENGER_MORE_ONE_LINE),1)
	CFLAGS += -DENABLE_MESSENGER_MORE_ONE_LINE
endif
ifeq ($(ENABLE_MESSENGER_SHOW_RX_FREQ),1)
	CFLAGS += -DENABLE_MESSENGER_SHOW_RX_FREQ
endif
ifeq ($(ENABLE_MESSENGER_SHOW_RX_TX_FREQ),1)
	CFLAGS += -DENABLE_MESSENGER_SHOW_RX_TX_FREQ
endif
ifeq ($(ENABLE_UART), 0)
	ENABLE_MESSENGER_UART := 0
endif
ifeq ($(ENABLE_MESSENGER_UART),1)
	CFLAGS += -DENABLE_MESSENGER_UART
endif
ifeq ($(ENABLE_MESSENGER_ENCRYPTION),1)
	CFLAGS  += -DENABLE_MESSENGER_ENCRYPTION
endif
ifeq ($(ENABLE_MESSENGER_ROGERBEEP_NOTIFICATION),1)
	CFLAGS += -DENABLE_MESSENGER_ROGERBEEP_NOTIFICATION
endif
ifeq ($(ENABLE_CW),1)
	CFLAGS += -DENABLE_CW
endif
ifeq ($(ENABLE_BATTERY_CHARGING),1)
	CFLAGS += -DENABLE_BATTERY_CHARGING
endif
ifeq ($(ENABLE_LIVESEEK_MHZ_KEYPAD),1)
	CFLAGS += -DENABLE_LIVESEEK_MHZ_KEYPAD
endif




ifeq ($(DEBUG),1)
ASFLAGS += -g
CFLAGS += -g
LDFLAGS += -g
endif

INC =
INC += -I $(TOP)
INC += -I $(TOP)/external/CMSIS_5/CMSIS/Core/Include/
INC += -I $(TOP)/external/CMSIS_5/Device/ARM/ARMCM0/Include

#show size
LDFLAGS += -Wl,--print-memory-usage

LIBS =

DEPS = $(OBJS:.o=.d)

all: $(TARGET)
	@echo Create $(notdir $<.bin)
	@$(OBJCOPY) -O binary $< $<.bin
	@echo Create $(notdir $<.packed.bin)
	@-$(MY_PYTHON) utils/fw-pack.py $<.bin MATOZ $(GIT_HASH) $<.packed.bin

debug:
	/opt/openocd/bin/openocd -c "bindto 0.0.0.0" -f interface/jlink.cfg -f dp32g030.cfg

flash:
	/opt/openocd/bin/openocd -c "bindto 0.0.0.0" -f interface/jlink.cfg -f dp32g030.cfg -c "write_image firmware.bin 0; shutdown;"

version.o: .FORCE

$(TARGET): $(OBJS)
	@echo LD $@
	@$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

bsp/dp32g030/%.h: hardware/dp32g030/%.def

%.o: %.c | $(BSP_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@

%.o: %.S
	@echo AS $<
	@$(AS) $(ASFLAGS) $< -o $@

.FORCE:

-include $(DEPS)

clean:
	$(RM) $(call FixPath, $(TARGET) $(OBJS) $(DEPS))

prog: all
	$(K5PROG) firmware.bin
	

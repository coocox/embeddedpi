#
# (C) Copyright 2013, CooCox 
# All rights reserved.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundatio; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

#
TOP_DIR		= $(shell pwd)/../..

# MCU name and submodel
MCU      	= cortex-m3
SUBMDL   	= stm32f103

# toolchain (using code gun now)
KBUILD_BUILDHOST := $(shell uname -m | sed -e s/i.86/i386/ \
				  -e s/sun4u/sparc64/ \
				  -e s/arm.*/arm/ -e s/sa110/arm/ \
				  -e s/s390x/s390/ -e s/parisc64/parisc/ \
				  -e s/ppc.*/powerpc/ -e s/mips.*/mips/ \
				  -e s/sh[234].*/sh/ )
# set default to nothing for native builds
ifeq ($(KBUILD_BUILDHOST), i386)
	TCHAIN	= arm-linux-
endif
ifeq ($(KBUILD_BUILDHOST), x86_64)
	TCHAIN	= arm-linux-
endif
ifeq ($(KBUILD_BUILDHOST), arm)
	TCHAIN	=
endif

THUMB    	= -mthumb
THUMB_IW 	= -mthumb-interwork

# Target file name (without extension).
#BUILDDIR 	= $(TOP_DIR)/build
#TARGET = $(BUILDDIR)/EmbeddedPi
TARGET 		= EmbeddedPi

# Optimization level [0,1,2,3,s]
OPT ?= 0
DEBUG ?= -g

INCDIRS = $(TOP_DIR)/ \
	      $(TOP_DIR)/Utils \
          $(TOP_DIR)/ComPort \
          $(TOP_DIR)/Apps \
	      $(TOP_DIR)/Drivers/DigitalAccelerometer \
	      $(TOP_DIR)/Drivers/ArduinoMotorShield \
          $(TOP_DIR)/Drivers/UltraSonic \
          $(TOP_DIR)/Drivers/DigitalCompass \
          $(TOP_DIR)/Drivers/SensorShield \
          $(TOP_DIR)/Drivers/ServoMotor \
          $(TOP_DIR)/../../../lib/libcox \
          $(TOP_DIR)/../../../lib/libcookie \
		  $(TOP_DIR)/Drivers/RGBLed

SRCDIRS := $(TOP_DIR)/ \
	  $(TOP_DIR)/Utils \
          $(TOP_DIR)/Framework \
          $(TOP_DIR)/ComPort \
          $(TOP_DIR)/Apps \
          $(TOP_DIR)/Drivers/DigitalAccelerometer \
          $(TOP_DIR)/Drivers/ArduinoMotorShield \
          $(TOP_DIR)/Drivers/UltraSonic \
          $(TOP_DIR)/Drivers/DigitalCompass \
          $(TOP_DIR)/Drivers/SensorShield \
          $(TOP_DIR)/Drivers/ServoMotor \
          $(TOP_DIR)/../../../lib/libcox \ 
		  $(TOP_DIR)/Drivers/RGBLed

CFLAGS = $(DEBUG)
CFLAGS += -O$(OPT)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wimplicit
CFLAGS += -Wcast-align
CFLAGS += -Wpointer-arith -Wswitch
CFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
#CFLAGS += -Wa,-adhlns=$(BUILDDIR)/$(subst $(suffix $<),.lst,$<)
CFLAGS += $(patsubst %,-I%,$(INCDIRS))
CFLAGS += -Lc -Lgcc -Lm

# Aeembler Flags
ASFLAGS = -Wa,-adhlns=$(BUILDDIR)/$(<:.s=.lst),--g$(DEBUG)

LDFLAGS 	= -nostartfiles -Map=$(TARGET).map --cref --gc-sections
LDFLAGS		+= $(DEBUG)
LDFLAGS		+= -O$(OPT)
LDFLAGS 	+= -Lc -Lgcc -Lm

# Set the linker script
SCRIPT_DIR	= $(TOP_DIR)/Projects/gcc
LDSCRIPT 	= $(SCRIPT_DIR)/stm32_f103ze_gcc.ld
LDFLAGS 	+= -T$(LDSCRIPT) 

# Define programs and commands.
CC 		= $(TCHAIN)gcc
AS 		= $(TCHAIN)as
LD 		= $(TCHAIN)ld
AR 		= $(TCHAIN)ar
OBJCOPY		= $(TCHAIN)objcopy
OBJDUMP		= $(TCHAIN)objdump
SIZE 		= $(TCHAIN)size
NM 		= $(TCHAIN)nm


# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -Wall -mcpu=$(MCU) $(THUMB_IW) $(CFLAGS)
ALL_ASFLAGS = -Wall -mcpu=$(MCU) $(THUMB_IW) -x assembler-with-cpp $(ASFLAGS)

# --------------------------------------------- #
# file management
SRC 		+=$(foreach x,${SRCDIRS}, $(wildcard $(addprefix  ${x}/*,.c) ) )
SRC 		+= $(TOP_DIR)/../../../lib/startup/src/startup_coide.c

SOURCE_ASM	+=

# Define all object files.
_COBJ		= $(SRC:%.c=%.o)
_AOBJ		= $(ASRC:%.S=%.o)

# Display size of file.
HEXSIZE = $(SIZE) --target=binary $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf

#go!
all: begin gccversion build sizeafter finished end
build: elf bin hex lss sym
hex: $(TARGET).hex
bin: $(TARGET).bin
elf: $(TARGET).elf
lss: $(TARGET).lss
sym: $(TARGET).sym

begin:
	@echo "-------- begin --------"
finished:
	@echo "Errors: none"
end:
	cp $(TARGET).bin $(TOP_DIR)/Bin
	@echo "--------  end  --------"
sizeafter:
	@if [ -f $(TARGET).elf ]; then echo "Size after:"; $(ELFSIZE); fi
gccversion:
	@$(CC) --version

program:
	@echo "Flash-programming with RPI"
	cp $(TARGET).bin flash/flash.bin
	cd flash && sudo python stm32loader.py -e -w -v flash.bin

# Create final output file (.hex) from BIN output file.
$(TARGET).hex: $(TARGET).bin
	@echo "Creating load file for Flash:"
	$(OBJCOPY) -Ibinary -Oihex $< $@ 

# Create final output file (.bin) from ELF output file.
$(TARGET).bin: $(TARGET).elf
	@echo -e "\e[1;34mCreating load file for Flash: \e[0m"
	$(OBJCOPY) -Obinary $< $@

# Create extended listing file from ELF output file.
# testing: option -C
%.lss: %.elf
	@echo "Creating Extended Listing:" $@
	$(OBJDUMP) -h -S -D $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo "Creating Symbol Table:" $@
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
$(TARGET).elf: $(_COBJ) $(_AOBJ)
	$(LD) $+ $(LDFLAGS) -o $@

# Compile: create object files from C source files. ARM/Thumb
$(_COBJ): %.o : %.c
	$(CC) $(THUMB) $(ALL_CFLAGS) -c -o $@ $<

# Assemble: create object files from assembler source files. ARM/Thumb
$(_AOBJ): %.o : %.S
	$(AS) $(THUMB) $(ALL_ASFLAGS) -c -o $@ $<


#########################################################################
#########################################################################
clean:
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin
	rm -f $(TARGET).hex
	rm -f $(TARGET).sym
	rm -f $(TARGET).lss
	rm -f $(TARGET).map
	rm -f $(_COBJ) $(_AOBJ)
	@echo "clean done"

.PHONY: all begin finished end gccversion build sizeafter program clean

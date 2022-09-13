# project configurations
PRG   = demo
MCU   = atmega8a
F_CPU = 11059200UL
C_STD = c99
OPT   = s
DBG   = 3

# include files and directories
INC    =
INCDIR = inc

# source files and directories
SRC  = src/main.c
SRC += src/usart.c
SRC += src/serio.c
SRC += src/queue.c
SRC += src/twi.c
SRC += src/ds1307.c
SRCDIR = src

# object files and directories
OBJ    = 
OBJDIR = obj

# compiler toolchain
CC      = avr-gcc
AR      = avr-ar
NM      = avr-nm
SIZE    = avr-size
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

# generic compiler flags
CC_FLAGS := -pipe -gdwarf-2 -g$(DBG) -O$(OPT)
CC_FLAGS += -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CC_FLAGS += -Wall -Wextra -Werror -Wshadow -Wformat=2 -Wundef -Wstrict-prototypes
CC_FLAGS += -fno-common -ffunction-sections -fdata-sections -fno-jump-tables
CC_FLAGS += -fno-strict-aliasing -funsigned-char -funsigned-bitfields -fpack-struct
CC_FLAGS += $(addprefix -I,$(INCDIR))

# generic loader flags
LD_FLAGS := -mmcu=$(MCU)
LD_FLAGS += -Wl,--cref
LD_FLAGS += -Wl,--gc-sections
LD_FLAGS += -Wl,-Map=$(PRG).map

# programmer utility
AVRDUDE = avrdude

# programmer options
AVRDUDE_PARTNO     = m8
AVRDUDE_PROGRAMMER = ponyser
AVRDUDE_PORT       = /dev/ttyS0
AVRDUDE_FLAGS     := -p $(AVRDUDE_PARTNO) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)

# formatter utility
INDENT = indent

# formatter options
INDENT_FLAGS = --linux-style

# disable default in-built make rules
ifeq ($(filter -r,$(MAKEFLAGS)),)
  MAKEFLAGS += -r
endif
.SUFFIXES:

# filter source files by type
C_SRC := $(filter %.c, $(SRC))
A_SRC := $(filter %.S, $(SRC))

# create a list of required output object files
OBJ += $(addsuffix .o, $(basename $(SRC)))
OBJ := $(addprefix $(patsubst %/,%,$(OBJDIR))/, $(notdir $(OBJ)))

# create the output object file directory
VPATH += $(dir $(SRC))

# create a list of dependency files
DEP := $(OBJ:%.o=%.d)

# configure output messages
MSG_INFO_TXT    := ' [INFO]    :'
MSG_CC_CMD      := ' [CC]      :'
MSG_AS_CMD      := ' [AS]      :'
MSG_NM_CMD      := ' [NM]      :'
MSG_RM_CMD      := ' [RM]      :'
MSG_LD_CMD      := ' [LD]      :'
MSG_AR_CMD      := ' [AR]      :'
MSG_SIZE_CMD    := ' [SIZE]    :'
MSG_INDENT_CMD  := ' [INDENT]  :'
MSG_OBJCPY_CMD  := ' [OBJCPY]  :'
MSG_OBJDMP_CMD  := ' [OBJDMP]  :'
MSG_AVRDUDE_CMD := ' [AVRDUDE] :'

# perform a complete build of the user application
all: header elf hex bin lss sym size footer

# print compiler and project name information when building
header:
	@echo $(MSG_INFO_TXT) Begin compilation of project \"$(PRG)\"...
	@echo ""
	@$(CC) --version

# print project name information when building has completed
footer:
	@echo $(MSG_INFO_TXT) Finished building project \"$(PRG)\".

# print size information of a compiled application
size: $(PRG).elf
	@echo $(MSG_SIZE_CMD) Determining size of \"$<\"
	@echo ""
	$(SIZE) --format=avr --mcu=$(MCU) $<

# print size information on the symbols within an ELF file
symbol-sizes: $(PRG).elf
	@echo $(MSG_NM_CMD) Extracting \"$<\" symbols with decimal byte sizes
	$(NM) --size-sort --demangle --radix=d $<

# clean intermediary build files
mostlyclean:
	@echo $(MSG_RM_CMD) Removing object files of \"$(PRG)\"
	rm -Rf $(OBJ) $(OBJDIR)
	@echo $(MSG_RM_CMD) Removing dependency files of \"$(PRG)\"
	rm -f $(DEP)

# clean all build files
clean: mostlyclean
	@echo $(MSG_RM_CMD) Removing output files of \"$(PRG)\"
	rm -f $(PRG).elf $(PRG).hex $(PRG).bin $(PRG).eep $(PRG).map $(PRG).lss $(PRG).sym lib$(PRG).a

# force coding styles on source code files
indent:
	@echo $(MSG_INDENT_CMD) Indenting source files of \"$(PRG)\"
	$(INDENT) $(INDENT_FLAGS) $(SRCDIR)/*.c $(INCDIR)/*.h
	@rm -f $(SRCDIR)/*.c~ $(INCDIR)/*.h~

# helper targets, to build a specific type of output file
elf: $(PRG).elf
hex: $(PRG).hex $(PRG).eep
bin: $(PRG).bin
lss: $(PRG).lss
sym: $(PRG).sym

# default target to *create* the user application's specified source files;
# if this rule is executed by make, the input source file doesn't exist
# and an error needs to be presented to the user
$(SRC):
	$(error Source file does not exist: $@)

# compile an input C source file and generate an assembly listing for it
%.s: %.c $(MAKEFILE_LIST)
	@echo $(MSG_CC_CMD) Generating assembly from C file \"$(notdir $<)\"
	$(CC) -S $(CC_FLAGS) -x c -std=$(C_STD) $< -o $@

# compile an input C source file and generate a linkable object file for it
$(OBJDIR)/%.o: %.c $(MAKEFILE_LIST)
	@echo $(MSG_CC_CMD) Compiling C file \"$(notdir $<)\"
	$(CC) -c $(CC_FLAGS) -x c -std=$(C_STD) -MMD -MP -MF $(@:%.o=%.d) $< -o $@

# assemble an input ASM source file and generate a linkable object file for it
$(OBJDIR)/%.o: %.S $(MAKEFILE_LIST)
	@echo $(MSG_AS_CMD) Assembling \"$(notdir $<)\"
	$(CC) -c $(CC_FLAGS) -x assembler-with-cpp -MMD -MP -MF $(@:%.o=%.d) $< -o $@

# generate an ELF file from the user application
.PRECIOUS  : $(OBJ)
.SECONDARY : %.elf
%.elf: $(OBJ)
	@echo $(MSG_LD_CMD) Linking object files into \"$@\"
	$(CC) $^ -o $@ $(LD_FLAGS)

# extract out the loadable FLASH memory data from the project ELF file, and create an Intel HEX format file of it
%.hex: %.elf
	@echo $(MSG_OBJCPY_CMD) Extracting HEX file data from \"$<\"
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

# extract out the loadable FLASH memory data from the project ELF file, and create an Binary format file of it
%.bin: %.elf
	@echo $(MSG_OBJCPY_CMD) Extracting BIN file data from \"$<\"
	$(OBJCOPY) -O binary -R .eeprom -R .fuse -R .lock -R .signature $< $@

# extract out the loadable EEPROM memory data from the project ELF file, and create an Intel HEX format file of it
%.eep: %.elf
	@echo $(MSG_OBJCPY_CMD) Extracting EEP file data from \"$<\"
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 $< $@

# create an assembly listing file from an input project ELF file, containing interleaved assembly and source data
%.lss: %.elf
	@echo $(MSG_OBJDMP_CMD) Extracting LSS file data from \"$<\"
	$(OBJDUMP) -h -d -z $< > $@

# create a symbol file listing the loadable and discarded symbols from an input project ELF file
%.sym: %.elf
	@echo $(MSG_NM_CMD) Extracting SYM file data from \"$<\"
	$(NM) -n $< > $@

# include build dependency files
-include $(shell mkdir -p $(OBJDIR) 2>/dev/null) $(DEP)

# program in the target FLASH memory using AVRDUDE
program: $(PRG).hex $(MAKEFILE_LIST)
	@echo $(MSG_AVRDUDE_CMD) Programming device \"$(AVRDUDE_PARTNO)\" FLASH using \"$(AVRDUDE_PROGRAMMER)\" on port \"$(AVRDUDE_PORT)\"
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<

# program in the target EEPROM memory using AVRDUDE
program-ee: $(PRG).eep $(MAKEFILE_LIST)
	@echo $(MSG_AVRDUDE_CMD) Programming device \"$(AVRDUDE_PARTNO)\" EEPROM using \"$(AVRDUDE_PROGRAMMER)\" on port \"$(AVRDUDE_PORT)\"
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:w:$<

BUILD_TARGETS  = header footer size symbol-sizes all elf bin hex lss clean mostlyclean
BUILD_TARGETS += program program-ee

# phony build targets for this module
.PHONY: $(BUILD_TARGETS)

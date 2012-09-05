############
# Settings #
############
VERBOSE=1
# Build all project applications:
#   make
#
# Program a project  application using UISP (appname =>  app name e.g. sems1):
#   make program app=appname

# Location of build tools and atomthreads sources

KERNEL_DIR=kernel
PROJ_DIR=project
AVR_DIR=arch
CC=/usr/bin/avr-gcc
OBJCOPY=/usr/bin/avr-objcopy
SIZE=/usr/bin/avr-size
UISP=/usr/bin/uisp
SIMAVR=/tmp/run_avr

# Modify this to the device name of the UART used for UISP
UISP_DEV=/dev/ttyUSB0

# Modify this to the CPU you are using
PART=atmega328p

# Enable stack-checking. WARNING: the full automated test suite currently
# requires a little over 1KB RAM with stack-checking enabled. If you are 
# using a device with 1KB internal SRAM and no external SRAM then you
# must disable stack-checking to run all of the automated tests.
#STACK_CHECK=true

# Test programs: Log stack usage to UART (if STACK_CHECK is enabled)
#TESTS_LOG_STACK=true

# Directory for built objects
BUILD_DIR=libs

# Port/application object files
APP_OBJECTS = atomport.o wiring.o wiring_digital.o uart.o tests-main.o 
APP_ASM_OBJECTS = atomport-asm.o

# Kernel object files
KERNEL_OBJECTS = atomkernel.o atomsem.o atommutex.o atomtimer.o atomqueue.o

# Collection of built objects (excluding proj applications)
ALL_OBJECTS = $(APP_OBJECTS) $(APP_ASM_OBJECTS) $(KERNEL_OBJECTS)
BUILT_OBJECTS = $(patsubst %,$(BUILD_DIR)/%,$(ALL_OBJECTS))

# Proj object files (dealt with separately as only one per application build)
PROJ_OBJECTS = $(notdir $(patsubst %.c,%.o,$(wildcard $(PROJ_DIR)/*.c)))

# Target application filenames (.elf and .hex) for each proj object
PROJ_ELFS = $(patsubst %.o,%.elf,$(PROJ_OBJECTS))
PROJ_HEXS = $(patsubst %.o,%.hex,$(PROJ_OBJECTS))

# Search build/output directory for dependencies
vpath %.o ./$(BUILD_DIR)
vpath %.elf ./$(BUILD_DIR)
vpath %.hex ./$(BUILD_DIR)

# GCC flags
CFLAGS=-g -mmcu=$(PART) -Wall -Werror -DARDUINO=100 -DF_CPU=16000000L -ffunction-sections -fdata-sections -fno-exceptions -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

# Enable stack-checking options (disable if not required)
ifeq ($(STACK_CHECK),true)
CFLAGS += -DATOM_STACK_CHECKING
endif
ifeq ($(TESTS_LOG_STACK),true)
CFLAGS += -DTESTS_LOG_STACK_USAGE
endif


#################
# Build targets #
#################

# All Projects
all: $(BUILD_DIR) $(PROJ_HEXS) Makefile

# Make build/output directory
$(BUILD_DIR):
	echo $(BUILT_OBJECTS)
	mkdir $(BUILD_DIR)

# Test HEX files (one application build for each test)
$(PROJ_HEXS): %.hex: %.elf
	@echo Building $@
	$(OBJCOPY) -j .text -j .data -O ihex ./$< ./$@

# Test ELF files (one application build for each test)
$(PROJ_ELFS): %.elf: %.o $(KERNEL_OBJECTS) $(APP_OBJECTS) $(APP_ASM_OBJECTS)
	$(CC) $(CFLAGS) $(BUILD_DIR)/$(notdir $<) $(BUILT_OBJECTS) --output ./$@ -Wl,-Map,$(BUILD_DIR)/$(basename $@).map

# Kernel objects builder
$(KERNEL_OBJECTS): %.o: $(KERNEL_DIR)/%.c
	$(CC) -c $(CFLAGS) -I$(AVR_DIR)/incl -I$(KERNEL_DIR)/incl $< -o $(BUILD_DIR)/$(notdir $@)

# Test objects builder
$(PROJ_OBJECTS): %.o: $(PROJ_DIR)/%.c
	$(CC) -c $(CFLAGS) -I$(AVR_DIR)/incl -I$(KERNEL_DIR)/incl $< -o $(BUILD_DIR)/$(notdir $@)

# Application C objects builder
$(APP_OBJECTS): %.o: $(AVR_DIR)/%.c
	$(CC) -c $(CFLAGS) -I$(AVR_DIR)/incl -I$(KERNEL_DIR)/incl -I$(PROJ_DIR)/incl $< -o $(BUILD_DIR)/$(notdir $@)

# Application asm objects builder
$(APP_ASM_OBJECTS): %.o:  $(AVR_DIR)/%.s
	$(CC) -c $(CFLAGS) -x assembler-with-cpp -I$(AVR_DIR)/incl -I$(KERNEL_DIR)/incl $< -o $(BUILD_DIR)/$(notdir $@)

# .lst file builder
%.lst: %.c
	$(CC) $(CFLAGS) -I$(AVR_DIR)/incl -I$(KERNEL_DIR)/incl -I$(PROJ_DIR)/incl -Wa,-al $< > $@

# Clean
clean:
	rm -f *.o *.elf *.map *.hex *.bin *.lst
	rm -rf doxygen-kernel
	rm -rf doxygen-avr
	rm -rf build

# Send to STK500
program : $(BUILD_DIR)/$(app).hex
	$(SIZE) -C --mcu=$(PART) $(BUILD_DIR)/$(app).elf
	$(UISP) -dprog=stk500 -dserial=$(UISP_DEV) -dpart=$(PART) --erase --upload --verify if=$(BUILD_DIR)/$(app).hex

# Generate Doxygen documentation
doxygen:
	doxygen $(KERNEL_DIR)/Doxyfile
	doxygen ./Doxyfile

upload:
	avrdude avrdude -V -F -c arduino -p m328p -P /dev/ttyACM0 -U flash:w:thread.hex
# Run tests within simavr simulator
phony_sim_elfs = $(addsuffix .sim, $(PROJ_ELFS))
simtests: $(phony_sim_elfs)
.PHONY: simtests $(phony_sim_elfs)
$(phony_sim_elfs):
	./run_test.exp $(SIMAVR) $(PART) $(BUILD_DIR)/$(basename $@)

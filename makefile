#target mcu
MCU = attiny3226
#optimize for size
OPT = -Os
#CPU freq
F_CPU = 20000000UL
#compiler flags
CFLAGS = -mmcu=$(MCU) $(OPT) -I./headers -I /home/bill/avrgcc/ATPACK/include
CFLAGS += -B /home/bill/avrgcc/ATPACK/gcc/dev/$(MCU) -DF_CPU=$(F_CPU)
#automagically find source files
SRCS = $(wildcard *.c)

#object files
OBJS = $(SRCS:.c=.o)

#output file
TARGET = output.elf

#toolchain
CC = avr-gcc
OBJCOPY = avr-objcopy

#UART port
UART_PORT = /dev/ttyUSB0



#default target: compile and link
all: $(TARGET)

#link obj files into ELF
$(TARGET): $(OBJS)
	$(CC) -mmcu=$(MCU) -B /home/bill/avrgcc/ATPACK/gcc/dev/$(MCU) -DF_CPU=$(F_CPU) -o $(TARGET) $(OBJS)
#compiling C files into obect files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#convert ELF to hex
hex: $(TARGET)
	$(OBJCOPY) -O ihex $(TARGET) output.hex

#flash hex using pymcuprog
flash: hex
	pymcuprog write -t uart -d $(MCU) -u $(UART_PORT) -f output.hex --erase --verify
	rm -f $(OBJS) $(TARGET) output.hex
#clean build directory
clean:
	rm -f $(OBJS) $(TARGET) output.hex
	

ASM := C:\Keil\ARM\ARMCC\bin\armasm.exe
CC := C:\Keil\ARM\ARMCC\bin\armcc.exe
LINK := C:\Keil\ARM\ARMCC\bin\armlink.exe
OBJCOPY := C:\Keil\ARM\ARMCC\bin\fromelf.exe
LIB := C:\Keil\ARM\ARMCC\bin\armar.exe
RM := del /q

A_SRC := $(sort $(wildcard *.s))
C_SRC := $(sort $(wildcard *.c))
A_OBJ := $(patsubst %.s,%.o,$(A_SRC))
C_OBJ := $(patsubst %.c,%.o,$(C_SRC))

TARGET := kd1

.PHONY : all clean

all : $(TARGET).bin $(TARGET).hex

$(TARGET).bin : $(TARGET).axf
	$(OBJCOPY) --bin $< --output $@
	copy /b update_txt.hex+$@ p0001.bin /Y > nul

$(TARGET).hex : $(TARGET).axf
	$(OBJCOPY) --i32 $< --output $@

$(TARGET).axf : $(A_OBJ) $(C_OBJ) LPC111x.sct
	-$(LINK) --cpu Cortex-M0 --feedback "fb.txt" --strict --scatter "LPC111x.sct" --list "$(TARGET).map" --output "$@" $(A_OBJ) $(C_OBJ)
	$(OBJCOPY) -z $@
	$(OBJCOPY) -z $(C_OBJ)

$(A_OBJ) : %.o : %.s Makefile
	-$(ASM) --cpu Cortex-M0 --thumb --apcs=interwork --depend "$(basename $<).d" -o "$@" $<

$(C_OBJ) : %.o : %.c Makefile
	-$(CC) -c --cpu Cortex-M0 --thumb --apcs=interwork --depend "$(basename $<).d" -O3 --multibyte_chars -I. --feedback "fb.txt" -o "$@" -c $<

clean:
	-$(RM) *.d > nul
	-$(RM) *.o > nul
	-$(RM) fb.txt > nul
	-$(RM) $(TARGET).map > nul
	-$(RM) $(TARGET).axf > nul
	-$(RM) $(TARGET).hex > nul
	-$(RM) $(TARGET).bin > nul

-include $(wildcard *.d)

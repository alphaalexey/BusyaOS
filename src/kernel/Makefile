.PHONY = all clean

all: main.o
	$(LD) $(LDFLAGS) -T script.ld -o kernel.bin main.o

main.o: main.asm
	$(ASM) main.asm $@
	
clean:
	$(RM) *.o *.bin
.PHONY = all clean

all: boot.bin

boot.bin: boot.asm
	$(ASM) boot.asm $@

clean:
	$(RM) *.bin

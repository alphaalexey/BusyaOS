#!/bin/sh
make

cp src/boot/boot.bin bin/
cp src/kernel/kernel.bin bin/
cp src/makefs/makefs bin/

dd if=bin/boot.bin of=bin/boot_sector.bin bs=512 count=1 status=none
dd if=bin/boot.bin of=disk/boot.bin bs=1 skip=512 status=none
cp bin/kernel.bin disk/system/kernel/kernel.bin

#rm -f disk.img
#dd if=/dev/null of=disk.img bs=4096 count=1
#dd if=bin/boot_sector.bin of=disk.img conv=notrunk

bin/makefs src=./disk out=disk.img size=4096 bs=512 boot=bin/boot_sector.bin

read -p "Press Enter to continue..." dummy
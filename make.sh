#!/bin/sh
make

cp src/boot/boot.bin bin/
cp src/kernel/kernel.bin bin/
cp src/makefs/makefs bin/

cp bin/kernel.bin disk/system/kernel/kernel.bin

bin/makefs src=./disk out=disk.img size=4096 bs=512 boot=bin/boot.bin
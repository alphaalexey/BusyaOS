format Binary as "bin"
org 0x7C00
	jmp boot

align 4
label disk_id byte at $$
info_msg db "BusyaOS v0.1 booting", 13, 10, 0
reboot_msg db "Press any key to reboot...", 13, 10, 0

dog_msg db "         ___", 13, 10, "   _.-*^|  \ \_______", 13, 10, "   ^-----\./|        |\", 13, 10, "           /  ____   /  \", 13, 10, "          /.//    \\.\    \", 13, 10, 0

write_str:
	push si
	mov ah, 0x0E
@@:
	lodsb
	test al, al
	jz @f
	int 10h
	jmp @b
@@:
	pop si
	ret

error:
	pop si
	call write_str

reboot:
	mov si, reboot_msg
	call write_str
	xor ah, ah
	int 16h
	jmp 0xFFFF:0

boot:
	jmp 0:@f
@@:
	mov ax, cs
	mov ds, ax
	mov es, ax
	
	mov ss, ax
	mov sp, $$
	
	sti

	mov ah, 00h
	mov al, 12h
	int 10h

	mov ax, 03h
	int 10h
	
	mov si, info_msg
	call write_str
	
	mov si, dog_msg
	call write_str
	
	mov [disk_id], dl
		
	jmp $
	
times 510 - ($ - $$) db 0
db  0x55, 0xAA
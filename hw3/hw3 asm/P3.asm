        ; 8080 assembler code
        .hexfile P3.hex
        .binfile P3.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

	; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6



FileCreate 	equ 12
FileClose	equ 13
FileOpen 	equ 14
FileRead	equ 15
FileWrite	equ 16
FileSeek	equ 17
DirRead		equ 18



	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE


DATA: DS 1024

begin:
	

	LXI SP,stack 	; always initialize the stack pointer


	LXI B, DATA
	
	MVI A, DirRead			; store the OS call code to A
	call GTU_OS				; call the OS


	LXI B, DATA
	MVI A, PRINT_STR
	call GTU_OS


	hlt				; end program
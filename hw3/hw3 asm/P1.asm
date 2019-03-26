        ; 8080 assembler code
        .hexfile P1.hex
        .binfile P1.com
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


	string:	dw '0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 ',00AH,00H		; null terminated string

begin:
	

	LXI SP,stack 	; always initialize the stack pointer


	MVI B, 1
	MVI A, READ_STR			; get file name
	call GTU_OS				; call the OS

	MOV D, B 				; temp filename


	MVI A, FileCreate		; create file
	call GTU_OS				; call the OS


	MVI B,1
	MOV B, D 				; get filename from temp

	MVI A, FileOpen			; open file
	call GTU_OS				; call the OS

	MOV H, B 				; opened file ID
	
	MOV E, H 				
	MVI D, 143
	

	MVI B, 1
	LXI B, string
	MVI A, FileWrite		; store the OS call code to A
	call GTU_OS				; call the OS



	MOV B, H 				; close file

	MVI A, FileClose		; store the OS call code to A
	call GTU_OS				; call the OS



	hlt				; end program
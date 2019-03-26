        ; 8080 assembler code
        .hexfile P4.hex
        .binfile P4.com
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


	string:	dw '50 51 52 53 54 55 56 57 58 59 60 ',00AH,00H		; null terminated string

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
	MVI D, 142
	

	MVI B, 1
	LXI B, string
	MVI A, FileWrite		; store the OS call code to A
	call GTU_OS				; call the OS



	MOV B, H 				; close file

	MVI A, FileClose		; store the OS call code to A
	call GTU_OS				; call the OS



	hlt				; end program
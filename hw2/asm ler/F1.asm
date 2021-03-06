        ; 8080 assembler code
        .hexfile F1.hex
        .binfile F1.com
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


NUM 		equ 50 	; number bound

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


begin:
	LXI SP,stack 	; always initialize the stack pointer

	MVI B,0 		; initialize b

LOOP:	
	MVI A, PRINT_B 	; initalize to print
	call GTU_OS 	; system call
	INR B 			; increment b
	MVI A, NUM 		; set a register to NUM
	SUB B  		; A=A-B	;
	JP LOOP 		; jmp if A is positive
	hlt				; end program
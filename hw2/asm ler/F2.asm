        ; 8080 assembler code
        .hexfile F2.hex
        .binfile F2.com
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



size: DB 22

begin:
	LXI SP,stack 	; always initialize the stack pointer
	

	MVI H, 0  			; i  for outer index
	MVI D, 0

FOR:	
	LDA size       		; A= 22
	SUB H  				; size(50) - H
	JZ EXIT 			; if size-H == 0 ,  go exit

	MOV L, H 			; j  
	INR H 				; ++i

	
NESTED_FOR:	
	LDA size 			; A =size
	SUB L 				; A =A-L 
	JZ FOR 				; if size == j   , back to the outer loop
	INR L 				; ++j
	INR D
	JMP NESTED_FOR


EXIT:
	MOV B, D	
	MVI A, PRINT_B 	; initalize to print
	call GTU_OS 	; system call
	hlt				; end program	




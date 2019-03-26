; 8080 assembler code
        .hexfile F1_F3_parallel.hex
        .binfile F1_F3_parallel.com
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
GET_RND		equ 7

TExit		equ 8
TJoin		equ 9
TYield		equ 10
TCreate		equ 11

		; Position for stack pointers
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

	;This program prints a null terminated string to the screen


NUM 		equ 50 	; number bound
NUM2 		equ 100 	; number bound


begin:
	LXI SP,stack 	; always initialize the stack pointer
					
	
	LXI B, F1			; put the address of F1 thread in registers B and C
	MVI A, TCreate		; store the OS call code to A
	call GTU_OS			; call the OS
	
	MOV D, B
	
	LXI B, F3			; put the address of F3 thread in registers B and C
	MVI A, TCreate		; store the OS call code to A
	call GTU_OS			; call the OS


	MVI A, TJoin		; join F3
	call GTU_OS			; call the OS

	MOV B, D

	MVI A, TJoin		; join F1
	call GTU_OS			; call the OS
	

	hlt					; end program

F1:
	MVI B,0 		; initialize b

LOOP:	
	MVI A, PRINT_B 	; initalize to print
	call GTU_OS 	; system call
	INR B 			; increment b
	MVI A, NUM 		; set a register to NUM
	SUB B  			; A=A-B	;
	JP LOOP 		; jmp if A is positive
	
	MVI A, TExit 
	call GTU_OS	 	; call the OS



F3:
	MVI B,50 		; initialize b

LOOP2:	
	MVI A, PRINT_B 	; initalize to print
	call GTU_OS 	; system call
	INR B 			; increment b
	MVI A, NUM2		; set a register to NUM
	SUB B  			; A=A-B	;
	JP LOOP2 		; jmp if A is positive
	
	MVI A, TExit 
	call GTU_OS	 		; call the OS
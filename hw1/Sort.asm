        ; 8080 assembler code
        .hexfile Sort.hex
        .binfile Sort.com
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
GET_RND 	equ 7



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

	
LIST: DS 50			 	; my array
size: DB 50 			; array size


	
begin:
	LXI SP,stack 		; always initialize the stack pointer

		
	MVI H, 0 			; for index , H=0   (i)
	LXI D, LIST  		; load LIST[0] adress to DE



INITIALIZE:

	MVI A, GET_RND
	call GTU_OS			; get random number to B

	MOV A,B  			; A = B
	STAX D  			; LIST[i] = A 
	
	INR D 				; cursor 1 index forward
	INR H 				; ++i

	LDA size       		; A= 50
	SUB H  				; size(50) - H
	JNZ INITIALIZE 		; if size-H != 0 , continue the initializing

						; after initializing array 
	

						;for (i = 0; i < n-1; i++)      
						;	for (j = 0; j < n-1-i; j++) 
						;		if (arr[j] > arr[j+1])
						;	    	swap(&arr[j], &arr[j+1]);

						; I implement bubble sort 


	MVI H, 0  			; i  for outer index


FOR:	
	LDA size       		; A= 50
	SUB H  				; size(50) - H
	JZ EXIT 			; if size-H == 0 ,  go exit

	MOV L, H 			; j  for nested index  (j=i) ( I always start the loop with LIST first element so, actually this causes j<(n-1-i) )
	INR H 				; ++i
	LXI B,LIST 			; load LIST[0] adress to BC

NESTED_FOR:	
	LDA size 			; A =size
	SUB L 				; A =A-L 
	JZ FOR 				; if size == j   , back to the outer loop
	INR L 				; ++j

IF:
	LDAX B 				; A	= list[j]
	MOV D,A 	 		; D = list[j]

	INR B  				; cursor 1 index forward

	LDAX B     			; A = list[j+1]

	CMP D 				; list[j+1]-list[j]	 (A-D) 
	JC SWAP    	 		; if  list[j+1] < list[j]
	JMP NESTED_FOR



SWAP:
	LDAX B 				; A = list[j+1]
	MOV E,A 			; E = A 		(E = temp = list[j+1])
	MOV A,D 			; A = D, D =list[j]
	STAX B  			; list[j+1] = D
	
	DCR B 				; cursor 1 index back
	
	MOV A,E 			; A = E
	STAX B 				; list[j] = E
	INR B 				; cursor 1 index forward again
	
						; inner loop end
	JMP NESTED_FOR		; go back inner loop

EXIT:
	LXI D, LIST
	MVI H,0 			; i=0   for  PRINT_LOOP

PRINT_LOOP:
	INR D  	 			; cursor 1 index forward
	LDAX D ; A <- (DE)
	MOV B,A
	MVI A, PRINT_B
	call GTU_OS

	;INR D  	 			; cursor 1 index forward

	INR H 				; ++i
	LDA size       		; A= 50
	SUB H  				; size(50) - H
	JNZ PRINT_LOOP 		; if size-H != 0 , continue the print
	

	hlt					; end program
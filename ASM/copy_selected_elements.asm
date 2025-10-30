	main: 
		ldr r0,=src
		ldr r1,=dst
		mov r2, #2
		mov r3, #6
		bl copyelements
		loop: b loop
		.org 0x30
		dst: .space 16
		src: .word 1,2,3,4,-1,-2,-3,-4

copyelements:	PUSH {R4,R5,LR}	

src_start:	LSL	R4,R2,#2
		ADD	R0,R0,R4

dst_start:	LSL	R5,R2,#1
		ADD	R1,R1,R5

		SUB R2,R3,R2

		BL	copyvec
		POP {R4,R5,PC}


copyvec:	PUSH	{R4,R5}
		MOV	R3,#0
copyvec1:	CMP	R3,R2
		BGE	copyvec2
		LSL	R5,R3,#2
		LDR	R4,[R0,R5]
		LSL	R5,R3,#1
		STRH	R4,[R1,R5]
		ADD	R3,R3,#1
		B	copyvec1
copyvec2:	POP	{R4,R5}
		BX	LR



		



		
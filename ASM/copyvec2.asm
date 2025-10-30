

start:		LDR	R0,=shortvec
		LDR	R1,=intvec
		MOV	R2,#8
		BL	copyvec
		B	start




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


		.ORG	0x30
intvec:		.SPACE	16
shortvec:	.WORD	1,2,3,4,-1,-2,-3,-4
		



		
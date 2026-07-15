
.global set_bit
.type set_bit, %function


set_bit:
	push {r4,lr}
	mov r1,#0x08
	orr r0, r0, r1

//	pop {r4,lr}

	bx lr

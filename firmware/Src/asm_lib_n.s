.syntax unified
.thumb
.global add_numbers
.type add_numbers, %function

add_numbers:
	add r0, r0, r1  // r0 = r0+r1
	bx lr			// return

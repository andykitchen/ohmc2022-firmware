.comm debug_status,4,4

.global _start
_start:
	addi zero,zero,-1
	.rept 15
	nop
	.endr
	lui s0,%hi(debug_status)
	lw t1,%lo(debug_status)(s0)
	addi t1,t1,1
	sw t1,%lo(debug_status)(s0)
	j _start

.global counter_loop
.comm global_counter,4,4

counter_loop:
	la a0, global_counter
1:
	lw t1,0(a0)
	addi t1,t1,1
	sw t1,0(a0)
	.rept 32
	nop
	.endr
	j 1b

	.globl syscall

syscall:
	movq %rdi, %rax

	movq %rsi, %rdi
	movq %rdx, %rsi
	movq %rcx, %rdx
	movq %r8, %r10
	movq %r9, %r8
	# TODO arg6
	syscall
	ret

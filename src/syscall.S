	.file "syscall.s"
	.globl syscall
	.type syscall,@function
	.func syscall,syscall

syscall:
	pushq %r15
	pushq %r14
	pushq %r13
	pushq %r12
	pushq %rbx
	pushq %rbp

	.equ	ARG6, ((1*8)+(6*6))

	movq %rdi, %rax			# syscall id
	movq %rsi, %rdi			# arg1
	movq %rdx, %rsi			# arg2
	movq %rcx, %rdx			# arg3
	movq %r8, %r10			# arg4
	movq %r9, %r8			# arg5
	movq ARG6(%rsp), %r9	# arg6 - (%rsp) is ret

	syscall
	nop

	popq %rbp
	popq %rbx
	popq %r12
	popq %r13
	popq %r14
	popq %r15

	cmp $-4096, %rax
	ja .has_error
	retq

.has_error:
	pushq %rax
	call __errno_location
	popq %rcx
	cmp $0, %rax
	je .no_errno
	not %ecx
	mov %ecx, (%rax)
	movq $-1, %rax
.no_errno:
	retq
	.endfunc

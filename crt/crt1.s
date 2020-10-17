	.file	"start.s"
	.text
	.globl	_start
	.type	_start,@function
	.align  8

# function arg order
# %rdi, %rsi, %rdx, %rcx


# stack on entry (ABI PDF)
# [1*8]:			null vector
# [2*8 each]:		aux vector
#                   0 [end of envp]
#					envp
# 8+8*argc+%rsp:	0 [end of argv]
# 8+%rsp:			argv
# %rsp:				argc

_start:
	xor %rbp, %rbp				# contains garbage

	pop %rdi					# pop off argc
	lea (%rsp), %rsi			# load addr (on stack) of argv
	lea 8(%rsp,%rdi,8), %rdx	# load addr (on stack) of envp

	andq $-16,%rsp				# align the stack
	
	call __libc_start_main

1:	jmp 1b

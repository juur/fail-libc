	.file	"crt1.S"
	.text
	.globl	_start
	.type	_start,@function
	.align  8

# function arg order (to pass to __libc_main)
# %rdi, %rsi, %rdx, %rcx
# argc, argv, envp, aux

# registers on entry
# %rbp - undefined but set to 0 for deepest stack
# %rsp - the stack, guaranteed to be 16-byte aligned (TODO failos)
# %rdx - a function, a function to register with atexit(BA_OS)

# stack on entry (ABI PDF)
# [1*8]:			null vector
# [2*8 each]:		aux vector
#                   0 [end of envp]
#					envp
# 8+8*argc+%rsp:	0 [end of argv]
# 8+%rsp:			argv
# %rsp:				argc

_start:
	xor  %rbp, %rbp				# contains garbage

	pop  %rdi					# pop off argc
	lea  (%rsp), %rsi			# load addr (on stack) of argv
	lea  8(%rsp,%rdi,8), %rdx	# load addr (on stack) of envp

	movq %rdi, __argc
	movq %rsi, __argv
	movq %rdx, __envp

	movq %rdi, %rax				# get argc again

.loop:
	movq 8(%rsp,%rax,8), %rbx
	cmpq $0, %rbx
	je   .done
	inc  %rax
	jmp  .loop

.done:
	inc  %rax					# skip NULL
	lea  8(%rsp,%rax,8), %rcx   # get version aux vector
	movq %rcx, __aux
	

	andq $-16, %rsp				# align the stack
	
	call __libc_start_main

1:	jmp 1b

	.data

__argc:	.long 0
__argv: .long 0
__envp: .long 0
__aux:  .long 0

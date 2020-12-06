	.file "clone.s"
	.globl _clone
	.globl start_thread
	.type syscall,@function

	.equ __NR_clone, 56

	.func _clone,_clone

# Register state on entry
#
# %rdi    - flags
# %rsi    - stack (high)
# %rdx    - &parent_id
# %rcx    - &child_tid
# %r8     - newtls
# %r9     - fn
# 8(%rsp) - arg

_clone:
	and $-16, %rsi		# align child stack

	sub $8, %rsi
	mov %r9, (%rsi)		# put fn   in stack[top+0]

	mov 8(%rsp), %r9	# extract arg

	sub $8, %rsi
	mov %r9, (%rsi)		# put args in stack[top+1]

	mov $__NR_clone, %rax
	syscall
	
	cmp $0, %rax
	jne .L1

	xor %rbp,%rbp

	pop %rsi			# pop off args (arg2)
	pop %rdi			# pop off fn   (arg1)

#ifdef DYN
	call start_thread@PLT
#else
	call start_thread
#endif

.L1:
	ret
	.endfunc

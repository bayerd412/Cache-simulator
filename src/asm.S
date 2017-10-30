#include "asmdef.h"
DECLARE_GLOBAL(asm_function)


################################################################################
# name:          asm_function
# action:        
# in:            
#
# out:
# modifies:
# notes:
################################################################################
FUNC(asm_function):

.data 			#Values
number:
	.long 0x00100000			#Maximum number, which will be checked.

pointer:
	.long 0x00000000			#Pointer for primes.


.text
.globl main
main:

	movl number, %eax			#Put number value to register %eax.
	inc %eax					#Increase %eax for 1.
	movl %eax, -4(%ebp)			#Put %eax to the operand address
	push %eax					# %ebp - 4. And push %eax to the stack.
	call malloc					#Allocate memory with malloc function.
	add $4, %esp

	cmpl $0, %eax				#If %eax == 0, then finish program.
	je exit						#Memory problem.
	movl %eax, -8(%ebp)
	movl $0, (%eax)

	cld							#Clear direction flag.
	movl %eax, %edi
	inc %edi
	movl -4(%ebp), %edx			#Save opperand address %ebp -4 to %edx.
	addl %eax, %edx
	movl $1, %al
	for_loop1:					#For (k=0;k<=n;k++)
		stosb					#Save %al in the receiver
		cmpl %edx, %edi			
		jb for_loop1			#Repeat for_loop1.

	movl -8(%ebp), %eax
	

	movl %eax, pointer 			#Add values to the %eax and %ebx
	movl pointer, %eax 			#registers to work with them.
	movl number, %ebx
	movl %eax, -4(%ebp)			

	addl %ebx, %eax				#Add %ebx and %eax into $eax.
	inc %eax
	movl %eax, -8(%ebp)
	cld							#Clear direction flag.
								#Change dirrection to the
								#increasing address.

	movl $2, %ecx				# k = 2
	movl $2, %edx				# l = 2
	for_loop2:					#Eratosthens sieve
		movl %ecx, %eax			#k = k * l
		push %ecx
		mull %edx
		pop %ecx
		cmpl %ebx, %eax			#(result <= max_number)-?
		ja for_loop3			#No, take out a number. 
			movl -4(%ebp), %edi
			addl %eax, %edi
			movb $0, (%edi)
			inc %edx			#Increase %edx by 1.
			for_loop2			#Repeat Eratosthenes sieve.

			for_loop3:			#Increase l.
				movl -4(%ebp), %esi
				addl %ecx, %esi
				inc %esi
				movl %ecx, %edx
				inc %edx
				for_loop4:		#Check a number.
					movl %edx, %eax
					mull %eax
					cmpl %ebx, %eax		#If l >= max_number, so go out
					ja cont				#from Eratosthenes algoritm.

					lodsb				#Load byte from a receiver into
					inc %edx			# the %al.
					cmpl $0, %al
					je for_loop4		#Check a number.

					movl %edx, %ecx		#Found a new 'l'.
					decl %edx
					movl $2, %edx
					for_loop2			#Repeat Eratosthenes algorithm.
			cont:						#Continue flag after algorithm.

	movl number, %ebx					#Clean and free a memory.
	movl pointer, %eax
	push %eax
	call free
	addl $4, %esp

	exit:
		ret
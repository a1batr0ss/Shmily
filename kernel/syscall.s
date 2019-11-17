[bits 32]

section .text
extern intr_exit
extern syscall_handler
global syscall_pre_handler
syscall_pre_handler:
	push 0
	push ds
	push es
	push fs
	push gs
	pushad
	push 0x99

	push ecx
	push ebx
	push eax
	call syscall_handler
	
	add esp, 12
	jmp intr_exit


[bits 32]

extern main
extern call_constructors

section .start

global _start
_start:
	call call_constructors

	push ebx
	push ecx
	call main

	jmp $


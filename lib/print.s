SELECTOR_VIDEO equ (0x0003<<3) + 0b + 00b

section .data
number_buffer dq 0

[bits 32]
section .text
global putstring
putstring:
    push ebx
    push ecx
    xor ecx, ecx
    mov ebx, [esp + 12]
.put_char:
    mov cl, [ebx]
    cmp cl, 0
    jz .str_end
    push ecx
    call putchar
    add esp, 4  ; recycle the arguments' space
    inc ebx
    jmp .put_char
.str_end:
    pop ecx
    pop ebx
    ret

global putchar
putchar:
    pushad

    mov ax, SELECTOR_VIDEO
    mov gs, ax

.get_cursor:
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    in al, dx
    mov ah, al

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    in al, dx

    mov bx, ax
    ; argument: the character will be printed.
    mov ecx, [esp + 36]

    cmp cl, 0xd
    jz .cr
    cmp cl, 0xa
    jz .lf
    cmp cl, 0x8
    jz .backspace
    jmp .visible_char

.backspace:
    dec bx
    shl bx, 1
    mov byte [gs:bx], 0x20
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    jmp .move_cursor
.visible_char:
    shl bx, 1
    mov [gs:bx], cl
    inc bx
    mov byte [gs:bx], 0x07
    shr bx, 1
    inc bx
    cmp bx, 2000
    jl .move_cursor
.cr:
.lf:
    xor dx, dx
    mov ax, bx
    mov si, 80
    div si
    sub bx, dx

    add bx, 80
    cmp bx, 2000
    jl .move_cursor
.roll_screen:
    cld
    mov ecx, 960
    mov esi, 0xb80a0
    mov edi, 0xb8000
    rep movsd

    mov ebx, 3840
    mov ecx, 80
.cls:
    mov word [gs:ebx], 0x0720
    add ebx, 2
    loop .cls
    mov bx, 1920
.move_cursor:
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    mov al, bh
    out dx, al

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    mov al, bl
    out dx, al

    popad
    ret

global set_cursor
set_cursor:
    pushad
    mov bx, [esp + 36]
    mov dx, 0x03d4
    mov al, 0x0e
    out dx, al
    mov dx, 0x03d5
    mov al, bh
    out dx, al

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al
    mov dx, 0x03d5
    mov al, bl
    out dx, al
    popad
    ret

global puthex
puthex:
    pushad
    mov ebp, esp
    mov eax, [ebp + 36]
    mov edx, eax
    mov edi, 7
    mov ecx, 8
    mov ebx, number_buffer
.16based_4bits:
    and edx, 0x0000000f
    cmp edx, 9
    jg .a2f
    add edx, '0'
    jmp .store
.a2f:
    sub edx, 10
    add edx, 'a'
.store:
    mov [ebx+edi], dl
    dec edi
    shr eax, 4
    mov edx, eax
    loop .16based_4bits
.ready_to_print:
    inc edi
.skip_prefix_0:
    cmp edi, 8
    je .full0
.skip:
    mov cl, [number_buffer+edi]
    inc edi
    cmp cl, '0'
    je .skip_prefix_0
    dec edi
    jmp .put_each_number
.full0:
    mov cl, '0'
.put_each_number:
    push ecx
    call putchar
    add esp, 4
    inc edi
    mov cl, [number_buffer+edi]
    cmp edi, 8
    jl .put_each_number
    popad
    ret

global cls
cls:
	pushad
	; push ebx
	; push ecx
    mov ebx, 0
    mov ecx, 80*25
.rm:
    mov word [gs:ebx], 0x0720
    add ebx, 2
    loop .rm
	; pop ecx
	; pop ebx
	popad
	ret


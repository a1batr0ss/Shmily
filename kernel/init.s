section init vstart=0x500

jmp init_start

INIT_CC_BASE_ADDR equ 0x60000
INIT_START_SECTOR equ 0x5
INIT_CC_ENTRY equ 0x10000
WILL_USE_SECTOR equ 300
PT_NULL equ 0

GDT_BASE:
.NULL_DESC:
    dd 0x00000000
    dd 0x00000000
.CODE_DESC:
    dd 0x0000ffff
    dd 0x00cf9a00
.DATA_DESC:
    dd 0x0000ffff
    dd 0x00cf9200
.VIDEO_DESC:
    dd 0x80000007
    dd 0x00c0920b 
.USER_CODE_DESC:
	dd 0x0000ffff
	dd 0x00cffa00
.USER_DATA_DESC:
	dd 0x0000ffff
	dd 0x00cff200

SELECTOR_CODE equ (0x0001<<3) + 0b + 00b
SELECTOR_DATA equ (0x0002<<3) + 0b + 00b
GDT_SIZE equ $ - GDT_BASE
GDT_LIMIT equ GDT_SIZE - 1
gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

; global mem_capacity
mem_capacity dd 0

init_start:
    ; xchg bx, bx
    call get_mem_capacity
    call enable_A20
    lgdt [gdt_ptr]
    call enable_PE
    jmp dword SELECTOR_CODE:prot_start 

; don't put the function below [bits 32], it will compile 32 bits(should be 16 bits)
get_mem_capacity:
    mov ax, 0xe801
    int 0x15
    mov cx, 0x400
    mul cx
    shl edx, 16  ; This is the carry bit of add operation.
    and eax, 0xffff
    or edx, eax
    add edx, 0x100000
    mov esi, edx
    
    xor eax, eax
    mov ax, bx
    mov ecx, 0x10000
    mul ecx
    add esi, eax
    mov [mem_capacity], esi
    ret


[bits 32]
prot_start:
    call reload_ds    
    ; read main.cc
    mov eax, INIT_START_SECTOR
    mov ebx, INIT_CC_BASE_ADDR
    mov ecx, 255
    call read_disk

.more_than_255:
	mov ecx, WILL_USE_SECTOR
	sub ecx, 255
	jc .load_modules_head
	
	mov eax, INIT_START_SECTOR + 255
	mov ebx, 130560  ; 255*512
	add ebx, INIT_CC_BASE_ADDR

	call read_disk

.load_modules_head:
	mov ecx, 0
.load_modules:
	mov edi, [INIT_CC_BASE_ADDR + ecx*4]
	cmp edi, 0
	jz .load_modules_end
	add edi, INIT_CC_BASE_ADDR
	inc ecx
	call load_elf32_init
	jmp .load_modules

.load_modules_end:
    ; set stack pointer
    mov esp, 0x81000

    jmp INIT_CC_ENTRY

    ; can't get here, since it's the jmp instruction above, not call, 
    ; not push the return address
    jmp $ 

; global enable_A20
enable_A20:
    in al, 0x92
    or al, 00000010B
    out 0x92, al
    ret

; global enable_PE
enable_PE:
    mov eax, cr0
    or eax, 0x00000001
    mov cr0, eax
    ret

reload_ds:
    mov ax, SELECTOR_DATA
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; haven't set gs, so can't print string

    ret

read_disk:
    mov esi, eax
    mov di, cx

    mov dx, 0x1f2
    mov al, cl
    out dx, al

    mov eax, esi
    mov dx, 0x1f3
    out dx, al

    mov cl, 8
    shr eax, cl
    mov dx, 0x1f4
    out dx, al

    shr eax, cl
    mov dx, 0x1f5
    out dx, al

    shr eax, cl
    and al, 0x0f
    or al, 0xe0
    mov dx, 0x1f6
    out dx, al

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

.not_ready:
    nop
    in al, dx
    and al, 0x88
    cmp al, 0x08
    jnz .not_ready

    mov ax, di
    mov dx, 256
    mul dx
    mov cx, ax
    mov dx, 0x1f0

.go_on_read:
    in ax, dx
    mov [ds:ebx], ax
    add ebx, 2
    loop .go_on_read
    ret

load_elf32_init:
	push ecx
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx

    mov dx, [edi + 42]
    mov ebx, [edi + 28]

    add ebx, edi  ; 不要使用[edi], 要加的是edi的内容，那个地址，而不是这个地址的内容（elf文件开头）
    mov cx, [edi + 44]

.each_segment:
    cmp byte [ebx], PT_NULL
    je .PTNULL

    push dword [ebx + 16]
    mov eax, [ebx + 4]
    add eax, edi
    push eax
    push dword [ebx + 8]

    call mem_cpy
    add esp, 12

.PTNULL:
    add bx, dx
    loop .each_segment
	pop ecx
    ret

mem_cpy:
    cld
    push ebp
    mov ebp, esp
    push ecx
	push edi

    mov edi, [ebp + 8]
    mov esi, [ebp + 12]
    mov ecx, [ebp + 16]
    rep movsb

	pop edi
    pop ecx
    pop ebp
    ret


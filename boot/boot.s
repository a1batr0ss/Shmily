INIT_START_SECTOR equ 0x1
INIT_ENTRY equ 0x500

SECTION MBR vstart=0x7c00

    call clear_boot_screen

    mov eax, INIT_START_SECTOR
    mov ebx, INIT_ENTRY
    mov ecx, 40
    call read_disk

    jmp INIT_ENTRY
    
    jmp $

clear_boot_screen:
    mov ax, 0x600
    mov bx, 0x700
    mov cx, 0
    mov dx, 0x184f
    int 0x10
    ret

read_disk:
    mov esi, eax
    mov di, cx

    mov dx, 0x1f2
    mov al, cl
    out dx, al

    mov eax, esi
    ; 保存LBA地址
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

    times 510 - ($-$$) db 0
    dw 0xaa55


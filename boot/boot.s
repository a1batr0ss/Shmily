SECTION MBR vstart=0x7c00

    call clear_boot_screen 
    
    jmp $

clear_boot_screen:
    mov ax, 0x600
    mov bx, 0x700
    mov cx, 0
    mov dx, 0x184f
    int 0x10

    times 510 - ($-$$) db 0
    dw 0xaa55


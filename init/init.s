section init vstart=0x500
jmp init_start

GDT_BASE:
.NULL_DESC:
    dd 0x00000000
    dd 0x00000000
.CODE_DESC:
    dd 0x0000ffff
    dd 0x00cffa00
.DATA_DESC:
    dd 0x0000ffff
    dd 0x00cff200

GDT_SIZE equ $ - GDT_BASE
GDT_LIMIT equ GDT_SIZE - 1
gdt_ptr dw GDT_LIMIT
        dd GDT_BASE

init_start:
    call enable_A20
    lgdt [gdt_ptr]
    call enable_PE

    jmp $

; global flush_gdt
flush_gdt:
    mov eax, [esp + 4]
    lgdt [eax]
    
    jmp 0x8:.flush  ; without set RPL
.flush:
    ret

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


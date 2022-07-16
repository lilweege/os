org 0x7C00
bits 16
    ; save the drive number since dl will be overwritten
    mov [drive_num], dl

    ; set up stack
    mov bp, 0x3000 ; free space
    mov sp, bp


    mov bx, BOOTING_MSG
    call print_cstr

    ; load the kernel from disk
    call load_disk
    
    ; set video mode
    mov ah, 0x00 ; set video mode
    mov al, 0x13 ; 320x200 256 col
    int 0x10

    ; set the A20 line
    ; "Fast A20" is apparently not widely supported
    ; however it works in qemu so this is fine for now
    in al, 0x92
    test al, 2
    jnz A20_already_enabled
    or al, 2
    and al, 0xFE
    out 0x92, al
A20_already_enabled:
    
    ; finally, switch to protected mode
    jmp switch_to_pm



bits 16
switch_to_pm:
    cli                     ; 1. disable interrupts
    lgdt [gdt_descriptor]   ; 2. load the GDT descriptor
    mov eax, cr0
    or eax, 1
    mov cr0, eax            ; 3. set 32-bit mode bit in cr0
    jmp CODE_SEG:init_pm    ; 4. far jump by using a different segment

bits 32
init_pm: ; we are now using 32-bit instructions
    mov ax, DATA_SEG        ; 5. update the segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000        ; 6. update the stack right at the top of the free space
    mov esp, ebp

    call enter_pm           ; 7. Call a well-known label with useful code

bits 32
enter_pm:
    jmp KERNEL_OFFSET



; https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)
bits 16
load_disk:
    mov cx, 10              ; loop index
    mov dl, [drive_num]     ; from drive specified by bios
    mov si, disk_packet     ; address of "disk address packet" 

    mov WORD [num_sectors], 64
    mov WORD [disk_sector], 1
    mov WORD [disk_segment], KERNEL_OFFSET>>4
    mov WORD [disk_offset], 0
    
load_disk_loop:
    mov ah, 0x42
    int 0x13
    jc disk_error
    add WORD [disk_sector], 64
    add WORD [disk_offset], 0x8000 ; 64*512
    mov ah, 0x42
    int 0x13
    jc disk_error
    add WORD [disk_sector], 64
    add WORD [disk_offset], 0x8000

    add WORD [disk_segment], 0x1000 ; (0x8000*2)>>4
    mov WORD [disk_offset], 0

    ; dec cx and jnz
    loop load_disk_loop
    
    mov bx, DISK_SUCCESS_MSG
    call print_cstr
    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_cstr
    jmp $



; ptr in bx
bits 16
print_cstr:
    pusha
print_cstr_loop:
    mov al, [bx]
    cmp al, 0
    je print_cstr_done
    mov ah, 0x0e ; tty mode
    int 0x10     ; print char in al
    inc bx
    jmp print_cstr_loop
print_cstr_done:
    popa
    ret
print_crlf:
    mov bx, CRLF
    call print_cstr
    ret

; number in dx
bits 16
print_hex:
    pusha
    mov cx, 0 ; index from end
print_hex_loop:
    cmp cx, 4 ; loop 4 times
    je print_hex_done
    ; convert last char of dx to ascii
    ; note that ror dx, 4 happens each time
    mov ax, dx
    and ax, 0xF
    add al, '0'
    ; if it is not '0'-'9', it is 'A'-'F'
    cmp al, '9'
    jle print_hex_append
    add al, 'A'-'9'-1
print_hex_append:
    ; bx = len - idx
    mov bx, HEX_OUT + 5
    sub bx, cx
    mov [bx], al ; copy byte to ptr in output
    ror dx, 4    ; move next character into place
    inc cx
    jmp print_hex_loop
print_hex_done:
    mov bx, HEX_OUT
    call print_cstr
    popa
    ret

HEX_OUT:
    db '0x0000', 0
CRLF:
    db 0x0D, 0x0A, 0



drive_num:
    db 0x00

; https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h)
disk_packet:
    db 0x10
    db 0x00
num_sectors:
    dw 0x0000
disk_offset:
    dw 0x0000
disk_segment:
    dw 0x0000
disk_sector:
    dd 0x00000000
    dd 0x00000000


; Global Descriptor Table
; refer to page 34 in os-dev.pdf
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; limit
    dd gdt_start                ; base

gdt_start:
gdt_null: dq 0
; base = 0x0, limit = 0xfffff
; 1st flags : (present)1 (privilege)00 (type)1 -> 1001 b
; 2nd flags  : (granularity)1 (32-bit default)1 (64-bit seg)0 (AVL)0 -> 1100 b
gdt_code: 
; type flags : (code)1 (conforming)0 (readable)1 (accessed)0 -> 1010 b
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b   ; 1st flags, type flags
    db 11001111b   ; 2nd flags, ...
    db 0x00
gdt_data:
; type flags : (code)0 (expand down)0 (writable)1 (accessed)0 -> 0010 b
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:



BOOTING_MSG: db "Hello!", 0x0D, 0x0A, 0
DISK_ERROR_MSG: db "Disk read error", 0x0D, 0x0A, 0
DISK_SUCCESS_MSG: db "Disk read success.", 0x0D, 0x0A, 0

; macros
KERNEL_OFFSET equ 0x7E00 ; link kernel entry to this value
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


; fill the rest of the sector
times 510 - ($-$$) db 0
dw 0xAA55

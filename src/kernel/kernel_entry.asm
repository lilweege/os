extern _start
bits 32
entry: ; loaded at 0x7E00
    call _start
    jmp $ ; unreachable unless the kernel returns (it shouldn't)

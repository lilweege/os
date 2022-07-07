extern _start
bits 32
loop:
    call _start
    ; unreachable unless the kernel returns (it probably shouldn't)
    ; jmp loop
    jmp $

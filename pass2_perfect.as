.entry MISSINGENTRY
MAIN:   mov r1, r2
1BAD:   inc r3
        jmp GHOST
        add r1, r2,,
        bne FAKELABEL,
MAIN:   clr r4
.data 5, 6, 7
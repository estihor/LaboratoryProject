; קובץ תקין לחלוטין לבדיקת אלגוריתם המעבר הראשון

.entry MAIN
.extern EXTERNALLABEL

mcro mycalc
    add r1, r2
    inc r3
endmcro

MAIN:           lea DATAARR, r4
                mycalc
                jmp %LOOP1
LOOP1:          prn #-5
                prn #123
                sub r1, r5
                bne %EXTERNALLABEL
                
                ; פקודות ללא אופרנדים
                rts
                stop

DATAARR:        .data 6, -9, 15, 100
STRLABEL:       .string "Perfect String"

; --- pass2_errors.as ---
; קובץ שגיאות שמיועד להיתפס רק במעבר השני

.entry FAKELABEL
.extern MYEXT

MAIN:   mov r1, GHOSTLABEL
        bne %MYEXT
        stop

MYDATA: .data 5, 6
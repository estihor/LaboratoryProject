; בדיקת קצה למקרואים ותוויות
macro m_error
    sub r1, r2
    ; פה נכניס משהו שיכול לבלבל
    jmp %LOOP
endmacro

macro empty_macro
endmacro

LOOP:   m_error
        empty_macro     ; מקרו ריק - לא אמור לעשות כלום אבל גם לא לקרוס
mov:    .data 10        ; שגיאה: שם תווית הוא מילה שמורה (mov)
r1:     .string "bad"   ; שגיאה: שם תווית הוא שם אוגר
LABEL:  .data 5
LABEL:  .data 6         ; שגיאה: תווית כפולה (מוגדרת פעמיים)
        m_error
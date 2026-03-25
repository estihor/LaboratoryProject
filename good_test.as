; This file is completely valid and should pass without ANY errors!
.entry MaxLenLabelExactlyThirtyOneChar
.extern ExternalFunc

; 1. Testing max length label (exactly 31 characters)
MaxLenLabelExactlyThirtyOneChar: mov r1, r2

; 2. Testing all valid addressing modes for add 
add #100, MaxLenLabelExactlyThirtyOneChar
add r5, r6
add MaxLenLabelExactlyThirtyOneChar, r1

; 3. Testing relative addressing (mode 2) - valid only for jumps
LOOP: jmp %ExternalFunc
      bne %LOOP

; 4. Testing a very long string, but perfectly legal
LongStr: .string "This is a very long string but it is totally legal!"

; 5. Testing valid data with tricky spaces and signs
NUMBERS: .data +5, -10 ,0 , 99

; 6. A line that is exactly 79 characters long (plus \n makes it 80 - legal)
; 12345678901234567890123456789012345678901234567890123456789012345678901234567
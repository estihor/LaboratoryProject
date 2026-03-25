; 1. LINE EXCEEDS 80 CHARACTERS: This should trigger our new fgets logic!
ThisLineIsWayTooLongAndItWillDefinitelyExceedTheEightyCharacterLimitWeDefinedInTheProjectSpecs: mov r1, r2

; 2. LABEL TOO LONG: Over 31 characters (exactly 32 chars long)
ThisLabelIsThirtyTwoCharactersLo: clr r1

; 3. INVALID LABEL NAMES: Using registers or instructions
r1: add r2, r3
mov: inc r5

; 4. ILLEGAL ADDRESSING MODES
lea #5, r1
add r1, #10
jmp %r3

; 5. OPERAND COUNT ERRORS: Too many or too few
not r1, r2
cmp r1

; 6. SYNTAX ERRORS: Missing commas or extra commas
sub r1 r2
.data 1,, 2
.data 1 2 3

; 7. STRING ERRORS: Missing closing quote and extraneous text
.string "Unclosed string
.string "Valid string" garbage

; 8. DUPLICATE LABELS
DupLabel: clr r1
DupLabel: inc r2
; Number of loop iterations to run. For debug, choose a small number (1 to 10)
; then increase it when your code works.
NUM_LOOPS = 100


; Unitialized data section.
.bss
; *****************************************************************************
; TO DO: Declare other variables above; only u8_e is declared here. See the
; next TO DO comment for what variables to declare.
;
; You MUST place .space declarations for 32-bit variables first, followed by
; 16-bit variables, followed by 8-bit variables, to preserve proper alignment.
; For example,
; u32_b: .space 4
; u16_c: .space 2
; u8_a:  .space 1
; is the correct order.
; *****************************************************************************
i32_a: .space 4
i32_b: .space 4
i32_c: .space 4
i16_d: .space 2
i16_d2: .space 2
u8_e:  .space 1

;..............................................................................
;Code Section in Program Memory
;..............................................................................

.text                           ; Start of Code section.

.global _main                   ; Therefore, it must be visible outside this file.
_main:                          ; _main is called after C startup code runs.

    ; *************************************************************************
    ; TO DO: Translate the assignments below to PIC24 assembly. Declare these
    ; variables after the previous TO DO comment.
    ; *************************************************************************
    ;; int32_t i32_a = 0xB3E83894;
    ;; int32_t i32_b = 0x348AC297;
    ;; int32_t i32_c = 0xA55A93CD;
    ;; int16_t i16_d = 0xA4F5;
    
    MOV #0x3894, WO
    MOV W0, i32_a
    MOV #0xB3E8, W0
    MOV W0, i32_a + 2
    
    MOV #0xC297, WO
    MOV W0, i32_b
    MOV #0x348A, W0
    MOV W0, i32_b + 2
    
    MOV #0x93CD, WO
    MOV W0, i32_c
    MOV #0xA55A, W0
    MOV W0, i32_c + 2

    MOV #0xA4F5, WO
    MOV W0, i16_d
    
    ; u8_e = 0;
    clr.b u8_e

    ; do {
    do_top:

    mov.b u8_e, WREG
    mov.b W0, W7
        ; *********************************************************************
        ; TO DO: To print out the variables in your code, set:
        ; W1:W0 = i32_a
        ; W3:W2 = i32_b
        ; W5:W4 = i32_c
        ; W6 = i16_d
        ; W7 = u8_e
        ; Note: since only u8_e is used in this sample,
        ; this sample code just sets it. You'll have to do all of them. Make
        ; sure your code comes AFTER this comment but BEFORE the call
        ; instruction.
        ; *********************************************************************
        ; Your code goes here.
	
    MOV i32_a, W0
    MOV i32_a + 2, W1
    MOV i32_b, W2
    MOV i32_b + 2, W3
    MOV i32_c, W4
    MOV i32_c + 2, W5
    MOV i16_d, W6
    MOV.b u8_e, WREG
    MOV.b W0, W7
	

	
    call _check

        ; *********************************************************************
        ; The code fragment to implement:
        ; *********************************************************************
        ;; if (i32_c & 0x08000000) {
        ;;     if (i32_b < i32_a) {
        ;;         i32_b = i32_b + i32_a;
        ;;     } else {
        ;;         i32_a = i32_b + (i32_a >> 2) + ((int32_t) i16_d);
        ;;     }
        ;; } else {
        ;;     i32_b = i32_a - i32_b;
        ;;     i32_a = i32_a + 0xA2588080 ;
        ;;     i16_d = ~( (i16_d ^ 0x00A5) + 128) ; //128 is in decimal!
        ;; }
        ;; i32_c = ~( (i32_c << 1) + i32_b);

        ; *********************************************************************
        ; TO DO: Implement the templates below:
        ; 1. Fill in all register assignments.
        ; 2. Write the code for each line. DO NOT rely on previous register
        ;    values from other lines of code. Instead, simply load in all inputs
        ;    for the line of C code you're implementing.
        ; *********************************************************************

        ;      W?       W?
        ; if (i32_c & 0x08000000) {
        ; Input
        ; Process
        ; Output
    mov i32_c, W1
    mov i32_c + 2, W2
    and W1, #0x0000,W1
    bra NZ, end_if_1
    cp W2, #0x0800
    bra NZ, end_if_1
   
	
	
            ; Replace this line with your register assigments.
            ; if (i32_b < i32_a) {
            ; Input
            ; Process
            ; Output
    if_body_1:
    MOV i32_b, W1
    MOV i32_b + 2, W2
    MOV i32_a, W3
    MOV i32_a + 2, W4
    
    cp W1, W3
    ; bra GEU, end_if_2 ;; It doesn't branch because the cp is just to check if there is a borrow.
    cpb W2, W4
    bra GEU, end_if_2
    
                ; Replace this line with your register assigments.
                ; i32_b = i32_b + i32_a;
                ; Input
                ; Process
                ; Output
    if_body_2:
    MOV i32_b, W1
    MOV i32_b + 2, W2
    MOV i32_a, W3
    MOV i32_a + 2, W4
    
    ADD W1, W3, W1
    ADD W2, W4, W2
    MOV W1, i32_b
    MOV W2, i32_b + 2
    
           ; Code may go here...
           ; } else {
	   
    end_if_2:
    else_body_1:
    
           ; ...and may also go here.

                ; Replace this line with your register assigments.
                ; i32_a = i32_b + (i32_a >> 2) + ((int32_t) i16_d);
                ; Input
                ; Process
                ; Output
    MOV i32_a, W1
    MOV i32_a + 2, W2
    MOV i32_b, W3
    MOV i32_b + 2, W4
    MOV i16_d, W5
    
    MOV #0x0000, WO
    MOV W0, i16_d2
    
    MOV i16_d32, W6
    
    LSR W2, W2
    RRC W1, W1
    
    LSR W2, W2
    RRC W1, W1
    
    ADD W3, W1, W1
    ADDC W4, W2, W2
    
    ADD W1, W5, W1
    ADDC W2, W6, W2
    
    MOV W1, i32_a
    MOV W2, i32_a + 2
           ; Code may go here...
           ; }
           ; ...and may also go here.	
	   
    end_else_1:
    end_if_1:
    
      ; Code may go here...
      ; } else {
      ; ...and may also go here.
    else_body_2:
    MOV i32_a, W1
    MOV i32_a + 2, W2
    MOV i32_b, W3
    MOV i32_b + 2, W4
    MOV i16_d, W5
    
    
          ; Replace this line with your register assigments.
          ; i32_b = i32_a - i32_b;
          ; Input
          ; Process
          ; Output
    SUB W1, W3, W3
    SUBB W2, W4, W4
    MOV W3, i32_b
    MOV W4, i32_b + 2
    
    
    
          ; Replace this line with your register assigments.
          ;   i32_a = i32_a + 0xA2588080 ;
          ; Input
          ; Process
          ; Output
    ADD W1, 0x8080
    ADDC W2, 0xA258
    MOV W1, i32_a
    MOV W2, i32_a + 2
    
          ; Replace this line with your register assigments.
          ;  i16_d = ~( (i16_d ^ 0x00A5) + 128) ; ; //128 is in decimal!
          ; Input
          ; Process
          ; Output
    XOR W5, #0x00A5, W5
    ADD W5, #128
    COM W5, W5
    MOV W5, u16_d
    
        ; Code may go here...
        ; }
    end_else_2:
        ; ...and may also go here.

        ; Replace this line with your register assigments.
        ; i32_c = ~( (i32_c << 1) + i32_b);
        ; Input
        ; Process
        ; Output
    
    MOV i32_b, W1
    MOV i32_b + 2, W2
    MOV i32_c, W3
    MOV i32_c + 2, W4
    
    SL W3, W3
    RLC W4, W4
    ADD W3, W1, W3
    ADDC W4, W2, W4
    COM W3, W3
    COM W4, W4
    MOV W3, i32_c
    MOV w4, i32_c + 2
	

        ; The two lines of C code below have already been implemented.
        ; Do not modify them.

        ; u8_e++
        inc.b u8_e
        ;        WREG       W1
        ; } while (u8_e < NUM_LOOPS);
        mov.b u8_e, WREG
        mov.b #NUM_LOOPS, W1
        cp.b W0, W1
        bra LTU, do_top
        bra GEU, do_end
    do_end:

done:
    goto    done

.end       ;End of program code in this file

/** \endcond */

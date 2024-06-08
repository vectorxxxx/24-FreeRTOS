                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY

__Vectors       DCD     0
                DCD     Reset_Handler              ; Reset Handler


                AREA    |.text|, CODE, READONLY
                
; Reset handler
Reset_Handler   PROC
                IMPORT  main
				LDR     SP, =0x20000000+0x100
                BL      main
                ENDP
				END
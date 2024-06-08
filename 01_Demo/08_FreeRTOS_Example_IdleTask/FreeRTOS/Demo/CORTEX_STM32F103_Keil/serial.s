; generated by Component: ARM Compiler 5.06 update 5 (build 528) Tool: ArmCC [4d3621]
; commandline ArmCC [--debug -c -S -o.\serial.s --depend=.\serial.d --cpu=Cortex-M3 --apcs=interwork -O0 --diag_suppress=9931 -I..\CORTEX_STM32F103_Keil -I..\Common\include -I..\..\Source\include -I.\STM32F10xFWLib\inc -ID:\Program\MDK51_32\Keil\STM32F1xx_DFP\2.2.0\Device\Include -D__MICROLIB -D__UVISION_VERSION=539 -DSTM32F10X_MD -DRVDS_ARMCM3_LM3S102 --omf_browse=.\serial.crf serial\serial.c]
        THUMB
        REQUIRE8
        PRESERVE8

        AREA ||.text||, CODE, READONLY, ALIGN=2

SerialPortInit PROC
        PUSH     {r4,lr}
        SUB      sp,sp,#0x20
        MOV      r4,#0x1c200
        MOVS     r1,#1
        MOV      r0,#0x4004
        BL       RCC_APB2PeriphClockCmd
        MOV      r0,#0x400
        STRH     r0,[sp,#4]
        MOVS     r0,#4
        STRB     r0,[sp,#7]
        ADD      r1,sp,#4
        LDR      r0,|L0.160|
        BL       GPIO_Init
        MOV      r0,#0x200
        STRH     r0,[sp,#4]
        MOVS     r0,#3
        STRB     r0,[sp,#6]
        MOVS     r0,#0x18
        STRB     r0,[sp,#7]
        ADD      r1,sp,#4
        LDR      r0,|L0.160|
        BL       GPIO_Init
        STR      r4,[sp,#8]
        MOVS     r0,#0
        STRH     r0,[sp,#0xc]
        STRH     r0,[sp,#0xe]
        STRH     r0,[sp,#0x10]
        STRH     r0,[sp,#0x12]
        MOVS     r0,#0xc
        STRH     r0,[sp,#0x14]
        MOVS     r0,#0
        STRH     r0,[sp,#0x16]
        STRH     r0,[sp,#0x18]
        MOV      r0,#0x200
        STRH     r0,[sp,#0x1a]
        MOVS     r0,#0
        STRH     r0,[sp,#0x1c]
        ADD      r1,sp,#8
        LDR      r0,|L0.164|
        BL       USART_Init
        MOVS     r1,#1
        LDR      r0,|L0.164|
        BL       USART_Cmd
        ADD      sp,sp,#0x20
        POP      {r4,pc}
        ENDP

fputc PROC
        MOV      r2,r1
        LDR      r1,|L0.164|
        NOP      
|L0.144|
        LDRH     r3,[r1,#0]
        AND      r3,r3,#0x80
        CMP      r3,#0
        BEQ      |L0.144|
        STRH     r0,[r1,#4]
        BX       lr
        ENDP

        DCW      0x0000
|L0.160|
        DCD      0x40010800
|L0.164|
        DCD      0x40013800

        AREA ||.arm_vfe_header||, DATA, READONLY, NOALLOC, ALIGN=2

        DCD      0x00000000

        EXPORT SerialPortInit [CODE]
        EXPORT fputc [CODE]

        IMPORT ||Lib$$Request$$armlib|| [CODE,WEAK]
        IMPORT RCC_APB2PeriphClockCmd [CODE]
        IMPORT GPIO_Init [CODE]
        IMPORT USART_Init [CODE]
        IMPORT USART_Cmd [CODE]

        ATTR FILESCOPE
        ATTR SETVALUE Tag_ABI_PCS_wchar_t,2
        ATTR SETVALUE Tag_ABI_enum_size,1
        ATTR SETVALUE Tag_ABI_optimization_goals,6
        ATTR SETSTRING Tag_conformance,"2.09"
        ATTR SETVALUE AV,18,1

        ASSERT {ENDIAN} = "little"
        ASSERT {INTER} = {TRUE}
        ASSERT {ROPI} = {FALSE}
        ASSERT {RWPI} = {FALSE}
        ASSERT {IEEE_FULL} = {FALSE}
        ASSERT {IEEE_PART} = {FALSE}
        ASSERT {IEEE_JAVA} = {FALSE}
        END
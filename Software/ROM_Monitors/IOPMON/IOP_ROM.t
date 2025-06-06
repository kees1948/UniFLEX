        nam    mon6809
        opt     nop
*
* monitor program for 6809
*
* REV 3.0 DATE 12 11 83
*
* C.SCHOENMAKERS
*
*
* PROGRAM ASSUMES A 2 KBYTE PROM !!..
*
*
        OPT    PAG
        PAG
*
* SYSTEM EQUATES
*

ACIAC   EQU    $807C     ACIA ADDRESS, use last port in system
ACIAD   EQU    ACIAC+1   DATA PORT

FIFO    EQU     $0000


RAMBASE EQU    $7f00
        ORG    RAMBASE
IOVR    RMB    2         IRQ RAM VECTOR
NIOR    RMB    2         NMI RAM VECTOR
SWIVEC  RMB    2         SWI RAM VECTOR
FIRQVC  rmb     2       firq vector
SW2VEC  rmb     2       swi 2 ram vector
SW3VEC  rmb     2       swi 3 ram vector
TRAPVC	rmb		2		trap vector
BEGA    RMB    2         BEGIN ADDRESS
ENDA    RMB    2         END ADDRESS
INVEC   RMB    2         INPUT CHAR VECTOR
OUTVEC  RMB    2         OUTPUT CHAR VECTOR
TW      RMB    2         TEMP WORK REG
BYTEC1  RMB    1         BYTE COUNT
XTEMP3  RMB    2         TEMP FOR X
CKSM    RMB    1         CHECKSUM
XHI     RMB    1         ADDRESS BUILD
XLO     RMB    1
TEMP1   RMB    1         SCRATCH
TSP     RMB    2         USER STACK SAVE
MCONT   RMB    1         COUNTER
STATUS  RMB    1         ECHO / NOECHO FLAG
CPUMOD	rmb		1		 6809/6309	


TSTACK  EQU    RAMBASE+$80-24 USER STACK
CSTACK  EQU    RAMBASE+$7F MONITOR STACK

ROMBASE EQU      $F800
        ORG    ROMBASE
*
* PROGRAM STARTS HERE
*
	FDB		PDATA1		printstring (X)
	FDB		HEXBYT		print one hex byte (A)
	FDB		OUTS
	FDB		OTCAR		write char to terminal
	FDB		TINIT
	FDB		INCAR

REENT   JMP     CONREN   JUMP INTO WARM MONITOR
IOV     JMP    [IOVR]     RAM VECTOR IRQ
POWDWN  JMP    [NIOR]     RAM VECTOR NMI
SWIV    JMP    [SWIVEC]  RAM VECTOR SWI
FIRQV   jmp     [FIRQVC] ram firq vector
SWI2V   jmp     [SW2VEC]
SWI3V   jmp     [SW3VEC]
TRAP    jmp     [TRAPVC]

CONREN  PSHS   CC,D,DP,X,Y,U SAVE REMAINDER OF REGS
        STS     TSP
        JMP     CONTRL

RDRON
NOECHO  LDAA   STATUS    UPDATE FLAG
        ORAA   #$80
        JMP    ECHO1     STORE AND RETURN

LOAD    EQU    *
        BSR    RDRON     PUT READER ON
LOAD3   JSR    INCH      READ CHAR
        CMPA   #'S       START OF RECORD?
        BNE    LOAD3     NO
        JSR    INCH
        CMPA   #'9       END RECORD?
        BEQ    LOAD21    YES
        CMPA   #'1       DATA RECORD?
        BNE    LOAD3     TRY AGAIN
        CLR    CKSM      CLEAR CHECKSUM
        BSR    INBYTE      READ COUNT
        SUBA   #2
        STAA   BYTEC1    KEEP COUNT
        BSR    BADDR     READ ADDRESS
LOAD11   BSR    INBYTE      DATA
        DEC    BYTEC1    LAST BYTE OF RECORD?
        BEQ    LOAD15
        STAA   0,X+        PUT DATA IN MEM
        BRA    LOAD11

OUTQM   LDAA   #'?       OUTPUT '?'
        BRA    OUTCH

LOAD15  INC    CKSM      MUST BE ZERO
        BEQ    LOAD3     YES
        BSR    OUTQM     PUT MESSAGE

LOAD21  EQU    *
C1      JMP    CONTRL    LEAVE LOADER

OUTHL   LSRA
        LSRA
        LSRA
        LSRA
OUTHR   ANDA   #%00001111 MASK BITS
        ADDA   #$30      CONVERT TO ASCII
        CMPA   #$39
        BLS    OUTCH
        ADDA   #7        WAS ALPHA

OUTCH   JMP    OUTEE
INCH    JMP    INEEE

BADDR   BSR    INBYTE      READ HIBYTE
        bcs    OUTQA
        STAA   XHI
        BSR    INBYTE      READ LOBYTE
        bcs    OUTQA
        STAA   XLO
BADR1   LDX    XHI       ADDRESS > X
        RTS
OUTQA   BSR     OUTQM
        LDX     XHI
        RTS

INBYTE    BSR    INHEX     READ LEFT NIBBLE
        bcs     bytex
        ASLA
        ASLA
        ASLA
        ASLA
        tfr     a,b
        BSR    INHEX     READ RIGHT NIBBLE
        bcs     bytex
        ABA
        tfr     a,b
        ADDB   CKSM      UPDATE CHECKSUM
        STAB   CKSM
        clc
bytex   RTS              CHAR IN A'

OUTSTG  LDX    #MCL1     CRLF
        BRA    PDATA1

PDATA2  BSR    OUTCH     PUT CHAR
PDATA1  LDAA   0,X+      READ CHAR
        CMPA   #4        EOT?
        BNE    PDATA2    NO OUTPUT IT
        RTS

CHANGE  EQU    *
        BSR     BADDR  READ ADDRESS
        BSR    OUTS      SEPARATE
CHA1    BSR    OUT2HS    OUTPUT DATA
        BSR    INCH      COMMAND
        CMPA   #$0D      CR?
        BEQ    C11       EXIT
        CMPA   #'^       PREV.?
        BEQ    CHA11
        clc
        BSR    INHEX+3
        BCS    CHA12     NO CHANGE
        BSR    INBYTE+2
        BCS    CHA12     NO CHANGE
        DEX    ADJUST POINTER
        STAA   0,X         PUT DATA
        CMPA   0,X         CHECK DATA
        BEQ    CHA14
        JSR    OUTQM     WARN USER
CHA14   INX    ADJUST POINTER
CHA12   STX    XHI       SAVE X
        BSR    OUTSTG    PUT CRLF
        LDX    #XHI      OUTPUT NEW ADDRESS
        BSR    OUT4HS
        LDX    XHI       RESTORE POINTER
        BRA    CHA1
CHA11   leax    -2,x
        BRA    CHA12
C3      LBSR    OUTQM
C11     JMP    CONTRL

INHEX   LBSR   INCH      GET CHAR
		oraa	#$20     make lower
        SUBA   #$30      CHECK IF VALID
        BMI    C8
        CMPA   #$09      NUMERAL?
        BLS    IN1HG     YES
        CMPA   #$31      ALPHA
        BLO    C8
        CMPA   #$36
        BHI    C8        NO
        SUBA   #$27
IN1HG   clc
        RTS
C8      sec
        rts

HEXBYT	PSHS	A
	JSR    OUTHL
	PULS	A
	JMP    OUTHR

OUT2H   LDAA   0,X         GET CHAR
OUT2HA  JSR    OUTHL     LEFT NIBBLE
        LDAA   0,X+
        JMP    OUTHR     RIGHT NIBBLE

OUT4HS  BSR    OUT2H     LEFT BYTE
OUT2HS  BSR    OUT2H     RIGHT BYTE
OUTS    LDAA   #'        SPACE
        JMP    OUTCH

START   EQU    *
        LDS    #TSTACK   LOAD STACK POINTER
        STS    TSP       SAVE IT
        CLRA
        TFR     A,DP
        FCB     $11,$3D,$03             set 6309
*
        LDA    #$80      SET DP REG
        STA    0,S
        clr    5,s      set dp
        LDX    #SFE      SOFTWARE INTERR. ENTRY
        STX    SWIVEC
        LDX    #HWE      HARD WARE INTERRUPT
        STX    IOVR
        STX    NIOR       NMI VECTOR
        STX    SW2VEC
        STX    SW3VEC
	STX   TRAPVC
        LDX    #INCAR    SET INPUT SWITCH
        STX    INVEC
        LDX    #OTCAR    SET OUTPUT SWITCH
        STX    OUTVEC
        CLR    STATUS    ENABLE ECHO
	jsr	TINIT
        ldx     #CONTRL
        stx     12,s     set default go
* clear FIFO
        ldx     #0
fifo01  clr     0,x+
        cmpx    #$00ff
        bne     fifo01

CONTRL  LDS    #CSTACK   MONITOR STACK
        JSR    RDROFF
        JSR    PNHOFF

        LDX    #MCL0     PROMPT
        JSR    PDATA1
        JSR    INCMD     WAIT FOR ALPHA
        TAB              SAVE CHAR
        LBSR    OUTS      SEPARATE
	oraa	#$20
        CMPB    #'c     CALL FUNCTION
        LBEQ    MCALL
        CMPB   #'p       PRINT/PUNCH
        BEQ    CL1
        CMPB   #'l       LOAD FILE?
        BNE    *+5
        JMP    LOAD      YES
        CMPB   #'m       CHANGE?
        BNE    *+5
        JMP    CHANGE    YES
        CMPB   #'r       PRINT REG?
        BEQ    PRNTRG
        cmpb    #'b
        bne     contr1
        ldx     #HWE
        stx     SWIVEC
contr1  CMPB   #'g       GO?
        BNE    CONTRL    NO
* GO cmd
        JSR    INCH      HOW?
        CMPA   #$0D      CR?
        BEQ    CONTIN    WITH CURRENT ADDRESS
        JSR    INHEX+3   GET NEW ADDRESS
        BCS    CONTRL
        JSR    INBYTE+2
        BCS    CONTRL
        JSR    BADDR+2
        bcs    CONTRL
        LDD    XHI      TRANSFER ADDRESS
        LDX    TSP       TO PSEUDO REGS
	STD	   12,x
CONTIN  LDS    TSP       RESTORE USER STACK
        RTI              AND RUN...

CL1     BSR    INCH1     GET SUB COMM.
        CMPA   #'h       PUNCH?
        BEQ    PUNCH
        JMP    PRINT

SFE     EQU    *
	tst    13,s
	bne	*+4
	dec    12,s
	dec    13,s

HWE     EQU    *
        STS    TSP       SAVE STACK POINTER

PRNTRG  BSR    PRTREG    PRINT REGS
C4      JMP    CONTRL

PRTREG  LDX    TSP       GET REG AREA
        JSR    OUT2HS    CC
        JSR    OUT4HS    A,B
	jsr   OUT4HS	 wh/wl
PRT01   JSR    OUT2HS    DP
        JSR    OUT4HS    X
        JSR    OUT4HS    Y
        JSR    OUT4HS    U
        JSR    OUT4HS    PC
        LDX    #TSP
        JMP    OUT4HS    SP  +RTS

TINIT   LDA    #3        RESET ACIA
        STA    ACIAC
        LDA    #$11      INIT ACIA
        STA    ACIAC
        lda    #$77
        sta    ACIAC+2  set BRG
	RTS

INCH1   JMP    INEEE
OUTCH1  JMP    OUTEE

PNHOFF  RTS
RDROFF
ECHO    LDAA   STATUS
        ANDA   #$7F      RESET BIT
ECHO1   STAA   STATUS    STORE RESULT
        RTS

PUNCH   EQU    *
        BSR     SADDR  GET BEGA ENDA
        JSR    PNCHHD    PU'CH LEADER
PUN35   LDX    BEGA      MAKE TEMP POINTER
        STX    TW
PUN11   LDD    ENDA      CALC. SIZE
        SUBD   TW
        CMPD   #24       STANDARD RECORD
        BCS    PUN23
PUN22   LDAB   #23
PUN23   ADDB   #4
        STAB   MCONT     FRAMES
        SUBB   #3
        STAB   TEMP1     DATA BYTES
        LDX    #MTAPE1   'S1'
        BSR    PUN40
PUN50   LDX    TW        CHECK DONE
        DEX
        CPX    ENDA
        BLS    PUN11
        JMP    PNCHTR    PUNCH TRAILER

PUN40   JSR    PDATA1    PUNCH STRING
        CLRB   CLEAR CHECKSUM
        LDX    #MCONT    PUNCH FRAMES
        BSR    PUNT2
        LDX    #TW       PUNCH ADDRESS
        BSR    PUNT2
        BSR    PUNT2
        LDX    TW        DATA POINTER
PUN32   BSR    PUNT2     PUNCH DATA
        DEC    TEMP1
        BNE    PUN32
        STX    TW
        COMB             PUNCH CHECKSUM
        PSHB
        TSX
        BSR    PUNT2
        PULB
        RTS
PUNT2   ADDB   0,X         UPDATE CHECKSUM
        JMP    OUT2H

SADDR   LDX    #STRG1    PUT BEGA HEADER
        JSR    PDATA1
        LDX    #BEGA     PUT OLD BEGA
        JSR    OUT4HS
        JSR    INEEE     GET DATA
        CMPA   #'        SPACE;ALTER?
        BNE    SAD1      NO
        JSR    BADDR     NEW ADDRESS
        STX    BEGA
SAD1    LDX    #STRG2    ENDA HEADER
        JSR    PDATA1
        LDX    #ENDA     PUT OLD ENDA
        JSR    OUT4HS
        JSR    INEEE
        CMPA   #'        SPACE;ALTER?
        BNE    SAD2      NO
        JSR    BADDR     GET NEW ENDA
        STX    ENDA
SAD2    RTS

PRINT   EQU    *
        BSR    SADDR     GET BEGA ENDA
        LDX    BEGA      KEEP TEMP POINTER
        STX    TW
PRT3    JSR    OUTSTG    CRLF
        LDX    #TW       OUTPUT ADDRESS
        JSR    OUT4HS
        LDX    TW        OUTPUT DATA
        LDAB   #16       BLOCK SIZE
PRT1    LDAA   0,X         GET DATA
        JSR    OUT2HS    OUTPUT IN HEX
        DECB
        BNE    PRT1
        LDX    TW        RESTORE POINTER
        LDAB   #16       RESTORE SIZE
PRT2    LDAA   0,X+        GET DATA
        BSR    SELCAR    OUTPUT IN ASCII
        DECB
        BNE    PRT2
        STX    TW        CHECK DONE
	DEX
        CMPX   ENDA
        BLO    PRT3
        JMP    CONTRL    DONE

SELCAR  CMPA   #$20      CHECK LITERAL
        BLO    PRT4      NO
        CMPA   #$7F      UPPER LIMIT
        BLS    PRT5
PRT4    LDAA   #'.       NOT PRINTABLE
PRT5    BRA    OUTEE

INCMD   BSR    INEEE     GET CHAR
        BITA   #$40      ALPHA?
        BEQ    INCMD     WAIT UNTIL
        RTS

OUTEE   JMP    [OUTVEC]  USE RAM VECTOR

OTCAR   PSHB             SAVE B
OT1     LDAB   ACIAC    CHECK STATUS
        BITB   #%00000010 TRANSMITTER BIT
        BEQ    OT1
        STAA   ACIAD    TRANSMITTER
EXIT1   PULB   RESTORE B

EXIT    RTS

INEEE   JMP    [INVEC]   USE RAM VECTOR

INCAR   PSHB             SAVE B
INC1    LDAB   ACIAC    STATUS
        BITB   #%00000001
        BEQ    INC1
        LDAA   ACIAD    GET DATA
        ANDA   #$7F
        CMPA   #$7F      SKIP RUBOUT
        BEQ    INC1
        TST    STATUS    ECHO?
        BMI    EXIT1
        BRA    OT1       ECHO


PNCHHD  RTS

PNCHTR  BSR    PNCHHD    50 NULLS
        LDX    #MTAPE3   S9
        JSR    PDATA1
C5      JMP    CONTRL    EXIT

MCALL   jsr    BADDR
        bcs    CON1
        LDD    XHI      TRANSFER ADDRESS
        LDX    TSP       TO PSEUDO REGS
        STD    10,X
        LDS    TSP       RESTORE USER STACK
        PULS    CC,D,DP,X,Y,U
        JSR     [0,S++]
        PSHS    CC,D,DP,X,Y,U
        STS     TSP
CON1    JMP     CONTRL


MCL0    FCB    $D,$A,0,0,'*','*','*',$20
MCL1    FCB    $D,$A,0,0,4
STRG1   FCB    $D,$A,0,0,'B','E','G','A',$20,4
STRG2   FCB    $D,$A,0,0,'E','N','D','A',$20,4
MTAPE1  FCB    $D,$A,0,0,'S','1',4
MTAPE3  FCB    $D,$A,0,0,'S','9',0,0,4


        ORG    $FFF0
	FDB 	TRAP	reserved
        FDB     SWI3V
        FDB     SWI2V
        FDB     FIRQV
        FDB    IOV        IRQ INPUT
        FDB    SWIV      SWI VECTOR
        FDB    POWDWN    NMI
        FDB    START     RESET VECTOR

        END

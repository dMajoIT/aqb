'
' _brt: BASIC runtime, minimal core needed to support the compiler's generated code
'

OPTION EXPLICIT
OPTION PRIVATE

' ERROR codes

PUBLIC EXTERN ERR AS INTEGER

PUBLIC CONST AS INTEGER ERR_ILLEGAL_FUNCTION_CALL  = 5
PUBLIC CONST AS INTEGER ERR_OUT_OF_MEMORY          = 7
PUBLIC CONST AS INTEGER ERR_SUBSCRIPT_OUT_OF_RANGE = 9

PUBLIC DECLARE SUB _debug_puts (BYVAL s AS STRING)
PUBLIC DECLARE SUB _debug_puts1(BYVAL s AS BYTE)
PUBLIC DECLARE SUB _debug_puts2(BYVAL s AS INTEGER)
PUBLIC DECLARE SUB _debug_puts4(BYVAL s AS LONG)
PUBLIC DECLARE SUB _debug_putu1(BYVAL s AS UBYTE)
PUBLIC DECLARE SUB _debug_putu2(BYVAL s AS UINTEGER)
PUBLIC DECLARE SUB _debug_putu4(BYVAL s AS ULONG)
PUBLIC DECLARE SUB _debug_putf (BYVAL f AS SINGLE)
PUBLIC DECLARE SUB _debug_puttab
PUBLIC DECLARE SUB _debug_putnl
PUBLIC DECLARE SUB _debug_cls
PUBLIC DECLARE SUB _debug_break

' AmigaOS library bases

PUBLIC EXTERN SysBase AS VOID PTR
PUBLIC EXTERN DOSBase AS VOID PTR
PUBLIC EXTERN MathTransBase AS VOID PTR

' math support

PUBLIC DECLARE FUNCTION fix   (BYVAL f AS SINGLE) AS INTEGER
PUBLIC DECLARE FUNCTION int   (BYVAL f AS SINGLE) AS INTEGER
PUBLIC DECLARE FUNCTION cint  (BYVAL f AS SINGLE) AS INTEGER
PUBLIC DECLARE FUNCTION clng  (BYVAL f AS SINGLE) AS LONG

PUBLIC CONST AS SINGLE PI = 3.14159

PUBLIC DECLARE FUNCTION ATN  (BYVAL f AS SINGLE) AS SINGLE LIB  -30 MathTransBase (d0)
PUBLIC DECLARE FUNCTION SIN  (BYVAL f AS SINGLE) AS SINGLE LIB  -36 MathTransBase (d0)
PUBLIC DECLARE FUNCTION COS  (BYVAL f AS SINGLE) AS SINGLE LIB  -42 MathTransBase (d0)
PUBLIC DECLARE FUNCTION TAN  (BYVAL f AS SINGLE) AS SINGLE LIB  -48 MathTransBase (d0)
PUBLIC DECLARE FUNCTION EXP  (BYVAL f AS SINGLE) AS SINGLE LIB  -78 MathTransBase (d0)
PUBLIC DECLARE FUNCTION LOG  (BYVAL f AS SINGLE) AS SINGLE LIB  -84 MathTransBase (d0)
PUBLIC DECLARE FUNCTION SQR  (BYVAL f AS SINGLE) AS SINGLE LIB  -96 MathTransBase (d0)
PUBLIC DECLARE FUNCTION ASIN (BYVAL f AS SINGLE) AS SINGLE LIB -114 MathTransBase (d0)
PUBLIC DECLARE FUNCTION ACOS (BYVAL f AS SINGLE) AS SINGLE LIB -120 MathTransBase (d0)

' STR$ support

PUBLIC DECLARE FUNCTION _s1toa   (BYVAL b AS BYTE    ) AS STRING
PUBLIC DECLARE FUNCTION _s2toa   (BYVAL i AS INTEGER ) AS STRING
PUBLIC DECLARE FUNCTION _s4toa   (BYVAL l AS LONG    ) AS STRING
PUBLIC DECLARE FUNCTION _u1toa   (BYVAL b AS UBYTE   ) AS STRING
PUBLIC DECLARE FUNCTION _u2toa   (BYVAL i AS UINTEGER) AS STRING
PUBLIC DECLARE FUNCTION _u4toa   (BYVAL l AS ULONG   ) AS STRING
PUBLIC DECLARE FUNCTION _ftoa    (BYVAL f AS SINGLE  ) AS STRING
PUBLIC DECLARE FUNCTION _booltoa (BYVAL b AS BOOLEAN ) AS STRING

' VAL* support

PUBLIC DECLARE FUNCTION _str2i4_ (BYVAL s AS STRING, BYVAL l AS INTEGER, BYVAL b AS INTEGER) AS LONG
PUBLIC DECLARE FUNCTION _str2f_  (BYVAL s AS STRING, BYVAL l AS INTEGER, BYVAL b AS INTEGER) AS SINGLE

PUBLIC DECLARE FUNCTION VAL      (BYVAL s AS STRING) AS SINGLE
PUBLIC DECLARE FUNCTION VALINT   (BYVAL s AS STRING) AS INTEGER
PUBLIC DECLARE FUNCTION VALUINT  (BYVAL s AS STRING) AS UINTEGER
PUBLIC DECLARE FUNCTION VALLNG   (BYVAL s AS STRING) AS LONG
PUBLIC DECLARE FUNCTION VALULNG  (BYVAL s AS STRING) AS ULONG

' string utils

PUBLIC DECLARE FUNCTION LEN   (BYVAL s AS STRING)  AS LONG
PUBLIC DECLARE FUNCTION CHR$  (BYVAL i AS INTEGER) AS STRING
PUBLIC DECLARE FUNCTION ASC   (BYVAL s AS STRING)  AS INTEGER
PUBLIC DECLARE FUNCTION MID$  (BYVAL s AS STRING, BYVAL n AS INTEGER, BYVAL m AS INTEGER=-1) AS STRING

' (dynamic) memory

PUBLIC DECLARE FUNCTION ALLOCATE   (BYVAL size AS ULONG, BYVAL flags AS ULONG=0) AS VOID PTR
PUBLIC DECLARE SUB      DEALLOCATE (p AS VOID PTR)
PUBLIC DECLARE SUB      _MEMSET    (dst AS BYTE PTR, c AS BYTE, n AS ULONG)
PUBLIC DECLARE FUNCTION FRE        (BYVAL x AS INTEGER) AS ULONG

PUBLIC DECLARE SUB POKE  (BYVAL adr AS ULONG, BYVAL b AS BYTE)
PUBLIC DECLARE SUB POKEW (BYVAL adr AS ULONG, BYVAL w AS UINTEGER)
PUBLIC DECLARE SUB POKEL (BYVAL adr AS ULONG, BYVAL l AS ULONG)

PUBLIC DECLARE FUNCTION PEEK  (BYVAL adr AS ULONG) AS BYTE
PUBLIC DECLARE FUNCTION PEEKW (BYVAL adr AS ULONG) AS INTEGER
PUBLIC DECLARE FUNCTION PEEKL (BYVAL adr AS ULONG) AS ULONG

' random numbers

PUBLIC DECLARE FUNCTION RND       (BYVAL n AS SINGLE = 1) AS SINGLE
PUBLIC DECLARE SUB      RANDOMIZE (BYVAL seed AS SINGLE)

' dynamic array support

PUBLIC TYPE _DARRAY_BOUNDS_T
    PUBLIC:
        AS ULONG lbound, ubound, numElements
END TYPE

PUBLIC TYPE _DARRAY_T

    PUBLIC:
        DECLARE CONSTRUCTOR (BYVAL elementSize AS ULONG)

        DECLARE SUB      REDIM  (BYVAL numDims AS UINTEGER, BYVAL preserve AS BOOLEAN, ...)
        DECLARE FUNCTION IDXPTR (BYVAL dimCnt AS UINTEGER, ...) AS VOID PTR
        DECLARE FUNCTION LBOUND (BYVAL d AS INTEGER) AS INTEGER
        DECLARE FUNCTION UBOUND (BYVAL d AS INTEGER) AS INTEGER
        DECLARE SUB      COPY   (BYREF darray AS _DARRAY_T)
        DECLARE SUB      ERASE  ()
        DECLARE SUB      CLEAR  ()

    PRIVATE:

        AS VOID PTR             data
        AS UINTEGER             numDims
        AS ULONG                elementSize
        AS _DARRAY_BOUNDS_T PTR bounds

END TYPE

' DATA / READ / RESTORE support

PUBLIC DECLARE SUB _aqb_restore (BYVAL p AS VOID PTR)
PUBLIC DECLARE SUB _aqb_read1   (BYVAL v AS VOID PTR)
PUBLIC DECLARE SUB _aqb_read2   (BYVAL v AS VOID PTR)
PUBLIC DECLARE SUB _aqb_read4   (BYVAL v AS VOID PTR)
PUBLIC DECLARE SUB _aqb_readStr (BYVAL v AS VOID PTR)

' misc

PUBLIC DECLARE SUB _aqb_assert (BYVAL b AS BOOLEAN, BYVAL s AS STRING)
PUBLIC DECLARE SUB ERROR (BYVAL i AS INTEGER)
PUBLIC DECLARE SUB RESUME NEXT
PUBLIC DECLARE SUB ON ERROR CALL (BYVAL p AS SUB)
PUBLIC DECLARE SUB ON EXIT CALL  (BYVAL p AS SUB)
PUBLIC DECLARE SUB ON BREAK CALL (BYVAL p AS SUB)

PUBLIC DECLARE SUB SYSTEM

PUBLIC DECLARE FUNCTION TIMER() AS SINGLE

PUBLIC CONST AS VOID PTR NULL = 0

PUBLIC EXTERN g_stdout AS VOID PTR


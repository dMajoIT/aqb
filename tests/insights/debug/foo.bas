OPTION EXPLICIT

DIM AS INTEGER i=42
DIM AS INTEGER a(1)

TRACE "CLEAR ..."

CLEAR ,,1024

TRACE "stack size: ", FRE(-2)
ASSERT FRE(-2)=1024

REM TRACE i, a(0), a(1)
ASSERT i=42
ASSERT a(0)=0
ASSERT a(1)=0

i=23
a(0)=1 : a(1)=2

REM TRACE i, a(0), a(1)
ASSERT i=23
ASSERT a(0)=1
ASSERT a(1)=2

CLEAR

REM TRACE i, a(0), a(1)
ASSERT i=42
ASSERT a(0)=0
ASSERT a(1)=0


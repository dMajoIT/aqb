'
' OOP Test 6
'
' class inheritance
'

OPTION EXPLICIT

CLASS myc1

    field1 AS INTEGER

    DECLARE CONSTRUCTOR (BYVAL initValue AS INTEGER)

    DECLARE SUB store (BYVAL i AS INTEGER)
    DECLARE FUNCTION retrieve () AS INTEGER

    DECLARE FUNCTION square() AS INTEGER

END CLASS

CONSTRUCTOR myc1 (BYVAL initValue AS INTEGER)
    ' TRACE "CONSTRUCTOR myc1 called, initValue="; initValue
    field1 = initValue
END CONSTRUCTOR

'SUB myc1.store (BYVAL i AS INTEGER)
'    field1 = i
'END SUB
'
'FUNCTION myc1.square () AS INTEGER
'    RETURN retrieve()*retrieve()
'END FUNCTION
'
'FUNCTION myc1.retrieve () AS INTEGER
'    ' TRACE "myc1.retrieve: field1=";field1
'    RETURN field1
'END FUNCTION

CLASS myc2 EXTENDS myc1

    field2 AS INTEGER

    DECLARE CONSTRUCTOR (BYVAL iv1 AS INTEGER, BYVAL iv2 AS INTEGER)

    DECLARE SUB store2 (BYVAL i AS INTEGER)
    DECLARE FUNCTION retrieve2 () AS INTEGER

END CLASS

CONSTRUCTOR myc2 (BYVAL iv1 AS INTEGER, BYVAL iv2 AS INTEGER)
    ' TRACE "CONSTRUCTOR myc2 called, iv1="; iv1; ", iv2="; iv2

    field1 = iv1
    field2 = iv2

END CONSTRUCTOR

'SUB myc2.store2 (BYVAL i AS INTEGER)
'    field2 = i
'END SUB
'
'FUNCTION myc2.retrieve2 () AS INTEGER
'    ' TRACE "myc2.retrieve2: field2=";field2
'    RETURN field2
'END FUNCTION

'
' main
'

'TRACE "HUBBA"

DIM o AS myc2 PTR = NEW myc2(23, 42)

'ASSERT o->retrieve()  = 23
'ASSERT o->retrieve2() = 42

'o->store(42)
'ASSERT o->retrieve() = 42
'ASSERT o->retrieve2() = 42
'
'o->store2(23)
'ASSERT o->retrieve() = 42
'ASSERT o->retrieve2() = 23
'ASSERT o->square() = 1764


'
' test proc pointers: function with two args
'

OPTION EXPLICIT

DIM SHARED g_p AS FUNCTION (INTEGER, INTEGER) AS INTEGER

FUNCTION q(x AS INTEGER, y AS INTEGER) AS INTEGER
   q = x*y
END FUNCTION

FUNCTION p(x AS INTEGER, y AS INTEGER) AS INTEGER
   p = x+y
END FUNCTION

SUB setfp (p AS FUNCTION (INTEGER, INTEGER) AS INTEGER)
	g_p = p
END SUB

setfp q
ASSERT g_p (6, 7) = 42

setfp p
ASSERT g_p (16, 7) = 23


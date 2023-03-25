OPTION EXPLICIT

IMPORT OSGadTools

CONST AE_GTG_CREATE   AS INTEGER = 400
CONST AE_GTG_MODIFY   AS INTEGER = 401
CONST AE_GTG_DEPLOY   AS INTEGER = 402
CONST AE_GTG_SELECTED AS INTEGER = 403
CONST AE_GTG_CALLBACK AS INTEGER = 404
CONST AE_GTG_BUFFER   AS INTEGER = 405
CONST AE_GTG_NUM      AS INTEGER = 406

TYPE GTGADGET

    PUBLIC:

        DECLARE CONSTRUCTOR (BYVAL txt AS STRING=NULL, _
                             _COORD2(BYVAL s1 AS BOOLEAN=FALSE, BYVAL x1 AS INTEGER=-1, BYVAL y1 AS INTEGER=-1, BYVAL s2 AS BOOLEAN=FALSE, BYVAL x2 AS INTEGER=-1, BYVAL y2 AS INTEGER=-1), _
                             BYVAL user_data AS VOID PTR=NULL, BYVAL flags AS ULONG=0, BYVAL underscore AS ULONG=95)

        DECLARE PROPERTY x1 AS INTEGER
        DECLARE PROPERTY x1 (BYVAL x AS INTEGER)

        DECLARE PROPERTY y1 AS INTEGER
        DECLARE PROPERTY y1 (BYVAL y AS INTEGER)

        DECLARE PROPERTY x2 AS INTEGER
        DECLARE PROPERTY x2 (BYVAL x AS INTEGER)

        DECLARE PROPERTY y2 AS INTEGER
        DECLARE PROPERTY y2 (BYVAL y AS INTEGER)

        DECLARE PROPERTY text AS STRING
        DECLARE PROPERTY text (BYVAL txt AS STRING)

        DECLARE PROPERTY id AS INTEGER
        DECLARE PROPERTY id (BYVAL id AS INTEGER)

        DECLARE PROPERTY flags AS ULONG
        DECLARE PROPERTY flags (BYVAL f AS ULONG)

        DECLARE PROPERTY deployed AS BOOLEAN

        AS SUB (BYVAL GTGADGET PTR, BYVAL UINTEGER) gadgetup_cb
        AS SUB (BYVAL GTGADGET PTR, BYVAL UINTEGER) gadgetdown_cb
        AS SUB (BYVAL GTGADGET PTR, BYVAL UINTEGER) gadgetmove_cb

        AS VOID PTR user_data
        AS ULONG    underscore

    PRIVATE:

        AS GTGADGET PTR   prev, next

        AS NewGadget      ng

        AS Gadget PTR     gad
        AS Window PTR     win

        AS VOID PTR       deploy_cb


END TYPE

TYPE GTBUTTON EXTENDS GTGADGET

    DECLARE CONSTRUCTOR (BYVAL txt AS STRING=NULL, _
                         _COORD2(BYVAL s1 AS BOOLEAN=FALSE, BYVAL x1 AS INTEGER=-1, BYVAL y1 AS INTEGER=-1, BYVAL s2 AS BOOLEAN=FALSE, BYVAL x2 AS INTEGER=-1, BYVAL y2 AS INTEGER=-1), _
                         BYVAL user_data AS VOID PTR=NULL, BYVAL flags AS ULONG=0, BYVAL underscore AS ULONG=95)

    DECLARE PROPERTY disabled AS BOOLEAN
    DECLARE PROPERTY disabled (BYVAL b AS BOOLEAN)

    PRIVATE:
        AS BOOLEAN  _disabled

END TYPE

TYPE GTCHECKBOX EXTENDS GTGADGET

    DECLARE CONSTRUCTOR (BYVAL txt AS STRING=NULL, _
                         _COORD2(BYVAL s1 AS BOOLEAN=FALSE, BYVAL x1 AS INTEGER=-1, BYVAL y1 AS INTEGER=-1, BYVAL s2 AS BOOLEAN=FALSE, BYVAL x2 AS INTEGER=-1, BYVAL y2 AS INTEGER=-1), _
                         BYVAL user_data AS VOID PTR=NULL, BYVAL flags AS ULONG=0, BYVAL underscore AS ULONG=95)

    DECLARE PROPERTY disabled AS BOOLEAN
    DECLARE PROPERTY disabled (BYVAL b AS BOOLEAN)

    DECLARE PROPERTY checked AS BOOLEAN
    DECLARE PROPERTY checked (BYVAL b AS BOOLEAN)

    PRIVATE:
        AS BOOLEAN  _disabled
        AS BOOLEAN  _checked

END TYPE


' DECLARE FUNCTION GTGSELECTED (BYVAL g AS GTGADGET_t PTR) AS BOOLEAN
' DECLARE FUNCTION GTGBUFFER   (BYVAL g AS GTGADGET_t PTR) AS STRING
' DECLARE FUNCTION GTGNUM      (BYVAL g AS GTGADGET_t PTR) AS LONG

DECLARE SUB      GTGADGETS DEPLOY
DECLARE SUB      GTGADGETS FREE

DECLARE SUB      GTG DRAW BEVEL BOX (_COORD2(BYVAL s1 AS BOOLEAN=FALSE, BYVAL x1 AS INTEGER, BYVAL y1 AS INTEGER, BYVAL s2 AS BOOLEAN=FALSE, BYVAL x2 AS INTEGER, BYVAL y2 AS INTEGER), BYVAL recessed AS BOOLEAN = FALSE)

DECLARE FUNCTION GTGADGETS_NEXT_ID AS INTEGER


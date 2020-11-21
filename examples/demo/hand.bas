SCREEN 1, 320, 200, 2, 1

WINDOW 2,"hand",,15,1

WINDOW OUTPUT 2

PALETTE 0, 1!, 1!, 1!
PALETTE 1, 0,0,0
PALETTE 2,.467,.467,.8
PALETTE 3,.73,.73,.73

OFFSETX% =       70 : OFFSETY% = 15
TYPE%    =        0 : FINISHED%=0

WHILE FINISHED%=0

    READ B1%, B2%

    ' PRINT B1%;",";B2%;",";FINISHED%

    IF B1%=255 AND B2%=255 THEN
        FINISHED% = 1
    ELSEIF B1%=255 THEN
        REM start line
        COLOR B2% : TYPE%=2
    ELSEIF B1%=254 THEN
        REM fill area
        COLOR B2% : TYPE%=1
    ELSEIF TYPE%=0 THEN
        REM draw line
        LINE -(OFFSETX%+B1%,OFFSETY%+B2%)
    ELSEIF TYPE%=1 THEN
        PAINT (OFFSETX%+B1%,OFFSETY%+B2%),,1
        TYPE%=0
    ELSEIF TYPE%=2 THEN
        PSET (OFFSETX%+B1%, OFFSETY%+B2%)
        TYPE%=0
    END IF
WEND

COLOR 1
LOCATE 12,20
PRINT "AQB"

COLOR 1
LOCATE 22,1
PRINT "PRESS ANY KEY"

WHILE INKEY$=""
WEND

WINDOW CLOSE 2
SCREEN CLOSE 1

DATA &HFF, &H01, &H23, &H0B, &H3A, &H0B, &H3A, &H21, &H71, &H21, &H71, &H0B, &H7D, &H0B, &H88, &H16, &H88, &H5E, &H7F, &H5E, &H7F, &H38, &H40, &H38
DATA &H3E, &H36, &H35, &H36, &H34, &H38, &H2D, &H38, &H2D, &H41, &H23, &H48, &H23, &H0B, &HFE, &H02, &H25, &H45, &HFF, &H01, &H21, &H48, &H21, &H0A
DATA &H7E, &H0A, &H8A, &H16, &H8A, &H5F, &H56, &H5F, &H56, &H64, &H52, &H6C, &H4E, &H71, &H4A, &H74, &H44, &H7D, &H3C, &H81, &H3C, &H8C, &H0A, &H8C
DATA &H0A, &H6D, &H09, &H6D, &H09, &H51, &H0D, &H4B, &H14, &H45, &H15, &H41, &H19, &H3A, &H1E, &H37, &H21, &H36, &H21, &H36, &H1E, &H38, &H1A, &H3A
DATA &H16, &H41, &H15, &H45, &H0E, &H4B, &H0A, &H51, &H0A, &H6C, &H0B, &H6D, &H0B, &H8B, &H28, &H8B, &H28, &H76, &H30, &H76, &H34, &H72, &H34, &H5F
DATA &H32, &H5C, &H32, &H52, &H41, &H45, &H41, &H39, &H3E, &H37, &H3B, &H37, &H3E, &H3A, &H3E, &H41, &H3D, &H42, &H36, &H42, &H33, &H3F, &H2A, &H46
DATA &H1E, &H4C, &H12, &H55, &H12, &H54, &H1E, &H4B, &H1A, &H4A, &H17, &H47, &H1A, &H49, &H1E, &H4A, &H21, &H48, &HFF, &H01, &H32, &H3D, &H34, &H36
DATA &H3C, &H37, &H3D, &H3A, &H3D, &H41, &H36, &H41, &H32, &H3D, &HFF, &H01, &H33, &H5C, &H33, &H52, &H42, &H45, &H42, &H39, &H7D, &H39, &H7D, &H5E
DATA &H34, &H5E, &H33, &H5A, &HFF, &H01, &H3C, &H0B, &H6F, &H0B, &H6F, &H20, &H3C, &H20, &H3C, &H0B, &HFF, &H01, &H60, &H0E, &H6B, &H0E, &H6B, &H1C
DATA &H60, &H1C, &H60, &H0E, &HFE, &H03, &H3E, &H1F, &HFF, &H01, &H62, &H0F, &H69, &H0F, &H69, &H1B, &H62, &H1B, &H62, &H0F, &HFE, &H02, &H63, &H1A
DATA &HFF, &H01, &H2F, &H39, &H32, &H39, &H32, &H3B, &H2F, &H3F, &H2F, &H39, &HFF, &H01, &H29, &H8B, &H29, &H77, &H30, &H77, &H35, &H72, &H35, &H69
DATA &H39, &H6B, &H41, &H6B, &H41, &H6D, &H45, &H72, &H49, &H72, &H49, &H74, &H43, &H7D, &H3B, &H80, &H3B, &H8B, &H29, &H8B, &HFF, &H01, &H35, &H5F
DATA &H35, &H64, &H3A, &H61, &H35, &H5F, &HFF, &H01, &H39, &H62, &H35, &H64, &H35, &H5F, &H4A, &H5F, &H40, &H69, &H3F, &H69, &H41, &H67, &H3C, &H62
DATA &H39, &H62, &HFF, &H01, &H4E, &H5F, &H55, &H5F, &H55, &H64, &H51, &H6C, &H4E, &H70, &H49, &H71, &H46, &H71, &H43, &H6D, &H43, &H6A, &H4E, &H5F
DATA &HFF, &H01, &H44, &H6A, &H44, &H6D, &H46, &H70, &H48, &H70, &H4C, &H6F, &H4D, &H6C, &H49, &H69, &H44, &H6A, &HFF, &H01, &H36, &H68, &H3E, &H6A
DATA &H40, &H67, &H3C, &H63, &H39, &H63, &H36, &H65, &H36, &H68, &HFF, &H01, &H7E, &H0B, &H89, &H16, &H89, &H5E, &HFE, &H01, &H22, &H0B, &HFE, &H01
DATA &H3B, &H0B, &HFE, &H01, &H61, &H0F, &HFE, &H01, &H6A, &H1B, &HFE, &H01, &H70, &H0F, &HFE, &H01, &H7E, &H5E, &HFE, &H01, &H4B, &H60, &HFE, &H01
DATA &H2E, &H39, &HFF, &HFF


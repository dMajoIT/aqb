#ifdef __amigaos__

#define INTUI_V36_NAMES_ONLY

#include <stdlib.h>
#include <stdarg.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>

#include <devices/conunit.h>

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

#include <proto/console.h>
#include <pragmas/console_pragmas.h>

#include <clib/alib_protos.h>
#include <clib/asl_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/console_protos.h>

#include <inline/exec.h>
#include <inline/dos.h>
#include <inline/intuition.h>
#include <inline/graphics.h>
#include <inline/asl.h>

#include <libraries/gadtools.h>
#include <clib/gadtools_protos.h>
#include <inline/gadtools.h>

#include <clib/console_protos.h>
#include <inline/console.h>

#include "ui.h"
#include "logger.h"
#include "options.h"
#include "tui.h"
#include "run.h"
#include "amigasupport.h"

//#define LOG_KEY_EVENTS

#define BM_HEIGHT 8

extern struct ExecBase      *SysBase;
extern struct DOSBase       *DOSBase;
extern struct GfxBase       *GfxBase;
extern struct IntuitionBase *IntuitionBase;
extern struct AslBase       *AslBase;
struct Library              *GadToolsBase;
struct Device               *ConsoleDevice = NULL;

uint16_t UI_size_cols=80, UI_size_rows=25;

static struct NewMenu g_newmenu[] =
    {
        { NM_TITLE, (STRPTR) "Project",             0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "New",                 0 , 0, 0, (APTR)KEY_NEW,},
        {  NM_ITEM, (STRPTR) "Open...",   (STRPTR) "O", 0, 0, (APTR)KEY_OPEN,},
        {  NM_ITEM, NM_BARLABEL,                    0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Save",      (STRPTR) "S", 0, 0, (APTR)KEY_SAVE,},
        {  NM_ITEM, (STRPTR) "Save As...",(STRPTR) "A", 0, 0, (APTR)KEY_SAVE_AS,},
        {  NM_ITEM, NM_BARLABEL,                    0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Help...",             0 , 0, 0, (APTR)KEY_HELP,},
        {  NM_ITEM, (STRPTR) "About...",            0 , 0, 0, (APTR)KEY_ABOUT,},
        {  NM_ITEM, NM_BARLABEL,                    0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Quit...",   (STRPTR) "Q", 0, 0, (APTR)KEY_QUIT,},

        { NM_TITLE, (STRPTR) "Edit",                0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Cut",       (STRPTR) "X", 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Copy",      (STRPTR) "C", 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Paste",     (STRPTR) "V", 0, 0, 0,},
        {  NM_ITEM, NM_BARLABEL,                    0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Block",     (STRPTR) "B", 0, 0, (APTR)KEY_BLOCK,},

        { NM_TITLE, (STRPTR) "Find",                0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Find...",   (STRPTR) "F", 0, 0, (APTR)KEY_FIND,},
        {  NM_ITEM, (STRPTR) "Find next", (STRPTR) "N", 0, 0, (APTR)KEY_FIND_NEXT,},

        { NM_TITLE, (STRPTR) "Run",                 0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Compile      F7",     0, 0, 0, (APTR)KEY_F7,},
        {  NM_ITEM, (STRPTR) "Compile+Run  F5",     0, 0, 0, (APTR)KEY_F5,},

        { NM_TITLE, (STRPTR) "Settings",            0 , 0, 0, 0,},
        {  NM_ITEM, (STRPTR) "Colorscheme",         0 , 0, 0, 0,},
        {   NM_SUB, (STRPTR) "Dark",                0 , CHECKIT | MENUTOGGLE,  ~1, (APTR)KEY_COLORSCHEME_0,},
        {   NM_SUB, (STRPTR) "Light",               0 , CHECKIT | MENUTOGGLE,  ~2, (APTR)KEY_COLORSCHEME_1,},
        {  NM_ITEM, (STRPTR) "Font",                0 , 0, 0, 0,},
        {   NM_SUB, (STRPTR) "6",                   0 , CHECKIT | MENUTOGGLE,  ~1, (APTR)KEY_FONT_0,},
        {   NM_SUB, (STRPTR) "8",                   0 , CHECKIT | MENUTOGGLE,  ~2, (APTR)KEY_FONT_1,},
        {   NM_END, NULL, 0 , 0, 0, 0,},
    };

static struct Window     *g_win           = NULL;
static struct RastPort   *g_rp            = NULL;
static struct MsgPort    *g_debugPort     = NULL;
static struct IOStdReq    console_ioreq;
static UWORD              g_OffLeft, g_OffRight, g_OffTop, g_OffBottom;
static APTR               g_vi                = NULL;
static struct Menu       *g_menuStrip         = NULL;
static UWORD              g_fontHeight        = 8;
#include "fonts.h"
static UBYTE              g_curFont           = 1;
static struct BitMap     *g_renderBM;
static UWORD              g_renderBMBytesPerRow;
static UBYTE             *g_renderBMPlanes[3] = {NULL, NULL, NULL};
static UBYTE             *g_renderBMPtr[3];
static bool               g_renderBMPE[3]     = { TRUE, FALSE, FALSE }; // PE: PlaneEnabled
static bool               g_renderBMPEI[3]    = {FALSE, FALSE, FALSE }; // PE: PlaneEnabledInverse
static uint16_t           g_renderBMcurCol    = 0;
static uint16_t           g_renderBMcurRow    = 1;
static uint16_t           g_curLineStart      = 1;
static uint16_t           g_curLineCols       = 80;
static uint16_t           g_visWidth;
static uint16_t           g_renderBMmaxCols   = 80;
// RTG graphics library based rendering
#define BUFSIZE 1024
static UI_size_cb         g_size_cb           = NULL;
static void              *g_size_cb_user_data = NULL;
static UI_event_cb        g_event_cb            = NULL;
static void              *g_event_cb_user_data  = NULL;
static uint16_t           g_scrollStart       = 1;
static uint16_t           g_scrollEnd         = 10;
static bool               g_cursorVisible     = FALSE;
static uint16_t           g_cursorRow         = 1;
static uint16_t           g_cursorCol         = 1;
static uint16_t           g_theme             = 0;

typedef struct
{
    char   *name;
    uint8_t fg[5];
    uint8_t bg[5];
} UI_theme_t;

#define NUM_THEMES 2

static UI_theme_t g_themes[NUM_THEMES] = {
    {
        "Dark",
        //  TEXT KEYWORD COMMENT INVERSE DIALOG
        {      2,      3,      0,      1,     1 },
        {      1,      1,      1,      0,     3 }
    },
    {
        "Light",
        //  TEXT KEYWORD COMMENT INVERSE DIALOG
        {      1,      2,      3,      0,     0 },
        {      0,      0,      0,      1,     3 }
    },
};


static void cleanexit (char *s, uint32_t n)
{
    if (s)
    {
        printf("%s\n", s);
        U_request(NULL, NULL, "OK", "%s", s);
    }
    exit(n);
}

static void drawCursor(void)
{
    uint16_t x = (g_cursorCol-1)*8 + g_OffLeft;
    uint16_t y = (g_cursorRow-1)*g_fontHeight + g_OffTop;
    //LOG_printf (LOG_DEBUG, "ui_amiga: drawCursor g_cursorCol=%d, g_cursorRow=%d, x=%d, y=%d\n", g_cursorCol, g_cursorRow, x, y);
    BYTE DrawMode = g_rp->DrawMode;
    SetDrMd (g_rp, COMPLEMENT);
    g_rp->Mask = 3;
    RectFill (g_rp, x, y, x+7, y+g_fontHeight-1);
    SetDrMd (g_rp, DrawMode);
}

static void setTextColor (uint8_t fg, uint8_t bg)
{
    switch (fg)
    {
        case 0: g_renderBMPE[0] = FALSE; g_renderBMPE[1] = FALSE; g_renderBMPE[2] = FALSE; break;
        case 1: g_renderBMPE[0] =  TRUE; g_renderBMPE[1] = FALSE; g_renderBMPE[2] = FALSE; break;
        case 2: g_renderBMPE[0] = FALSE; g_renderBMPE[1] =  TRUE; g_renderBMPE[2] = FALSE; break;
        case 3: g_renderBMPE[0] =  TRUE; g_renderBMPE[1] =  TRUE; g_renderBMPE[2] = FALSE; break;
        case 4: g_renderBMPE[0] = FALSE; g_renderBMPE[1] = FALSE; g_renderBMPE[2] =  TRUE; break;
        case 5: g_renderBMPE[0] =  TRUE; g_renderBMPE[1] = FALSE; g_renderBMPE[2] =  TRUE; break;
        case 6: g_renderBMPE[0] = FALSE; g_renderBMPE[1] =  TRUE; g_renderBMPE[2] =  TRUE; break;
        case 7: g_renderBMPE[0] =  TRUE; g_renderBMPE[1] =  TRUE; g_renderBMPE[2] =  TRUE; break;
        default: assert(FALSE);
    }

    switch (bg)
    {
        case 0: g_renderBMPEI[0] = FALSE; g_renderBMPEI[1] = FALSE; g_renderBMPEI[2] = FALSE; break;
        case 1: g_renderBMPEI[0] =  TRUE; g_renderBMPEI[1] = FALSE; g_renderBMPEI[2] = FALSE; break;
        case 2: g_renderBMPEI[0] = FALSE; g_renderBMPEI[1] =  TRUE; g_renderBMPEI[2] = FALSE; break;
        case 3: g_renderBMPEI[0] =  TRUE; g_renderBMPEI[1] =  TRUE; g_renderBMPEI[2] = FALSE; break;
        case 4: g_renderBMPEI[0] = FALSE; g_renderBMPEI[1] = FALSE; g_renderBMPEI[2] =  TRUE; break;
        case 5: g_renderBMPEI[0] =  TRUE; g_renderBMPEI[1] = FALSE; g_renderBMPEI[2] =  TRUE; break;
        case 6: g_renderBMPEI[0] = FALSE; g_renderBMPEI[1] =  TRUE; g_renderBMPEI[2] =  TRUE; break;
        case 7: g_renderBMPEI[0] =  TRUE; g_renderBMPEI[1] =  TRUE; g_renderBMPEI[2] =  TRUE; break;
        default: assert(FALSE);
    }
}

void UI_setTextStyle (uint16_t style)
{
    setTextColor (g_themes[g_theme].fg[style], g_themes[g_theme].bg[style]);
}

void UI_beginLine (uint16_t row, uint16_t col_start, uint16_t cols)
{
    // LOG_printf (LOG_DEBUG,
    //             "ui_amiga: beginLine row=%d, col_start=%d, cols=%d\n",
    //             row, col_start, cols);
    g_curLineStart = col_start;
    g_curLineCols  = cols;
    //LOG_printf (LOG_DEBUG, "ui_amiga: beginLine g_curLineCols=%d\n", g_curLineCols);
    g_renderBMcurCol = col_start-1;
    g_renderBMcurRow = row;
    for (uint8_t d = 0; d<g_renderBM->Depth; d++)
        g_renderBMPtr[d] = g_renderBMPlanes[d];
    for (uint16_t r = 0; r<g_fontHeight; r++)
    {
        for (uint16_t d = 0; d<g_renderBM->Depth; d++)
            memset (g_renderBMPtr[d] + r*g_renderBMBytesPerRow, g_renderBMPEI[d] ? 0xff : 0x00, g_curLineCols);
    }
}

#define CSI_BUF_LEN 16

void UI_putc(char c)
{
    static BOOL bCSI = FALSE;
    static char csiBuf[CSI_BUF_LEN];
    static uint16_t csiBufLen=0;
    UBYTE uc = (UBYTE) c;
    //printf ("UI_putc: %c[%d]\n", c, uc);
    if (!bCSI)
    {
        if (uc==0x9b)
        {
            bCSI = TRUE;
            return;
        }
    }
    else
    {
        /*
        0x30–0x3F (ASCII 0–9:;<=>?)                  parameter bytes
        0x20–0x2F (ASCII space and !\"#$%&'()*+,-./) intermediate bytes
        0x40–0x7E (ASCII @A–Z[\]^_`a–z{|}~)          final byte
        */
        if (uc>=0x40)
        {
            //LOG_printf (LOG_DEBUG, "!CSI seq detected: %s%c\n", csiBuf, c);
            //printf ("CSI seq detected: %s%c csiBufLen=%d\n", csiBuf, c, csiBufLen);

            switch (c)
            {
                case 'p': // csr on/off
                    if (csiBufLen == 2)
                    {
                        switch (csiBuf[0])
                        {
                            case '0':
                               UI_setCursorVisible (FALSE);
                               break;
                            case '1':
                               UI_setCursorVisible (TRUE);
                               break;
                        }
                    }
                    break;
                case 'm': // presentation
                    if (csiBufLen == 1)
                    {
                        switch (csiBuf[0])
                        {
                            case '0':
                               setTextColor (g_themes[g_theme].fg[0], g_themes[g_theme].bg[0]);
                               break;
                        }
                    }
                    else
                    {
                        if (csiBufLen == 2)
                        {
                            uint8_t color = (csiBuf[0]-'0')*10+(csiBuf[1]-'0');
                            //printf ("setting color %d\n", color);
                            switch (color)
                            {
                                case 30: setTextColor (0, g_themes[g_theme].bg[0]); break;
                                case 31: setTextColor (1, g_themes[g_theme].bg[0]); break;
                                case 32: setTextColor (2, g_themes[g_theme].bg[0]); break;
                                case 33: setTextColor (3, g_themes[g_theme].bg[0]); break;
                                case 34: setTextColor (0, g_themes[g_theme].bg[0]); break;
                                case 35: setTextColor (1, g_themes[g_theme].bg[0]); break;
                                case 36: setTextColor (2, g_themes[g_theme].bg[0]); break;
                                case 37: setTextColor (3, g_themes[g_theme].bg[0]); break;
                            }
                        }
                    }
                    break;
            }

            bCSI = FALSE;
            csiBufLen = 0;
        }
        else
        {
            if (csiBufLen<CSI_BUF_LEN)
                csiBuf[csiBufLen++] = c;
        }
        return;
    }

    if (g_renderBMcurCol >= g_renderBMmaxCols)
        return;
    g_renderBMcurCol++;

    if (g_cursorVisible)
        drawCursor();
    //printf ("painting char %d (%c)\n", c, c);

    UBYTE ci = c;
    UBYTE *dst[3];
    for (uint8_t planeNum = 0; planeNum < g_renderBM->Depth; planeNum++)
        dst[planeNum] = g_renderBMPtr[planeNum];

    //printf ("ci=%d (%c) bl=%d byl=%d bs=%d\n", ci, ci, bl, byl, bs);
    for (UBYTE y=0; y<g_fontHeight; y++)
    {
        UBYTE fd = g_fontData[g_curFont][ci][y];
        for (uint8_t planeNum = 0; planeNum < g_renderBM->Depth; planeNum++)
        {
            *dst[planeNum]  = g_renderBMPE[planeNum] ? fd : 0;
            if (g_renderBMPEI[planeNum])
                *dst[planeNum] |= ~fd;
            dst[planeNum] += g_renderBMBytesPerRow;
        }
    }
    for (uint8_t planeNum = 0; planeNum < g_renderBM->Depth; planeNum++)
        g_renderBMPtr[planeNum]++;

    if (g_cursorVisible)
        drawCursor();
}

void UI_putstr(char *s)
{
    while (*s)
        UI_putc (*s++);
}

void UI_printf (char* format, ...)
{
    va_list args;
    va_start(args, format);
    UI_vprintf (format, args);
    va_end(args);
}

void UI_vprintf (char* format, va_list args)
{
    static char buf[BUFSIZE];
    vsnprintf (buf, BUFSIZE, format, args);
    UI_putstr(buf);
}

void UI_endLine (void)
{
    if (g_cursorVisible)
        drawCursor();
    BltBitMapRastPort (g_renderBM, 0, 0, g_rp, (g_curLineStart-1)*8+g_OffLeft, (g_renderBMcurRow-1)*g_fontHeight+g_OffTop, g_curLineCols*8, g_fontHeight, 0xc0);
    if (g_cursorVisible)
        drawCursor();
}

void UI_setCursorVisible (bool visible)
{
    if ((visible && !g_cursorVisible) || (!visible && g_cursorVisible))
        drawCursor();

    g_cursorVisible = visible;
}

void UI_moveCursor (uint16_t row, uint16_t col)
{
    if (g_cursorVisible)
        drawCursor();
    g_cursorRow = row;
    g_cursorCol = col;
    if (g_cursorVisible)
        drawCursor();
}

struct MsgPort *UI_debugPort(void)
{
    return g_debugPort;
}

void UI_bell (void)
{
    DisplayBeep (NULL);
}

void UI_eraseDisplay (void)
{
    SetAPen(g_rp, g_themes[g_theme].bg[0]);
    SetBPen(g_rp, g_themes[g_theme].bg[0]);
    SetDrMd(g_rp, JAM1);
    RectFill (g_rp, g_OffLeft, g_OffTop, g_win->Width-g_OffRight-1, g_win->Height-g_OffBottom-1);
}

void UI_setColorScheme (int theme)
{
    //LOG_printf (LOG_DEBUG, "UI_setColorScheme(%d)\n", theme);
    OPT_prefSetInt (OPT_PREF_COLORSCHEME, theme);
    g_theme = theme;
}

void UI_setScrollArea (uint16_t row_start, uint16_t row_end)
{
    g_scrollStart = row_start;
    g_scrollEnd   = row_end;
}

void UI_scrollUp (bool fullscreen)
{
    if (g_cursorVisible)
        drawCursor();
    WORD min_x = g_OffLeft;
    WORD min_y = g_OffTop + (g_scrollStart-1)*g_fontHeight;
    WORD max_x = g_win->Width - g_OffRight-1;
    WORD max_y = fullscreen ? g_win->Height-g_OffBottom-1 : g_OffTop + g_scrollEnd*g_fontHeight-1;
#if 0
    printf ("ScrollRaster g_win: (%d/%d)-(%d/%d)\n", g_win->LeftEdge, g_win->TopEdge, g_win->Width, g_win->Height);
    printf ("ScrollRaster (%d/%d)-(%d/%d) g_scrollStart=%d, g_scrollEnd=%d\n", min_x, min_y, max_x, max_y, g_scrollStart, g_scrollEnd);
    if (fullscreen)
    {
        BYTE FgPen = g_rp->FgPen;
        BYTE DrawMode = g_rp->DrawMode;
        //struct RastPort *rp = &g_screen->RastPort;
        struct RastPort *rp = g_rp;
        SetAPen(rp, 2);
        SetDrMd(rp, JAM1);
        //RectFill (g_rp, min_x, min_y, max_x, max_y);
        Move(rp, min_x, min_y); Draw(rp, max_x, max_y);
        //Move(rp, 0, 0); Draw(rp, 639, 511-2);
        //Move(rp, 0, 0); Draw(rp, 639, 511-4);
        SetAPen(rp, FgPen);
        SetDrMd(rp, DrawMode);

        //UBYTE *p = g_screen->BitMap.Planes[1] + 507*g_renderBMBytesPerRow;
        //for (uint16_t i = 0; i<80; i++)
        //    *p++ = 0xff;

        //ScrollRaster(g_rp, 0, g_fontHeight, min_x, min_y, max_x, max_y);
    }
    else
    {
        ScrollRaster(g_rp, 0, g_fontHeight, min_x, min_y, max_x, max_y);
    }
#else
    ScrollRaster(g_rp, 0, g_fontHeight, min_x, min_y, max_x, max_y);
#endif
    if (g_cursorVisible)
        drawCursor();
}

void UI_scrollDown (void)
{
    if (g_cursorVisible)
        drawCursor();
    WORD max_x = g_win->Width - g_OffRight-1;
    ScrollRaster(g_rp, 0, -g_fontHeight, g_OffLeft, g_OffTop + (g_scrollStart-1)*g_fontHeight, max_x, (g_scrollEnd-1)*g_fontHeight-1);
    if (g_cursorVisible)
        drawCursor();
}

void UI_onSizeChangeCall (UI_size_cb cb, void *user_data)
{
    g_size_cb = cb;
    g_size_cb_user_data = user_data;
}

void UI_onEventCall (UI_event_cb cb, void *user_data)
{
    g_event_cb           = cb;
    g_event_cb_user_data = user_data;
}

uint16_t UI_EZRequest (char *body, char *gadgets, ...)
{
    char *posTxt=NULL;
    char *negTxt=NULL;

    static char buf[256];
    strncpy (buf, gadgets, 256);
    char *s = buf;
    char *c = buf;
    while (*c)
    {
        if (*c=='|')
        {
            *c = 0;
            if (negTxt)
                posTxt = negTxt;
            negTxt = s;
            c++;
            s=c;
        }
        else
        {
            c++;
        }
    }
    *c = 0;
    if (negTxt)
        posTxt = negTxt;
    negTxt = s;

    va_list args;
    va_start(args, gadgets);
    static char buf2[1024];
    vsnprintf (buf2, 1024, body, args);
    va_end(args);
    bool b = U_request (g_win, posTxt, negTxt, "%s", buf2);

    return b ? 1 : 0;
}

char *UI_FileReq (char *title)
{
	static char pathbuf[1024];

	struct FileRequester *fr;

	if (fr = (struct FileRequester *) AllocAslRequestTags(ASL_FileRequest,
			                                              ASL_Hail,      (ULONG)title,
			                                              ASL_Dir,       (ULONG)aqb_home,
			                                              ASL_File,      (ULONG)"",
														  ASL_Pattern,   (ULONG)"#?.bas",
														  ASL_FuncFlags, FILF_PATGAD,
			                                              ASL_Window,    (ULONG) g_win,
			                                              TAG_DONE))
	{
		if (AslRequest(fr, 0L))
		{
			//printf("PATH=%s FILE=%s\n", fr->rf_Dir, fr->rf_File);
			strncpy (pathbuf, (char*)fr->rf_Dir, 1024);
			AddPart ((STRPTR) pathbuf, fr->rf_File, 1024);
			//printf(" -> %s\n", pathbuf);

			return String (UP_ide, pathbuf);
		}
		FreeAslRequest(fr);
	}

	return NULL;
}

bool UI_FindReq (char *buf, uint16_t buf_len, bool *matchCase, bool *wholeWord, bool *searchBackwards)
{
    return TUI_FindReq (buf, buf_len, matchCase, wholeWord, searchBackwards);
}

void UI_HelpBrowser (void)
{
    TUI_HelpBrowser();
}
typedef enum { esWait, esGetWin, esGetDebug } eventState;

static void updateWindowSize(void)
{
    g_OffLeft   = g_win->BorderLeft;
    g_OffRight  = g_win->BorderRight;
    g_OffTop    = g_win->BorderTop;
    g_OffBottom = g_win->BorderBottom;
    g_renderBMmaxCols = (g_win->Width - g_OffLeft - g_OffRight) /8;
    UI_size_cols = (g_win->Width  - g_OffLeft - g_OffRight) / 8;
    UI_size_rows = (g_win->Height - g_OffTop  - g_OffBottom) / g_fontHeight;
    // printf ("updateWindowSize: g_OffLeft=%d, g_OffRight=%d, g_OffTop=%d, g_OffBottom=%d, g_renderBMmaxCols=%d\n",
    //         g_OffLeft, g_OffRight, g_OffTop, g_OffBottom, g_renderBMmaxCols);
}

static uint16_t nextEvent(void)
{
    static eventState state = esWait;

    ULONG windowsig = 1 << g_win->UserPort->mp_SigBit;
	ULONG debugsig  = 1 << g_debugPort->mp_SigBit;
    uint16_t res = KEY_NONE;
    static ULONG signals;

    switch (state)
    {
        case esWait:
            //LOG_printf (LOG_DEBUG, "ui_amiga: nextEvent(): Wait...\n");
            signals = Wait(windowsig|debugsig);
            state = esGetWin;
            /* fall trough */
        case esGetWin:
            if (signals & windowsig)
            {
                //LOG_printf (LOG_DEBUG, "ui_amiga: nextEvent(): GetMsg...\n");
                struct IntuiMessage *winmsg;
                winmsg = (struct IntuiMessage *)GetMsg(g_win->UserPort);
                if (winmsg)
                {
                    //LOG_printf (LOG_DEBUG, "ui_amiga: nextEvent(): got a message, class=%d\n", winmsg->Class);
                    switch (winmsg->Class)
				    {
                        case IDCMP_CLOSEWINDOW:
                            res = KEY_QUIT;
				    		break;

                        case IDCMP_REFRESHWINDOW:
                            res = KEY_REFRESH;
				    		break;

                        case IDCMP_NEWSIZE:
                            updateWindowSize();
                            if (g_size_cb)
                                g_size_cb (g_size_cb_user_data);
                            break;

				    	case IDCMP_MENUPICK:
				    	{
				    		UWORD menuNumber = winmsg->Code;
                            //LOG_printf (LOG_DEBUG, "ui_amiga: menu picked, menuNumber=%d\n", menuNumber);
				    		while ((menuNumber != MENUNULL))
				    		{
				    			struct MenuItem *item = ItemAddress(g_menuStrip, menuNumber);

				    			UWORD menuNum = MENUNUM(menuNumber);
				    			UWORD itemNum = ITEMNUM(menuNumber);
				    			UWORD subNum  = SUBNUM(menuNumber);
                                uint32_t k = (uint32_t) GTMENUITEM_USERDATA(item);
				    			LOG_printf (LOG_DEBUG, "ui_amiga: menu picked menuNum=%d, itemNum=%d, subNum=%d, userData=%d\n", menuNum, itemNum, subNum, k);
                                if (k)
                                    res = (uint16_t)k;

				    			menuNumber = item->NextSelect;
				    		}
				    		break;
				    	}

				    	case IDCMP_RAWKEY:
                        {
                            //LOG_printf (LOG_DEBUG, "RAWKEY EVENT: code=%d, qualifier=%d\n", winmsg->Code, winmsg->Qualifier);

                            static struct InputEvent ievent;
                            static UBYTE kbuffer[16];
                            ievent.ie_Class            = IECLASS_RAWKEY;
                            ievent.ie_Code             = winmsg->Code;
                            ievent.ie_Qualifier        = winmsg->Qualifier;
                            ievent.ie_position.ie_addr = *((APTR*)winmsg->IAddress);

                            USHORT nc = RawKeyConvert(&ievent, kbuffer, 15, /*kmap=*/NULL);
#ifdef LOG_KEY_EVENTS
                            LOG_printf (LOG_DEBUG, " -> RawKeyConvert nc=%d, buf=\n", nc);
                            for (int i=0; i<nc; i++)
                            {
                                LOG_printf (LOG_DEBUG, " 0x%02x[%c]", kbuffer[i], kbuffer[i]);
                            }
                            LOG_printf(LOG_DEBUG, "\n");
#endif
                            switch (nc)
                            {
                                case 1:
                                    switch (kbuffer[0])
                                    {
                                        case 0x14: res = KEY_GOTO_BOF; break;
                                        case 0x02: res = KEY_GOTO_EOF; break;
                                        default: res = (UWORD) kbuffer[0];
                                    }
                                    break;
                                case 2:
                                    switch (kbuffer[0])
                                    {
                                        case 0x9b:
                                            switch (kbuffer[1])
                                            {
                                                case 0x41: if (winmsg->Qualifier & 8) res = KEY_GOTO_BOF ; else res = KEY_CURSOR_UP  ; break;
                                                case 0x42: if (winmsg->Qualifier & 8) res = KEY_GOTO_EOF ; else res = KEY_CURSOR_DOWN; break;
                                                case 0x44: res = KEY_CURSOR_LEFT ; break;
                                                case 0x43: res = KEY_CURSOR_RIGHT; break;
                                                case 0x54: res = KEY_PAGE_UP     ; break;
                                                case 0x53: res = KEY_PAGE_DOWN   ; break;
                                            }
                                    }
                                    break;
                                case 3:
                                    switch (kbuffer[0])
                                    {
                                        case 0x9b:
                                            if (kbuffer[2]==0x7e)
                                            {
                                                switch (kbuffer[1])
                                                {
                                                    case 0x30: res = KEY_F1 ; break;
                                                    case 0x31: res = KEY_F2 ; break;
                                                    case 0x32: res = KEY_F3 ; break;
                                                    case 0x33: res = KEY_F4 ; break;
                                                    case 0x34: res = KEY_F5 ; break;
                                                    case 0x35: res = KEY_F6 ; break;
                                                    case 0x36: res = KEY_F7 ; break;
                                                    case 0x37: res = KEY_F8 ; break;
                                                    case 0x38: res = KEY_F9 ; break;
                                                    case 0x39: res = KEY_F10; break;
                                                }
                                            }
                                            else
                                            {
                                                if (kbuffer[1]==0x20)
                                                {
                                                    switch (kbuffer[2])
                                                    {
                                                        case 0x41: res = KEY_HOME; break;
                                                        case 0x40: res = KEY_END ; break;
                                                    }
                                                }
                                            }
                                    }
                                    break;
                            }
                            break;
                        }
                    }
                    ReplyMsg((struct Message *)winmsg);
                    break;
                }
                else
                {
                    //LOG_printf (LOG_DEBUG, "ui_amiga: nextEvent(): got no message\n");
                    state = esGetDebug;
                }
            }
            else
            {
                    state = esGetDebug;
            }
            /* fall trough */
        case esGetDebug:
            if (signals & debugsig)
            {
                RUN_handleMessages();
                res = KEY_REFRESH;
            }
            state = esWait;
            break;
    } /* switch (state) */

    //LOG_printf (LOG_DEBUG, "ui_amiga: nextEvent(): done, res=%d\n", res);
    return res;
}

uint16_t UI_waitkey (void)
{
    while (TRUE)
    {
        uint16_t k = nextEvent();
        if (k!=KEY_NONE)
            return k;
    }
}

void UI_deinit(void)
{
    if (g_renderBM)
    {
        for (uint8_t planeNum = 0; planeNum < g_renderBM->Depth; planeNum++)
        {
            if (g_renderBM->Planes[planeNum])
                FreeRaster(g_renderBM->Planes[planeNum], g_visWidth, BM_HEIGHT);
        }
        FreeMem(g_renderBM, sizeof(struct BitMap));
    }

    if (g_win)
	{
		ClearMenuStrip(g_win);
		if (g_menuStrip)
			FreeMenus (g_menuStrip);
        CloseWindow(g_win);
	}
	if (g_vi)
		FreeVisualInfo(g_vi);
    if (g_debugPort)
        ASUP_delete_port(g_debugPort);
    if (ConsoleDevice)
        CloseDevice((struct IORequest *)&console_ioreq);
}

void UI_setFont (int font)
{
    OPT_prefSetInt (OPT_PREF_FONT, font);
    g_curFont = font;
    g_fontHeight = font ? 8 : 6;

    updateWindowSize();
    UI_eraseDisplay();
}

bool UI_init (void)
{
    SysBase = *(APTR *)4L;

    // check library versions

    if ( ((struct Library *)IntuitionBase)->lib_Version < 37)
         cleanexit("intuition library is too old, need at least V37", RETURN_FAIL);
    if ( ((struct Library *)GfxBase)->lib_Version < 37)
         cleanexit("graphics library is too old, need at least V37", RETURN_FAIL);
    if (OpenDevice((STRPTR)"console.device", -1, (struct IORequest *)&console_ioreq,0))
         cleanexit("Can't open console.device", RETURN_FAIL);
    ConsoleDevice = (struct Device *)console_ioreq.io_Device;

    struct Screen *sc = LockPubScreen (NULL); // default public screen
    if (!sc)
         cleanexit("Failed to lock default public screen", RETURN_FAIL);

	// determine screen size
	ULONG mid = GetVPModeID(&sc->ViewPort);

	struct DimensionInfo di;
	if (!GetDisplayInfoData(NULL, (APTR)&di, sizeof(di), DTAG_DIMS, mid))
		 cleanexit("Failed to retrieve display info data", RETURN_FAIL);

	//printf ("ui: workbench screen size is %d x %d (TxtOScan: (%d / %d) - (%d / %d)\n", (int)sc->Width, (int)sc->Height,
	//        (int)di.TxtOScan.MinX, (int)di.TxtOScan.MinY, (int)di.TxtOScan.MaxX, (int)di.TxtOScan.MaxY);

	WORD maxW = di.TxtOScan.MaxX - di.TxtOScan.MinX + 1;
	WORD maxH = di.TxtOScan.MaxY - di.TxtOScan.MinY + 1;

	g_visWidth = sc->Width > maxW ? maxW : sc->Width;
	WORD visHeight = sc->Height > maxH ? maxH : sc->Height;

    static char winTitle[128];
    snprintf (winTitle, 128, "%s %s", PROGRAM_NAME_LONG, VERSION);

    // open a full screen window
    if (!(g_win = OpenWindowTags(NULL,
                                 WA_Top,           sc->BarHeight+1,
                                 WA_Width,         g_visWidth,
                                 WA_Height,        visHeight-sc->BarHeight-1,
                                 WA_IDCMP,         IDCMP_MENUPICK | IDCMP_RAWKEY | IDCMP_REFRESHWINDOW | IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE,
                                 WA_Backdrop,      FALSE,
                                 WA_SimpleRefresh, TRUE,
                                 WA_Activate,      TRUE,
                                 WA_Borderless,    FALSE,
                                 WA_SizeGadget,    TRUE,
                                 WA_DragBar,       TRUE,
                                 WA_DepthGadget,   TRUE,
                                 WA_CloseGadget,   TRUE,
                                 WA_Title,         (LONG)winTitle,
                                 WA_MinWidth,      240,
                                 WA_MinHeight,     100,
                                 WA_MaxWidth,      g_visWidth,
                                 WA_MaxHeight,     visHeight,
                                 WA_NewLookMenus,  TRUE)))
         cleanexit("Can't open window", RETURN_FAIL);


    UI_setColorScheme(OPT_prefGetInt (OPT_PREF_COLORSCHEME));
    UI_setTextStyle (UI_TEXT_STYLE_TEXT);

    updateWindowSize();

    UnlockPubScreen(NULL, sc);

    g_rp = g_win->RPort;

    UI_setFont(OPT_prefGetInt (OPT_PREF_FONT));

    g_renderBM = AllocMem(sizeof(struct BitMap), MEMF_PUBLIC | MEMF_CLEAR);
    if (!g_renderBM)
         cleanexit("Failed to allocate render BitMap struct", RETURN_FAIL);

    uint8_t depth = sc->BitMap.Depth > 3 ? 3 : sc->BitMap.Depth;

    InitBitMap(g_renderBM, depth, g_visWidth, BM_HEIGHT);

    for (uint8_t planeNum = 0; planeNum < depth; planeNum++)
    {
        g_renderBMPtr[planeNum] = g_renderBM->Planes[planeNum] = g_renderBMPlanes[planeNum] = AllocRaster(g_visWidth, BM_HEIGHT);
        if (!g_renderBM->Planes[planeNum])
            cleanexit ("Failed to allocate render BitMap plane", RETURN_FAIL);
    }

    g_renderBMBytesPerRow = g_renderBM->BytesPerRow;

    UI_beginLine (1, 1, UI_size_cols);

	/*
     * menu
     */

    if (!(g_vi = GetVisualInfo(g_win->WScreen, TAG_END)))
		cleanexit ("failed to get screen visual info", RETURN_FAIL);
	if (!(g_menuStrip = CreateMenus(g_newmenu, TAG_END)))
		cleanexit("failed to create menu", RETURN_FAIL);
	if (!LayoutMenus(g_menuStrip, g_vi, GTMN_NewLookMenus, TRUE, TAG_END))
		cleanexit("failed to layout menu", RETURN_FAIL);
	if (!SetMenuStrip(g_win, g_menuStrip))
		cleanexit("failed to set menu strip", RETURN_FAIL);

    struct MenuItem *item = ItemAddress(g_menuStrip, FULLMENUNUM(/*menu=*/4, /*item=*/0, /*sub=*/OPT_prefGetInt (OPT_PREF_COLORSCHEME)));
    item->Flags |= CHECKED;
    item = ItemAddress(g_menuStrip, FULLMENUNUM(/*menu=*/4, /*item=*/1, /*sub=*/OPT_prefGetInt (OPT_PREF_FONT)));
    item->Flags |= CHECKED;

    /*
     * debug port
     */

    g_debugPort  = ASUP_create_port ((STRPTR) "AQB debug reply port", 0);
    if (!g_debugPort)
		cleanexit("failed to create debug port", RETURN_FAIL);

	return TRUE;
}


static inline void report_key (uint16_t key)
{
    if (key == KEY_NONE)
        return;
    if (g_event_cb)
        g_event_cb (key, g_event_cb_user_data);
}

void UI_run (void)
{
    BOOL running = TRUE;
    while (running)
    {
        uint16_t key = nextEvent();
        if (key == KEY_QUIT)
            running = FALSE;
        else
            report_key (key);
	}
}

#endif


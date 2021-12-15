
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <libgen.h>

#include <exec/types.h>
#include <exec/memory.h>

#include <libraries/diskfont.h>

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include <inline/exec.h>
#include <inline/dos.h>
#include <inline/graphics.h>
#include <inline/intuition.h>

extern struct ExecBase      *SysBase;
extern struct DOSBase       *DOSBase;
extern struct GfxBase       *GfxBase;
extern struct IntuitionBase *IntuitionBase;

#define FONT_DIR  "SYS:x/Fonts"
#define FONT_NAME "Dungeon"
#define FONT_SIZE "8"

static struct DiskFontHeader *_loadFont (char *font_dir, char *font_name, char *font_size)
{
    static char fontPath[256];

    strncpy (fontPath, font_dir, 256);
    AddPart ((STRPTR) fontPath, (STRPTR)font_name, 256);
    AddPart ((STRPTR) fontPath, (STRPTR)font_size, 256);

    printf ("fontPath: %s\n", fontPath);

    BPTR seglist = LoadSeg ((STRPTR)fontPath);

    if (seglist)
    {
        printf ("LoadSeg worked: 0x%08lx\n", seglist);

		struct DiskFontHeader *dfh;

		dfh = (struct DiskFontHeader *) (BADDR(seglist) + 8);
        dfh->dfh_Segment = seglist;

		printf ("dfh->dfh_Name: %s, revision: %d\n", dfh->dfh_Name, dfh->dfh_Revision);

		struct TextFont *tf = &dfh->dfh_TF;
		printf ("tf->tf_YSize    =%d\n", tf->tf_YSize    );
		printf ("tf->tf_Style    =%d\n", tf->tf_Style    );
		printf ("tf->tf_Flags    =%d\n", tf->tf_Flags    );
		printf ("tf->tf_XSize    =%d\n", tf->tf_XSize    );
		printf ("tf->tf_Baseline =%d\n", tf->tf_Baseline );
		printf ("tf->tf_BoldSmear=%d\n", tf->tf_BoldSmear);

        return dfh;
    }
    else
    {
        printf ("LoadSeg failed!\n");
    }

    return NULL;
}

static void _freeFont (struct DiskFontHeader *dfh)
{
    BPTR seglist = dfh->dfh_Segment;
    if (!seglist)
        return;
    dfh->dfh_Segment = 0l;
    UnLoadSeg (seglist);
}

typedef UBYTE fontData_t[256][8];

static void _fontConv(struct TextFont *font, fontData_t *fontData)
{
    for (UWORD ci=0; ci<256; ci++)
        for (UBYTE y=0; y<8; y++)
            *fontData[ci][y] = 0;

    UWORD *pCharLoc = font->tf_CharLoc;
#ifdef DEBUG_FONTCONV
    uint16_t cnt=0;
#endif
    for (UBYTE ci=font->tf_LoChar; ci<font->tf_HiChar; ci++)
    {
        UWORD bl = *pCharLoc;
        UWORD byl = bl / 8;
        BYTE bitl = bl % 8;
        pCharLoc++;
        UWORD bs = *pCharLoc;
        pCharLoc++;
#ifdef DEBUG_FONTCONV
        if (cnt<DEBUG_FONTCONV_NUM)
            printf ("ci=%d(%c) bl=%d -> byl=%d/bitl=%d, bs=%d\n", ci, ci, bl, byl, bitl, bs);
#endif
        if (bs>8)
            bs = 8;
        for (UBYTE y=0; y<font->tf_YSize; y++)
        {
            char *p = font->tf_CharData;
            p += y*font->tf_Modulo + byl;
            BYTE bsc = bs;
            BYTE bitlc = 7-bitl;
            for (BYTE x=7; x>=0; x--)
            {
                if (*p & (1<<bitlc))
                {
                    *fontData[ci][y] |= (1<<x);
#ifdef DEBUG_FONTCONV
                    if (cnt<DEBUG_FONTCONV_NUM)
                        printf("*");
#endif
                }
                else
                {
#ifdef DEBUG_FONTCONV
                    if (cnt<DEBUG_FONTCONV_NUM)
                        printf(".");
#endif
                }
                bsc--;
                if (!bsc)
                    break;
                bitlc--;
                if (bitlc<0)
                {
                    bitlc = 7;
                    p++;
                }
            }
#ifdef DEBUG_FONTCONV
            if (cnt<DEBUG_FONTCONV_NUM)
                printf("\n");
#endif
        }
#ifdef DEBUG_FONTCONV
        cnt++;
#endif
    }
}

int main (int argc, char *argv[])
{
    struct Screen *my_screen;
    ULONG  screen_modeID;

    struct Screen   *pub_screen = NULL;
    struct DrawInfo *screen_drawinfo = NULL;

    STRPTR pub_screen_name = (STRPTR)"workbench";

    pub_screen = LockPubScreen(pub_screen_name);
    if (pub_screen != NULL)
    {
        screen_drawinfo = GetScreenDrawInfo(pub_screen);
        if (screen_drawinfo != NULL)
        {
            screen_modeID = GetVPModeID(&(pub_screen->ViewPort));
            if( screen_modeID != INVALID_ID )
            {
                my_screen = OpenScreenTags(NULL,
                    SA_Width,      pub_screen->Width,
                    SA_Height,     pub_screen->Height,
                    SA_Depth,      screen_drawinfo->dri_Depth,
                    SA_Overscan,   OSCAN_TEXT,
                    SA_AutoScroll, TRUE,
                    SA_Pens,       (ULONG)(screen_drawinfo->dri_Pens),
                    SA_DisplayID,  screen_modeID,
                    SA_Title,      (ULONG) "Cloned Screen",
                    TAG_END);
                if (my_screen != NULL)
                {
                    FreeScreenDrawInfo(pub_screen,screen_drawinfo);
                    screen_drawinfo = NULL;
                    UnlockPubScreen(pub_screen_name,pub_screen);
                    pub_screen = NULL;

                    struct DiskFontHeader *dfh = _loadFont (FONT_DIR, FONT_NAME, FONT_SIZE);

                    if (dfh)
                    {
                        struct TextFont *tf = &dfh->dfh_TF;
                        struct RastPort *rp = &my_screen->RastPort;

                        SetFont (rp, tf);
                        SetAPen (rp, 1);
                        Move (rp, 10, 30);
                        char *str = "Hello, World! 1234567890 ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                        Text (rp, (STRPTR)str, strlen(str));

                        static fontData_t fontData;
                        _fontConv (tf, &fontData);

                        Delay(300);   /* should be rest_of_program */

                        _freeFont(dfh);
                    }
                    CloseScreen(my_screen);
                }
            }
        }
    }

    if (screen_drawinfo != NULL )
        FreeScreenDrawInfo(pub_screen,screen_drawinfo);
    if (pub_screen != NULL )
        UnlockPubScreen(pub_screen_name,pub_screen);

    return 0;
}


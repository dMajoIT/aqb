/*
 * AQB: AmigaQuickBasic
 *
 * (C) 2020 by G. Bartsch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "parser.h"
#include "prabsyn.h"
#include "frame.h"
#include "semant.h"
#include "errormsg.h"
#include "canon.h"
#include "printtree.h"
#include "codegen.h"
#include "regalloc.h"
#include "options.h"
#include "env.h"

#define VERSION "0.3.0"

/* print the assembly language instructions to filename.s */
static void doProc(FILE *out, F_frame frame, T_stm body)
{
    AS_proc proc;
    T_stmList stmList;
    AS_instrList iList;

    F_tempMap = Temp_empty();
    //printStmList(stdout, T_StmList(body, NULL));

    stmList = C_linearize(body);
    if (OPT_get(OPTION_VERBOSE))
    {
        fprintf(stdout, ">>>>>>>>>>>>>>>>>>>>> Proc stmt list (after C_linearize)\n");
        printStmList(stdout, stmList);
        fprintf(stdout, "<<<<<<<<<<<<<<<<<<<<< Proc stmt list\n");
    }

    stmList = C_traceSchedule(C_basicBlocks(stmList));

    if (OPT_get(OPTION_VERBOSE))
    {
        fprintf(stdout, ">>>>>>>>>>>>>>>>>>>>> Proc stmt list (after C_traceSchedule)\n");
        printStmList(stdout, stmList);
        fprintf(stdout, "<<<<<<<<<<<<<<<<<<<<< Proc stmt list\n");
    }

    iList  = F_codegen(frame, stmList);

    if (OPT_get(OPTION_VERBOSE))
    {
        fprintf(stdout, ">>>>>>>>>>>>>>>>>>>>> AS stmt list after codegen, before regalloc:\n");
        AS_printInstrList (stdout, iList, Temp_layerMap(F_tempMap,Temp_getNameMap()));
        fprintf(stdout, "<<<<<<<<<<<<<<<<<<<<< AS stmt list\n");
    }

    struct RA_result ra = RA_regAlloc(frame, iList);
    iList = ra.il;

    proc = F_procEntryExitAS(frame, iList);

    if (OPT_get(OPTION_VERBOSE))
    {
        fprintf(stdout, ">>>>>>>>>>>>>>>>>>>>> AS stmt list\n");
        fprintf(stdout, "%s\n", proc->prolog);
        AS_printInstrList(stdout, proc->body, Temp_layerMap(F_tempMap, Temp_layerMap(ra.coloring, Temp_getNameMap())));
        fprintf(stdout, "%s\n", proc->epilog);
        fprintf(stdout, "<<<<<<<<<<<<<<<<<<<<< AS stmt list\n");
    }

    fprintf(out, "%s\n", proc->prolog);
    AS_printInstrList(out, proc->body, Temp_layerMap(F_tempMap, Temp_layerMap(ra.coloring, Temp_getNameMap())));
     fprintf(out, "%s\n", proc->epilog);
//  fprintf(out, "BEGIN function\n");
//  AS_printInstrList (out, iList,
//                     Temp_layerMap(F_tempMap, Temp_layerMap(ra.coloring, Temp_getNameMap())));
//  fprintf(out, "END function\n\n");
}

char *expand_escapes(const char* src)
{
    char *str = checked_malloc(2 * strlen(src) + 10);

    char *dest = str;
    char c;

    while ((c = *(src++)))
    {
        switch(c)
        {
            case '\a':
                *(dest++) = '\\';
                *(dest++) = 'a';
                break;
            case '\b':
                *(dest++) = '\\';
                *(dest++) = 'b';
                break;
            case '\t':
                *(dest++) = '\\';
                *(dest++) = 't';
                break;
            case '\n':
                *(dest++) = '\\';
                *(dest++) = 'n';
                break;
            case '\v':
                *(dest++) = '\\';
                *(dest++) = 'v';
                break;
            case '\f':
                *(dest++) = '\\';
                *(dest++) = 'f';
                break;
            case '\r':
                *(dest++) = '\\';
                *(dest++) = 'r';
                break;
            case '\\':
                *(dest++) = '\\';
                *(dest++) = '\\';
                break;
            case '\"':
                *(dest++) = '\\';
                *(dest++) = '\"';
                break;
            default:
                *(dest++) = c;
         }
    }

    *(dest++) = '\\';
    *(dest++) = '0';

    *(dest++) = '\0'; /* Ensure nul terminator */
    return str;
}

static void doStr(FILE * out, string str, Temp_label label) {
    fprintf(out, "    .align 4\n");
    fprintf(out, "%s:\n", Temp_labelstring(label));
    fprintf(out, "    .ascii \"%s\"\n", expand_escapes(str));
    fprintf(out, "\n");
}

static void doData(FILE * out, Temp_label label, int size, unsigned char *data)
{
    fprintf(out, "    .align 4\n");
    fprintf(out, "%s:\n", Temp_labelstring(label));
    if (data)
    {
        int i;
        switch(size)
        {
            case 1:
                fprintf(out, "    dc.b %d\n", data[0]);
                break;
            case 2:
                fprintf(out, "    dc.b %d, %d\n", data[1], data[0]);
                break;
            case 4:
                fprintf(out, "    dc.b %d, %d, %d, %d\n", data[3], data[2], data[1], data[0]);
                break;
            default:
                fprintf(out, "    dc.b");
                for (i=0; i<size; i++)
                {
                    fprintf(out, "%d", data[i]);
                    if (i<size-1)
                        fprintf(out, ",");
                }
                fprintf(out, "    \n");
                break;
        }

    }
    else
    {
        fprintf(out, "    .fill %d\n", size);
    }
    fprintf(out, "\n");
}

static void print_usage(char *argv[])
{
	fprintf(stderr, "usage: %s [-v] <program.bas>\n\n", argv[0]);
}

int main (int argc, char *argv[])
{
	char           *sourcefn;
	FILE           *sourcef;
    A_sourceProgram sourceProgram;
    F_fragList      frags, fl;
    char            asmfn[1024];
    FILE           *out;
    size_t 			optind;

    printf("AQB V" VERSION "\n");

    for (optind = 1; optind < argc && argv[optind][0] == '-'; optind++)
	{
        switch (argv[optind][1])
		{
        	case 'v':
				OPT_set(OPTION_VERBOSE, TRUE);
				break;
        	default:
				print_usage(argv);
	            exit(EXIT_FAILURE);
        }
    }

	if (argc != (optind+1))
	{
		print_usage(argv);
		exit(EXIT_FAILURE);
	}

	sourcefn = argv[optind];
    /* filename.bas -> filename.s */
    {
        int l = strlen(sourcefn);
        if (l>1024)
            l = 1024;
        if (l<4)
            l = 4;
        strncpy(asmfn, sourcefn, 1024);
        asmfn[l-3] = 's';
        asmfn[l-2] = 0;
    }

    EM_init();
    S_symbol_init();
    E_init();

    /*
     * parsing
     */

	sourcef = fopen(sourcefn, "r");
	if (!sourcef)
	{
		fprintf(stderr, "failed to read %s: %s\n\n", sourcefn, strerror(errno));
		exit(2);
	}

	if (!P_sourceProgram(sourcef, sourcefn, &sourceProgram))
        exit(3);
	fclose(sourcef);

    if (EM_anyErrors)
    {
        printf ("\n\nparsing failed.\n");
        exit(4);
    }

    if (OPT_get(OPTION_VERBOSE))
    {
        printf ("\n\nparsing worked.\n");
        pr_sourceProgram(stdout, sourceProgram, 0);
    }

    /*
     * intermediate code
     */

    F_initRegisters();

    frags = SEM_transProg(sourceProgram, Temp_namedlabel(AQB_MAIN_NAME));
    if (EM_anyErrors)
        exit(4);

    if (OPT_get(OPTION_VERBOSE))
    {
        printf ("\n\nsemantics worked.\n");
        F_printtree(stdout, frags);
    }

    /*
     * generate target assembly code
     */

    out = fopen(asmfn, "w");

    fprintf(out, ".globl %s\n\n", AQB_MAIN_NAME);
    /* Chapter 8, 9, 10, 11 & 12 */
    fprintf(out, ".text\n\n");
    for (fl=frags; fl; fl=fl->tail)
    {
        if (fl->head->kind == F_procFrag)
        {
            doProc(out, fl->head->u.proc.frame, fl->head->u.proc.body);
        }
    }

    fprintf(out, ".data\n\n");
    for (fl=frags; fl; fl=fl->tail)
    {
        if (fl->head->kind == F_stringFrag)
        {
            doStr(out, fl->head->u.stringg.str, fl->head->u.stringg.label);
        }
        if (fl->head->kind == F_dataFrag)
        {
            doData(out, fl->head->u.data.label, fl->head->u.data.size, fl->head->u.data.init);
        }
    }
    fclose(out);

    return 0;
}


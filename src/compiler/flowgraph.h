#ifndef HAVE_FLOWGRAPH_H
#define HAVE_FLOWGRAPH_H

#include "temp.h"

/*
 * flowgraph.h - Function prototypes to represent control flow graphs.
 */

Temp_tempList  FG_def(G_node n);
Temp_tempList  FG_use(G_node n);
Temp_tempLList FG_interferingRegsDef(G_node n) ;
Temp_tempLList FG_interferingRegsUse(G_node n) ;
bool           FG_isMove(G_node n);
AS_instr       FG_inst(G_node n);

G_graph        FG_AssemFlowGraph(AS_instrList il, F_frame f);

#endif

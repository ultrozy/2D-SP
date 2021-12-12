/* --------------------------------------------------------------------------
 * File: cplexremotel.h
 * Version 20.1.0
 * --------------------------------------------------------------------------
 * Licensed Materials - Property of IBM
 * 5725-A06 5725-A29 5724-Y48 5724-Y49 5724-Y54 5724-Y55 5655-Y21
 * Copyright IBM Corporation 1988, 2020. All Rights Reserved.
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with
 * IBM Corp.
 *---------------------------------------------------------------------------
 */

#ifndef CPX_CPLEX_REMOTEL_H
#   define CPX_CPLEX_REMOTEL_H 1
#include "ilcplex/cplexl.h"

#ifdef _WIN32
#pragma pack(push, 8)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if CPX_CPLEX_L_API_DEFINED > 0 || CPX_CPLEX_L_API_FORCE > 0
















































































CPXLIBAPI
int CPXPUBLIC
   CPXLloadLPhandle (CPXCENVptr env, CPXDESERIALIZERptr d,
                     CPXLPptr *lp_p);


CPXLIBAPI
int CPXPUBLIC
   CPXLloadNEThandle (CPXCENVptr env, CPXDESERIALIZERptr d,
                      CPXNETptr *net_p);


















































































CPXLIBAPI
int CPXPUBLIC
   CPXLstoreLPhandle (CPXCENVptr env, CPXSERIALIZERptr s, CPXCLPptr lp);


CPXLIBAPI
int CPXPUBLIC
   CPXLstoreNEThandle (CPXCENVptr env, CPXSERIALIZERptr s,
                       CPXCNETptr net);















#endif /* CPX_CPLEX_L_API_DEFINED > 0 */

#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#pragma pack(pop)
#endif

#endif /* !CPX_CPLEX_REMOTEL_H */

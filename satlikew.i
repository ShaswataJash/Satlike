/* File : satlikew.i */
%module satlikew

%{
#define SWIG_FILE_WITH_INIT
#include "basis_pms.h"
%}

/* Let's just grab the original header file here, ensure swig program is run from the directory where basis_pmh.h file is present */
/* If that is not the case, use complete absolute path for basis_pms.h below */
%include "basis_pms.h"
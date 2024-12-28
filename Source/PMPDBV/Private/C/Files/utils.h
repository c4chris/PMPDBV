//
//  utils.h
//  PDBV
//
//  Created by Christian Iseli on 22.09.2024.
//

#ifndef utils_h
#define utils_h

#include <stdio.h>

extern char* bundleRsrcDir;
extern char* downloadDir;
extern char* tempDir;

/* ----PROTOTYPES---------------------------------------------------------------------- */

void *safe_realloc(void **x, int l);

#define kEOF 1
#define kBufSize 1024
#define kOccupancyDotPos 57
#define kBfactorDotPos 63

short myfgets_(char *linbuf, int nChar, FILE *infile);

#endif /* utils_h */

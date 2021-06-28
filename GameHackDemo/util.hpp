#ifndef __UTIL__HPP__
#define __UTIL__HPP__

#include <cstdio>
#include <cstdlib>

typedef struct MatchingParam{
	float *HPptr, HPvalue;
}MatchingParam;


typedef struct DLLParam{
	float *HPptr;
}DLLParam;

void writeDLLParam2File(const char *fname, const DLLParam &dllparam);
void readDLLParamfromFile(const char *fname, DLLParam &dllparam);

#endif
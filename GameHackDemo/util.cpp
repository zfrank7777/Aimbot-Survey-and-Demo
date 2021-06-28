#include <cstdio>
#include <cstdlib>
#include "util.hpp"

float *addr_util;

void writeDLLParam2File(const char *fname, const DLLParam &dllparam){
	FILE *fp = fopen(fname, "w");
	fprintf(fp, "%012llx", dllparam.HPptr);
	fclose(fp);
}

void readDLLParamfromFile(const char *fname, DLLParam &dllparam){
	FILE *fp = fopen(fname, "r");
	fscanf(fp, "%012llx", &dllparam.HPptr);
	printf("%012llx\n", dllparam.HPptr);
	addr_util = dllparam.HPptr;
	fclose(fp);
}


#include "stdafx.h"
#include "Util.h"

// minsuk: All error related stuff should be here
// and then I propose change file name to errorhandling.cpp or something like that.

void ErrorHandling(char* message, DWORD error)
{
	char errorMessage[BUF_SIZE] = {0, };
	sprintf_s(errorMessage, "%s\n error No: %d", message, error);
	fputs(errorMessage, stderr);
	fputc('\n', stderr);
}

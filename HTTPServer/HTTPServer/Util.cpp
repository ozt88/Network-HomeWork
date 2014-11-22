#include "stdafx.h"
#include "Util.h"


void ErrorHandling(char* message, DWORD error)
{
	char errorMessage[BUF_SIZE] = {0, };
	sprintf_s(errorMessage, "%s\n error No: %d", message, error);
	fputs(errorMessage, stderr);
	fputc('\n', stderr);
}

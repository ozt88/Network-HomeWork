#pragma once

#define BUF_SIZE 2048
#define BUF_SMALL 100

enum ErrorCode
{
	ERROR_NONE,
	ERROR_400_BAD_REQUEST,
	ERROR_404_NOT_FOUND,
	ERROR_MAX,
};

void ErrorHandling(char* message, DWORD error);

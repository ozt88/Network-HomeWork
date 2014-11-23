#pragma once

#define BUF_SIZE 2048
#define BUF_SMALL 100
#define BAD_REQUEST_ERROR "400 Bad Request\r\n"
#define NOT_FOUND_ERROR "404 Not Found\r\n"

enum ErrorCode
{
	ERROR_NONE,
	ERROR_400_BAD_REQUEST,
	ERROR_404_NOT_FOUND,
	ERROR_MAX,
};
void SendErrorMSG( SOCKET socket , ErrorCode errorCode );
bool SafeStrToInt( OUT int* result , const char* inputString );

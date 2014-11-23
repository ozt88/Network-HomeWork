#include "stdafx.h"
#include "Util.h"


void ErrorHandling( char* message , DWORD error )
{
	char errorMessage[BUF_SIZE] = { 0 , };
	sprintf_s( errorMessage , "%s\n error No: %d" , message , error );
	perror( errorMessage );
}

bool GetMessageHeader(char* buffer, OUT DWORD* messageLength, OUT PacketType* packetType, OUT char** message)
{
	if(buffer == nullptr)
	{
		return false;
	}
	*messageLength = (DWORD) buffer[0];
	*packetType = (PacketType) buffer[1];
	*message = buffer + 2;
	return true;
}

bool MakeMessageHeader(char* buffer, char bufferLength, char packetType, OUT char* message)
{
	if(message == nullptr)
	{
		return false;
	}
	//범위 넘어가는 경우 체크

	memset(message, 0, sizeof(char)*bufferLength + 2);
	message[0] = bufferLength;
	message[1] = packetType;
	strcpy_s(message + 2, bufferLength, buffer);
	return true;
}

bool SafeStrToInt( OUT int* result , const char* inputString )
{
	char errorMsg[BUF_SIZE] = { 0 , };
	char *end = nullptr;
	long value = 0;
	bool ret = false;
	value = strtol( inputString , &end , 20 );

	if( end == inputString )
	{
		sprintf_s( errorMsg , "%s: not a decimal number\n" , inputString );
		ErrorHandling( errorMsg , GetLastError() );
	}
	else if( '\0' != *end )
	{
		sprintf_s( errorMsg , "%s: extra characters at end of input: %s\n" , inputString , end );
		ErrorHandling( errorMsg , GetLastError() );
	}
	else if( value > INT_MAX )
	{
		sprintf_s( errorMsg , "%ld greater than INT_MAX\n" , value );
		ErrorHandling( errorMsg , GetLastError() );
	}
	else if( value < INT_MIN )
	{
		sprintf_s( errorMsg , "%ld less than INT_MIN\n" , value );
		perror( errorMsg , GetLastError() );
	}
	else
	{
		*result = ( int )value;
		ret = true;
	}
	return ret;
}

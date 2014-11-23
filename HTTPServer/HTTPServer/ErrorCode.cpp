#include "stdafx.h"
#include "ErrorCode.h"

void SendErrorMSG( SOCKET socket , ErrorCode errorCode )
{
	char protocol[BUF_SMALL] = { 0 , };
	char cntLen[BUF_SMALL] = { 0 , };
	char content[BUF_SIZE] = { 0 , };
	char* errorMessage;
	switch( errorCode )
	{
		case ERROR_400_BAD_REQUEST:
			errorMessage = BAD_REQUEST_ERROR;
			break;
		case ERROR_404_NOT_FOUND:
			errorMessage = NOT_FOUND_ERROR;
			break;
		default:
			errorMessage = BAD_REQUEST_ERROR;
			break;
	}

	char servName[] = "Server : simple web server\r\n";
	char cntType[] = "Content-type:text/html\r\n\r\n";
	sprintf_s( protocol , "HTTP/1.0 %s" , errorMessage );
	sprintf_s( content , "<html><head><title>NETWORK</title></head>"
			   "<body><font size=+2><br>%s"
			   "</font></body></html>" , errorMessage );
	sprintf_s( cntLen , "Content-length : %d\r\n" , strlen( content ) );


	if( send( socket , protocol , strlen( protocol ) , 0 ) == SOCKET_ERROR )
	{
		perror( "send():protocol error!" );
		return;
	}
	if( send( socket , servName , strlen( servName ) , 0 ) == SOCKET_ERROR )
	{
		perror( "send():servName error!");
		return;
	}
	if( send( socket , cntLen , strlen( cntLen ) , 0 ) == SOCKET_ERROR )
	{
		perror( "send():cntLen error!");
		return;
	}
	if( send( socket , cntType , strlen( cntType ) , 0 ) == SOCKET_ERROR )
	{
		perror( "send():cntType error!");
		return;
	}
	if( send( socket , content , strlen( content ) , 0 ) == SOCKET_ERROR )
	{
		perror( "send():content error!");
		return;
	}
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
		perror( errorMsg );
	}
	else if( '\0' != *end )
	{
		sprintf_s( errorMsg , "%s: extra characters at end of input: %s\n" , inputString , end );
		perror( errorMsg );
	}
	else if( value > INT_MAX )
	{
		sprintf_s( errorMsg , "%ld greater than INT_MAX\n" , value );
		perror( errorMsg );
	}
	else if( value < INT_MIN )
	{
		sprintf_s( errorMsg , "%ld less than INT_MIN\n" , value );
		perror( errorMsg );
	}
	else
	{
		*result = ( int )value;
		ret = true;
	}
	return ret;
}

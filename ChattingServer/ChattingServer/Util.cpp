#include "stdafx.h"
#include "Util.h"


void ErrorHandling(char* message, DWORD error)
{
	char errorMessage[BUF_SIZE] = {0, };
	sprintf_s(errorMessage, "%s\n error No: %d", message, error);
	fputs(errorMessage, stderr);
	fputc('\n', stderr);
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

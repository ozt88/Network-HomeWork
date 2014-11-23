#include "stdafx.h"
#include "Util.h"



bool GetMessageHeader(char* buffer, OUT DWORD* messageLength, OUT PacketType* packetType, OUT char** message)
{
	if(buffer == nullptr)
	{
		return false;
	}
	// minsuk: it looks better if { and } are removed..
	*messageLength = (DWORD)buffer[0];
	*packetType = (PacketType)buffer[1];
	*message = buffer + 2;
	return true;
}

bool MakeMessageHeader(char* buffer, unsigned char bufferLength, char packetType, OUT char* message)
{
	if(message == nullptr)
	{
		return false;
	}
	//¹üÀ§ ³Ñ¾î°¡´Â °æ¿ì Ã¼Å©

	memset(message, 0, sizeof(char)*bufferLength);
	message[0] = bufferLength;
	message[1] = packetType;
	memcpy(message + 2, buffer, bufferLength - 2);
	return true;
}

void Gotoxy(int x, int y)
{
	COORD pos = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Hidecursor()
{
	CONSOLE_CURSOR_INFO info = {100, FALSE};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

void ClearLine(int lineNum)
{
	Lock lock;
	Gotoxy(0, lineNum);
	printf("%100c", ' ');
}



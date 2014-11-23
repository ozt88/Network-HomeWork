#pragma once
#define BUF_SIZE 1024
#define MAX_PRINT_LINE 25
#define MAX_MSG_LENGTH 253
#define MAX_LENGTH_BY_LINE 50

extern CRITICAL_SECTION gCriticalSection;

enum PacketType
{
	PACKET_NONE,
	PACKET_ENTER,
	PACKET_CHAT,
	PACKET_EXIT,
	PACKET_MAX,
};

struct PacketHeader
{
	unsigned char m_BytesTrans;
	unsigned char m_PacketType;
};

struct Lock
{
	Lock()
	{
		EnterCriticalSection(&gCriticalSection);
	}
	~Lock()
	{
		LeaveCriticalSection(&gCriticalSection);
	}
};

template <class T>
void SafeDelete(T deletePtr)
{
	if(deletePtr != nullptr)
	{
		delete deletePtr;
		deletePtr = nullptr;
	}
}

bool	GetMessageHeader(char* buffer, OUT DWORD* messageLength, OUT PacketType* packetType, OUT char** message);
bool	MakeMessageHeader(char* buffer, unsigned char bufferLength, char packetType, OUT char* message);
void	Gotoxy(int x, int y);
void	GetInput(OUT char** buffer);
void	Hidecursor();
void	ClearLine(int lineNum);
bool	SafeStrToInt( OUT int* result , char* inputString );
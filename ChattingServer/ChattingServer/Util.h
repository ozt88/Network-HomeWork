#pragma once
#define BUF_SIZE 1024
#define DEFAULT_CLIENT_NUM 10
#define MAX_THREAD_NUM 10

extern CRITICAL_SECTION gCriticalSection;

enum Mode
{
	MODE_NONE,
	MODE_RECV,
	MODE_SEND,
	MODE_END,
	MODE_MAX,
};

enum PacketType
{
	PACKET_NONE,
	PACKET_ENTER,
	PACKET_CHAT,
	PACKET_EXIT,
	PACKET_MAX,
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

struct IOData
{
	IOData()
		:m_Mode(MODE_NONE)
	{
		memset(&m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	}
	~IOData()
	{
	}

	WSAOVERLAPPED	m_Overlapped;
	Mode			m_Mode;
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

void ErrorHandling(char* message, DWORD error);
bool GetMessageHeader(char* buffer, OUT DWORD* messageLength, OUT PacketType* packetType, OUT char** message);
bool MakeMessageHeader(char* buffer, char bufferLength, char packetType, OUT char* message);


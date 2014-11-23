// ChattingClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Util.h"
#include "CustomBuffer.h"


unsigned int WINAPI SendThreadProc(LPVOID argv);
unsigned int WINAPI RecvThreadProc(LPVOID argv);
int					SendMessage(SOCKET socket, char* message, int length);
int					RecvMessage(SOCKET socket, char* buffer, int length);
void				ErrorHandling(char* message, DWORD error);
void				GetInput(char* inputBuffer, unsigned char* inputPointer);
void				GetPacketHeader(SOCKET socket, OUT PacketHeader* header);
void				Log(char* message);
void				PrintLogs();
void				PrintInput(char* inputBuffer, int length);
void				ErrorHandling(char* message, DWORD error);
bool				SafeStrToInt( OUT int* result , const char* inputString );

std::deque<char*> logs;
CRITICAL_SECTION gCriticalSection;

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	HANDLE hSendThread, hRecvThread;
	DWORD dwSendThreadId, dwRecvThreadId;
	SOCKET hServSock;
	SOCKADDR_IN servAddr;
	int port = 0;
	const char* ip;
	InitializeCriticalSection(&gCriticalSection);

	if(argc != 2)
	{
#ifndef _DEBUG
		Log("Usage: %s <IP> <PORT>\n", argv[0]);
		exit(1);
#else
		port = 8000;
		ip = "127.0.0.1";
#endif
	}
	else
	{
		ip = (const char*)argv[1];
		if( !SafeStrToInt( &port , ( const char* )argv[2] ) )
		{
			return 1;
		}
	}

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error!", GetLastError());
		return 1;
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if(hServSock == INVALID_SOCKET)
	{
		ErrorHandling("socket() error!", GetLastError());
		return 1;
	}

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(port);

	if(connect(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		ErrorHandling("connect() error!", GetLastError());
		return 1;
	}
	else
	{
		Log("Connected........");
	}

	Hidecursor();
	hSendThread = (HANDLE) _beginthreadex(NULL, 0, SendThreadProc, (LPVOID)hServSock, 0, (unsigned*) &dwSendThreadId);
	if(hSendThread == INVALID_HANDLE_VALUE)
	{
		ErrorHandling("_beginthreadex() : SendThread error!", GetLastError());
		return 1;
	}

	hRecvThread = (HANDLE) _beginthreadex(NULL, 0, RecvThreadProc, (LPVOID) hServSock, 0, (unsigned*) &dwRecvThreadId);
	if(hRecvThread == INVALID_HANDLE_VALUE)
	{
		ErrorHandling("_beginthreadex() : RecvThread error!", GetLastError());
		return 1;
	}

	WaitForSingleObject(hSendThread, INFINITE);
	WaitForSingleObject(hRecvThread, INFINITE);
	_endthreadex(dwSendThreadId);
	_endthreadex(dwRecvThreadId);
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

int SendMessage(SOCKET socket, char* message, int length)
{
	int ret = 0;
	int sentBytes = 0;
	while(sentBytes < length)
	{
		ret = send(socket, message + sentBytes, length - sentBytes, NULL);
		if(ret == SOCKET_ERROR)
		{
			ErrorHandling("send() error!", GetLastError());
			return ret;
		}
		sentBytes += ret;
	}
	return sentBytes;
}

int RecvMessage(SOCKET socket, char* buffer, int length)
{
	int ret = 0;
	int recvBytes = 0;
	while(recvBytes < length)
	{
		ret = recv(socket, buffer + recvBytes, length - recvBytes, NULL);
		if(ret == SOCKET_ERROR)
		{
			ErrorHandling("recv() error!", GetLastError()); 
			return ret;
		}
		else if(ret == 0)
		{
			break;
		}
		recvBytes += ret;
	}
	return recvBytes;
}

unsigned int WINAPI SendThreadProc(LPVOID argv)
{
	SOCKET hServSock = (SOCKET) argv;
	char inputBuffer[BUF_SIZE] = {0, };
	char message[BUF_SIZE] = {0, };
	unsigned char messageSize = 0;
	unsigned char inputSize = 0;

	while(true)
	{
		memset(message, 0, sizeof(char) * BUF_SIZE);
		memset(inputBuffer, 0, sizeof(char)*BUF_SIZE);
		inputSize = 0;
		GetInput(inputBuffer, &inputSize);
		messageSize = inputSize + 2;
		MakeMessageHeader(inputBuffer, messageSize, PACKET_CHAT, message);
		if( !strcmp(message , "q\n") || !strcmp(message , "Q\n") ) 
		{
			break;
		}
		if(SendMessage(hServSock, message, messageSize) == SOCKET_ERROR)
		{
			ErrorHandling("sendMessage() error!", GetLastError());
			break;
		}
	}

	return 0;
}

unsigned int WINAPI RecvThreadProc(LPVOID argv)
{
	SOCKET hServSock = (SOCKET) argv;
	int ret = 0;
	CustomBuffer packetBuffer(BUF_SIZE);
	char* message = new char[BUF_SIZE];
	unsigned short messageLength = 0;
	PacketHeader packetHeader;
	while(true)
	{
		memset(message, 0, sizeof(char)*BUF_SIZE);
		GetPacketHeader(hServSock, &packetHeader);
		messageLength = (unsigned char)(packetHeader.m_BytesTrans) - 2;
		ret = RecvMessage( hServSock , packetBuffer.GetBuffer() , messageLength );
		if(ret == SOCKET_ERROR)
		{
			ErrorHandling("RecvMessage(): error!", GetLastError());
			getchar();
			return 1;
		}
		else if( ret == 0 )
		{
			return 0;
		}
		packetBuffer.Commit(messageLength);
		packetBuffer.Read(message, messageLength);
		switch((PacketType)packetHeader.m_PacketType)
		{
			case PACKET_NONE:
				Log(message);
				break;
			case PACKET_ENTER:
				Log("Enter CSTalk");
				break;
			case PACKET_CHAT:
				Log(message);
				break;
			case PACKET_EXIT:
				break;
			default:
				break;
		}
	}
	return 0;
}

void GetInput(char* inputBuffer, unsigned char* inputPointer)
{
	char inputChar = NULL;
	int currentPointer = *inputPointer;
	while(true)
	{
		PrintInput(inputBuffer, currentPointer);
		inputChar = _getch();
		if(currentPointer > MAX_MSG_LENGTH)
		{
			currentPointer--;
			continue;
		}
		else if( ( currentPointer > 0 ) && ( inputChar == '\b' ) ) // backspace
		{
			inputBuffer[--currentPointer] = '\0';
		}
		else if(inputChar == '\r')
		{
			ClearLine(MAX_PRINT_LINE);
			inputBuffer[currentPointer++] = ' ';
			inputBuffer[currentPointer] = '\0';
			*inputPointer = currentPointer;
			break;
		}
		else 
		{
			if((currentPointer != 0 )&& (currentPointer%MAX_LENGTH_BY_LINE == 0)) 
			{
				inputBuffer[currentPointer++] = '\n';
			}
			inputBuffer[currentPointer++] = inputChar;
		}
	}
}

void PrintInput(char* inputBuffer, int length)
{
	EnterCriticalSection( &gCriticalSection );
	Gotoxy(1, MAX_PRINT_LINE);
	printf_s("ME> %s%50c", inputBuffer, ' ', length);
	LeaveCriticalSection( &gCriticalSection );
}

void PrintLogs()
{
	EnterCriticalSection( &gCriticalSection );
	for(int i = 0; i < logs.size(); ++i)
	{
		ClearLine(i + 1);
		Gotoxy(0, i + 1);
		printf_s("%s", logs[i], strlen(logs[i]));
	}
	LeaveCriticalSection( &gCriticalSection );
}

void Log(char* message)
{
	int length = strlen(message);
	char* log = new char[length + 1];
	memset(log, 0, sizeof(char)*(length + 1));
	memcpy(log, message, length);
	EnterCriticalSection( &gCriticalSection );
	logs.push_back(log);
	if(logs.size() > MAX_PRINT_LINE)
	{
		char* delNode = logs.front();
		logs.pop_front();
		SafeDelete<char*>(delNode);
	}
	PrintLogs();
	LeaveCriticalSection( &gCriticalSection );
}

void ErrorHandling(char* message, DWORD error)
{
	char errorMessage[BUF_SIZE] = {0, };
	sprintf_s(errorMessage, "%s error No: %d", message, error);
	Log(errorMessage);
}

void GetPacketHeader(SOCKET socket, OUT PacketHeader* header)
{
	if(RecvMessage(socket, (char*) header, sizeof(PacketHeader)) == SOCKET_ERROR)
	{
		ErrorHandling("GetPacketSize(): error!", GetLastError());
		getchar();
		exit(1);
	}
	return;
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
		ErrorHandling( errorMsg , GetLastError() );
	}
	else
	{
		*result = ( int )value;
		ret = true;
	}
	return ret;
}

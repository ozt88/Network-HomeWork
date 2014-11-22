#pragma once
#include "Util.h"

class ClientSession;
class ClientManager
{
public:
	static ClientManager*	GetInstance();
	static void				ReleaseInstance();
	bool					AddClient(ClientSession* client);
	bool					RemoveClient(ClientSession* client);
	bool					SendToAllClient(char* message, DWORD bytesTrans);

private:
	ClientManager();
	~ClientManager();

	static ClientManager*			m_Instance;
	std::vector<ClientSession*>		m_ClientSessions;

};


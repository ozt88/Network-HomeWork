#include "stdafx.h"
#include "ClientManager.h"
#include "ClientSession.h"

ClientManager* ClientManager::m_Instance = nullptr;
ClientManager* ClientManager::GetInstance()
{
	if(m_Instance == nullptr)
	{
		m_Instance = new ClientManager();
	}
	return m_Instance;
}

void ClientManager::ReleaseInstance()
{
	if(m_Instance != nullptr)
	{
		delete m_Instance;
		m_Instance = nullptr;
	}
}

bool ClientManager::AddClient(ClientSession* client)
{
	if(!client)
	{
		return false;
	}
	auto findIter = std::find(m_ClientSessions.begin(), m_ClientSessions.end(), client);
	if(findIter != m_ClientSessions.end())
	{
		//이미 있으면 실패
		return false;
	}
	m_ClientSessions.push_back(client);
	return true;
}

bool ClientManager::RemoveClient(ClientSession* clientSession)
{
	if(!clientSession)
	{
		return false;
	}
	auto findIter = std::find(m_ClientSessions.begin(), m_ClientSessions.end(), clientSession);
	if(findIter == m_ClientSessions.end())
	{
		//없으면 실패
		return false;
	}
	m_ClientSessions.erase(findIter);
	clientSession->Disconnect();
	SafeDelete<ClientSession*>(clientSession);
}

bool ClientManager::SendToAllClient(char* message, DWORD bytesTrans)
{
	bool ret = true;
	for(auto iter = m_ClientSessions.begin(); iter != m_ClientSessions.end(); iter++)
	{
		
		auto clientSession = *iter;
		ret = clientSession->SendToClient(message, bytesTrans);
		if(!ret)
		{
			ret = false;
			break;
		}
	}
	return ret;
}

ClientManager::ClientManager()
{
	m_ClientSessions.reserve(MAX_THREAD_NUM);
}

ClientManager::~ClientManager()
{
	for(auto iter = m_ClientSessions.begin(); iter != m_ClientSessions.end(); iter++)
	{
		auto clientSession = *iter;
		RemoveClient(clientSession);
	}
	m_ClientSessions.clear();
}


#pragma once
#include "Util.h"

class CustomBuffer
{
public:

	CustomBuffer(size_t capacity) : m_StartPointer(0), m_EndPointer(0), m_Capacity(capacity)
	{
		m_Buffer = new char[capacity];
		memset(m_Buffer, 0, sizeof(char)*capacity);
	}

	~CustomBuffer()
	{
		SafeDelete<char*>(m_Buffer);
	}

	bool	Peek(OUT char* buffer, size_t bytes) const;
	bool	Read(OUT char* buffer, size_t bytes);
	bool	Write(const char* data, size_t bytes);
	bool	Commit(size_t len);
	bool	Remove(size_t len);
	void	Flush();
	size_t	GetAvailableSize() const {return (size_t) ( m_Capacity - m_EndPointer );}
	size_t	GetStoredSize() const {return (size_t) ( m_Capacity - GetAvailableSize());};
	char*	GetBuffer() const {return m_Buffer+m_StartPointer; }

private:
	char*			m_Buffer;
	unsigned int	m_StartPointer;
	unsigned int	m_EndPointer;
	size_t			m_Capacity;
};

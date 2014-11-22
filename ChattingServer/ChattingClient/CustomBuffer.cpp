#include "stdafx.h"
#include "CustomBuffer.h"


bool CustomBuffer::Peek(OUT char* buffer, size_t bytes) const
{
	if(GetStoredSize() < bytes)
	{
		return false;
	}
	memcpy(buffer, GetBuffer(), bytes);

	return true;
}

bool CustomBuffer::Read(OUT char* buffer, size_t bytes)
{
	if(GetStoredSize() < bytes)
	{
		bytes = GetStoredSize();
	}
	memcpy(buffer, GetBuffer(), bytes);
	Remove(bytes);
	return true;
}

bool CustomBuffer::Write(const char* data, size_t bytes)
{
	if(GetAvailableSize() < bytes)
	{
		return false;
	}
	memcpy(m_Buffer + m_EndPointer, data, bytes);
	m_EndPointer += bytes;
	return true;
}

bool CustomBuffer::Commit(size_t len)
{
	if(GetAvailableSize() < len)
	{
		return false;
	}
	m_EndPointer += len;
	return true;
}

void CustomBuffer::Flush()
{
	Remove(GetStoredSize());
}

bool CustomBuffer::Remove(size_t len)
{
	if(len > GetStoredSize())
	{
		len = GetStoredSize();
	}
	m_EndPointer -= len;
	memmove(GetBuffer(), GetBuffer() + len, GetStoredSize());

	return true;
}




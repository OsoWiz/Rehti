#include "RehtiAlloc.hpp"

StackAllocator::StackAllocator(size_t size)
{
	m_data = new char[size];
	m_top = m_data;
	m_end = m_data + size;
}

StackAllocator::~StackAllocator()
{
}

void* StackAllocator::allocate(size_t size)
{
	return nullptr;
}

void StackAllocator::deallocate()
{
}

#pragma once

template<size_t size>
class ConstAllocator
{
public:
	void* allocate(size_t size)
	{
		return m_data;
	};

private:
	char m_data[size];
};




class BlockAllocator
{
	BlockAllocator(size_t blockSize, size_t numBlocks);
	~BlockAllocator();

	void* allocate(size_t size);

private:
	size_t m_blockSize;
	size_t m_numBlocks;
	char* m_data;
	char* m_freeList;
};

class StackAllocator
{
public:
	StackAllocator(size_t size);
	~StackAllocator();

	void* allocate(size_t size);
	void deallocate();
private:
	char* m_data;
	char* m_top;
	char* m_end;
};
/*********************************************************************
* MemPool class                                       					*
* Small_Object_Allocator class                                       *
*                                                                    *
* Version: 1.0                                                       *
* Date:    23-09-2025                                                *
* Author:  Dan Machado                                               *                                         *
**********************************************************************/
#ifndef SMALL_OBJECT_ALLOCATOR_H
#define SMALL_OBJECT_ALLOCATOR_H

#include <vector>
#include <mutex>

//====================================================================

namespace Allocator
{

#define CHUNK_SIZE 32

#define MEM_PAGE_SIZE 0x1000 // 4096

//====================================================================

class MemPool
{
	public:
		MemPool(size_t BLOCK_SIZE)
		: c_BLOCK_SIZE(BLOCK_SIZE)
		, c_TOTAL_BLOCKS(MEM_PAGE_SIZE/c_BLOCK_SIZE>20? MEM_PAGE_SIZE/c_BLOCK_SIZE : 20)
		{
			m_pool.reserve(64);
		}

		MemPool(MemPool&& other)
		: m_pool(std::move(other.m_pool))
		, m_availableChunk{other.m_availableChunk}
		, c_BLOCK_SIZE(other.c_BLOCK_SIZE)
		, c_TOTAL_BLOCKS(other.c_TOTAL_BLOCKS)
		{
			other.m_availableChunk=nullptr;
		}

		MemPool(const MemPool&)=delete;
		MemPool& operator=(const MemPool&)=delete;

		~MemPool()
		{
			for(auto chunk : m_pool){
				if(chunk){
					delete[] chunk;
				}
			}
		}

		unsigned char* allocateMem() __attribute__((always_inline)) __attribute__((hot));
		void freeMem(void* ptr) __attribute__((always_inline));

		void init() __attribute__((always_inline))
		{
			/*
			 * Notice that the min BLOCK_SIZE is 8 because as we are
			 * writing the next address in every chunk, an address
			 * needs 8bytes: |c_BLOCK_SIZE|c_BLOCK_SIZE|c_BLOCK_SIZE|...|c_BLOCK_SIZE|
			 * (we can not write an address of 8bytes in a c_BLOCK_SIZE of 4!)
			 * */
			m_availableChunk=new unsigned char[m_growBy*c_TOTAL_BLOCKS*c_BLOCK_SIZE];

			if(!m_availableChunk){
				throw std::bad_alloc();
			}

			for(size_t i=0; i<c_TOTAL_BLOCKS*m_growBy; i++){
				setNextAddr(&m_availableChunk[i*c_BLOCK_SIZE], &m_availableChunk[(i+1)*c_BLOCK_SIZE]);
			}

			setNextAddr(&m_availableChunk[(c_TOTAL_BLOCKS*m_growBy-1)*c_BLOCK_SIZE], nullptr);
			m_pool.emplace_back(m_availableChunk);
			m_growBy++;
		}

	private:
		std::vector<unsigned char*> m_pool;
		unsigned char* m_availableChunk{nullptr};
		uint c_BLOCK_SIZE;
		uint c_TOTAL_BLOCKS;
		uint m_growBy{1};

		static void setNextAddr(unsigned char* chunk, unsigned char* next) __attribute__((always_inline))
		{
			unsigned char** ptr=reinterpret_cast<unsigned char**>(chunk);
			*ptr=next;
		}
};

//--------------------------------------------------------------------

inline unsigned char* MemPool::allocateMem()
{
	if(!m_availableChunk){
		init();
	}

	unsigned char* tmp=m_availableChunk;
	m_availableChunk=*(reinterpret_cast<unsigned char**>(tmp));

	return tmp;
}

//--------------------------------------------------------------------

inline void MemPool::freeMem(void* obj)
{
	unsigned char** ptr=reinterpret_cast<unsigned char**>(obj);
	*ptr=m_availableChunk;
	m_availableChunk=reinterpret_cast<unsigned char*>(obj);
}

//====================================================================

class Small_Object_Allocator
{
	public:
		Small_Object_Allocator()
		{
			init();
		}
		
		~Small_Object_Allocator()=default;
		
		static void init();
		
		void* allocate(std::size_t block_size);
		
		void deallocate(void* p, std::size_t block_size);

		template<typename T, typename... Args>
		T* alloc(Args&& ...args);
		
		template<typename T>
		void free(T* p);

	private:
		static inline std::vector<MemPool> m_allocators;
		static inline std::size_t c_max_object_size{2048};

		Small_Object_Allocator(const Small_Object_Allocator&);
		Small_Object_Allocator& operator=(const Small_Object_Allocator&);
};

//--------------------------------------------------------------------
/*
 * Notice that allocations for easyJson are in powers of 32
 * so we restrict to max_obj_size to 2048 because
 * this is a buffer use by vector of nodes so the next allocation
 * from a vector is 4096 (MEM_PAGE_SIZE) which is allocated using new
 * */

inline void Small_Object_Allocator::init()
{
	if(m_allocators.size()>0){
		return;
	}

	size_t bins=0;
	while(c_max_object_size>=(size_t(CHUNK_SIZE)<<bins)){
		bins++;
	}
	
	m_allocators.reserve(bins);

	for(size_t i=0; i<bins; i++){
		m_allocators.emplace_back(CHUNK_SIZE<<i);
	}
	
	m_allocators[0].init();
}

//--------------------------------------------------------------------

template<typename T, typename... Args>
inline T* Small_Object_Allocator::alloc(Args&& ...args)
{
	return new(allocate(sizeof(T))) T(std::forward<Args>(args)...);
}

//--------------------------------------------------------------------

template<typename T>
inline void Small_Object_Allocator::free(T* p)
{
	p->T::~T();
	deallocate(p, sizeof(T));
}

//--------------------------------------------------------------------

inline void* Small_Object_Allocator::allocate(std::size_t block_size)
{
	if(block_size>c_max_object_size){
		return ::operator new(block_size);
	}

	size_t idx=0;
	while(block_size>(size_t(CHUNK_SIZE)<<(idx++)));

	return m_allocators[idx-1].allocateMem();
}

//--------------------------------------------------------------------

inline void Small_Object_Allocator::deallocate(void* p, std::size_t block_size)
{
	if(block_size>c_max_object_size){
		::operator delete(p);
		return;
	}
	
	size_t idx=0;
	while(block_size>(size_t(CHUNK_SIZE)<<(idx++)));
	
	m_allocators[idx-1].freeMem(p);
}

//====================================================================

}

#endif

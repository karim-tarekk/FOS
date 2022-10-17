#include <inc/lib.h>

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
struct Uheap_info{
	uint32 address;
	uint32 size;
};

bool Uheap[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE]={0};
struct Uheap_info UsedPages[((USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE)];
int idx=0;
int lastloc = 0;
void* Next_fit_strategy(uint32 size){
	int n=(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE;
	size = ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;
	int cnt = 0;
	uint32 start_of_free_page;
	bool flag = 0;
	if(lastloc >= n)
		lastloc = 0;
	int i = lastloc;
	int l = 0;
	while(i <= n)
	{
		if(Uheap[i] == 0)
		{
			if (flag == 0)
			{
				start_of_free_page = USER_HEAP_START + i*PAGE_SIZE;
				flag = 1;
			}
			if(i == n)
			{
				if (l == 0)
				{
					l = 1;
					flag = 0;
					i = 0;
					cnt = 0;
				}
				else break;
			}
			else
			{
				i++;
				cnt++;
			}
		}
		else
		{

			if (i == n)
			{
				if (l == 0)
				{
					l = 1;
					flag = 0;
					i = 0;
					cnt = 0;
				}
				else return NULL;
			}
			else
			{
				i++;
				cnt = 0;
				flag = 0;
			}

		}
		if(cnt == size)
			break;
	}
	return(void*)start_of_free_page;
}

void* malloc(uint32 size)
{
	int Pages = ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;
	int blocks = (USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE;
	uint32* address;
	if(sys_isUHeapPlacementStrategyNEXTFIT())
		address = Next_fit_strategy(size);

	if(address == NULL)
		return NULL;

	int index = ((uint32) address - USER_HEAP_START)/ PAGE_SIZE;
	UsedPages[idx].size = size;
	sys_allocateMem((uint32)address, size);
	UsedPages[idx].address = (uint32)address;

	for(int i=0; i < Pages; i++) {
		Uheap[i+index]=1;
	}
	lastloc = Pages + index;
	idx++;
	return (void*) address;
}

void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	panic("smalloc() is not required ..!!");
	return NULL;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	panic("sget() is not required ..!!");
	return 0;
}
void free(void* virtual_address)
{
	int index = ((uint32) virtual_address - USER_HEAP_START)/ PAGE_SIZE;
	uint32 size;
	for(int i=0; i<idx; i++)
	{
		if(UsedPages[i].address == (uint32)virtual_address )
		{
			size = UsedPages[i].size;
			UsedPages[i].address=UsedPages[idx-1].address;
			UsedPages[i].size=UsedPages[idx-1].size;
			sys_freeMem((uint32)virtual_address,size);
			idx--;
			break;
		}
	}
	size = ROUNDUP(size, PAGE_SIZE)/PAGE_SIZE;
	for(int i=0; i<size; i++)
		Uheap[i+index] =0;
}


void sfree(void* virtual_address)
{
	panic("sfree() is not requried ..!!");
}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2022 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

	return NULL;
}

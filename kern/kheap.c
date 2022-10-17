#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//2022: NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
uint32 Lastlocated = KERNEL_HEAP_START;
uint32 NoBlocks[((KERNEL_HEAP_MAX-KERNEL_HEAP_START))/PAGE_SIZE];
int Checknextpages(uint32 location, int noPAges)
{
	int count = 0 ;
	while(count < noPAges)
	{
		uint32 *ptr_table = NULL;
		struct Frame_Info* ptr = get_frame_info(ptr_page_directory, (void*)location, &ptr_table);
		if(ptr == NULL)
		{
			 if(location == KERNEL_HEAP_MAX)
			 {
				 location = KERNEL_HEAP_START;
			 }
			 else
			 {
				 location+=PAGE_SIZE;
			 }
		}
		else
		{
			return 0;
		}
		if(count == noPAges-1)
		{
			return 1;
		}
		count ++;

	}
	return 0;
}
void* kmalloc(unsigned int size)
{
	uint32 myloc = Lastlocated;
	size = ROUNDUP(size , PAGE_SIZE) ;
	int pages = size / PAGE_SIZE ;
	if (KERNEL_HEAP_MAX - Lastlocated < size)
	{
		return NULL ;
	}
	int l = 0;
	while (Checknextpages(myloc,pages)==0)
	{
		 if(myloc == KERNEL_HEAP_MAX)
		 {
			 if (l==0)
			 {
				 myloc = KERNEL_HEAP_START;
				 l=1;
			 }
			 else
			 {
				 return NULL;
			 }
		 }
		 else
		 {
			 myloc += PAGE_SIZE;
		 }
	}
	uint32 output  = myloc;
	int idx = (output - KERNEL_HEAP_START)/ PAGE_SIZE ;
	for (int i = 0; i < pages; i++)
	{
		uint32 *ptr_table = NULL;
		struct Frame_Info* ptr = get_frame_info(ptr_page_directory, (void*)myloc, &ptr_table);
		int re = allocate_frame(&ptr);
		if(re != E_NO_MEM)
		{
		   map_frame(ptr_page_directory, ptr, (void *)myloc, PERM_WRITEABLE);
		}
		   myloc += PAGE_SIZE;
	}
	NoBlocks[idx] = pages;
	if(myloc == KERNEL_HEAP_MAX)
		Lastlocated = KERNEL_HEAP_START;
	else
		Lastlocated = myloc;
	return (void*)output;
}
void kfree(void* virtual_address)
{
	uint32 *PT_ptr = NULL;
	int idx = ((uint32)virtual_address - KERNEL_HEAP_START)/ PAGE_SIZE;
	for(int k = 0; k< NoBlocks[idx] ; k++)
	{
		uint32 *ptr_table = NULL;
		uint32 address = ((uint32)virtual_address+(k*PAGE_SIZE)) ;
		struct Frame_Info *ptr_frame ;
		ptr_frame = get_frame_info(ptr_page_directory, (void*)address, &PT_ptr);
		if(ptr_frame != NULL)
			free_frame(ptr_frame);
		unmap_frame(ptr_page_directory, (void*) address);
	}
}
unsigned int kheap_virtual_address(unsigned int physical_address)
{
	uint32 ret = 0;
	for(uint32 i = KERNEL_HEAP_START ; i< Lastlocated ; i+=PAGE_SIZE)
	{
		uint32* ptr;
		struct Frame_Info* ptr_frame = NULL;
		ptr_frame = get_frame_info(ptr_page_directory, (void*)i, &ptr);
		if(ptr_frame != NULL)
		{
			uint32 pa = to_physical_address(ptr_frame);
			if(pa == physical_address)
			{
				ret = i;
				break;
			}
		}
	}
	return ret;
}
unsigned int kheap_physical_address(unsigned int virtual_address)
{
	uint32* PT_ptr = NULL;
	get_page_table(ptr_page_directory, (void*)virtual_address, &PT_ptr);
	uint32 z  = PT_ptr[PTX(virtual_address)] >> 12;
	uint32 res = z * PAGE_SIZE;
	return res;
}

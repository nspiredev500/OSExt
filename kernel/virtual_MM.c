#include "kernel.h"


void* const virtual_base_address = (void* const) 0xe0000000;


static struct address_space kernel_space;
static bool kernel_space_initialized = false;





// will be mapped when a user requests a read from a new page, it contains only zeros
static void* null_page = NULL;



void initializeKernelSpace()
{
	if (kernel_space_initialized)
	{
		return;
	}
	
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	
	kernel_space.tt = tt;
	
	
	kernel_space.cpts = NULL;
	kernel_space.cptds = NULL;
	
	initSlabAllocator();
	
	
	
	// map the first sections where the interrupt vectors are as user mode no access
	tt[0] = newSD(0,0,0,0b01,0xa4000000);
	
	
	
	null_page = usePage();
	if (null_page == NULL)
	{
		panic("no page for the null-page!\n");
	}
	k_memset(null_page,0,SMALL_PAGE_SIZE);
	
	
	uint32_t *init_pds = get_init_pds();
	uint32_t kernel_size = (uint32_t) ((&_EXEC_SIZE)-(&_EXEC_START));
	uint32_t sections = (kernel_size/SECTION_SIZE)+1;
	uint32_t section = ((uint32_t) virtual_base_address);
	for (uint32_t i = 0;i<sections;i++)
	{
		DEBUGPRINTF_3("migrating section 0x%x\n",section+SECTION_SIZE*i)
		migrateKernelCPT(section+SECTION_SIZE*i,init_pds+(256*i),256);
	}
	
	
	
	kernel_space_initialized = true;
}


void changeAddressSpace(struct address_space *space)
{
	register uint32_t tt asm("r0") = (uint32_t) space->tt;
	asm volatile("mcr p15,0, r0, c2, c0, 0"::"r" (tt));
	invalidate_TLB();
}

struct address_space* createAddressSpace()
{
	struct address_space *space = requestAddressSpace();
	space->tt = requestTranslationTable();
	space->cptds = NULL;
	space->cpts = NULL;
	
	// map the kernel pages and the first page, because the interrupt vectors are there
	space->tt[0] = kernel_space.tt[0];
	
	uint32_t i = 0;
	LinkedList *cptd = NULL;
	while ((cptd = getLinkedListEntry(&kernel_space.cptds,i)) != NULL)
	{
		space->tt[((uint32_t) (cptd->data))>>20] = kernel_space.tt[((uint32_t) (cptd->data))>>20];
		i++;
	}
	
	return space;
}
// you should switch out of the address space before destroying it
void destroyAddressSpace(struct address_space *space)
{
	uint32_t i = 0;
	LinkedList *cpt = NULL;
	while ((cpt = getLinkedListEntry(&space->cptds,i)) != NULL)
	{
		cpt = getLinkedListEntry(&space->cpts,i);
		freePagesFromCoarsePageTable(cpt->data);
		freeCPT(cpt->data);
		i++;
	}
	destroyLinkedList(&space->cpts);
	destroyLinkedList(&space->cptds);
	freeTranslationTable(space->tt);
}




// only used to initialize the memory allocator and the virtual memory manager
void migrateKernelCPT(uint32_t section,uint32_t *migrate_cpt,uint32_t pages)
{
	if (pages > SECTION_SIZE/SMALL_PAGE_SIZE)
	{
		pages = SECTION_SIZE/SMALL_PAGE_SIZE;
	}
	section = section & (~ 0xfffff);
	
	
	DEBUGPRINTF_3("migrating 0x%x pages from section 0x%x from page table 0x%x",pages,section,migrate_cpt)
	
	LinkedList *cptd = requestLinkedListEntry();
	cptd->data = (void*) section;
	addLinkedListEntry(&kernel_space.cptds,cptd);
	
	LinkedList *cpt = requestLinkedListEntry();
	cpt->data = requestCPT();
	DEBUGPRINTF_3(" to page table 0x%x\n",getPhysicalAddress(kernel_space.tt,cpt->data))
	k_memset(cpt->data,0,1024);
	addLinkedListEntry(&kernel_space.cpts,cpt);
	
	k_memcpy(cpt->data,migrate_cpt,pages*sizeof(uint32_t));
	
	
	kernel_space.tt[section >> 20] = newCPTD(0,(uint32_t) getPhysicalAddress(kernel_space.tt,cpt->data));
	
	clear_caches();
	invalidate_TLB();
}




void addVirtualKernelPage(void* page, void* virtual_address)
{
	DEBUGPRINTF_3("mapping 0x%x to 0x%x",page,virtual_address)
	uint32_t section = ((uint32_t) page) & (~ 0xfffff);
	uint32_t index = 0;
	LinkedList *sec = searchLinkedListEntry(&kernel_space.cptds,(void*) section,&index);
	if (sec == NULL)
	{
		DEBUGPRINTF_3("adding coarse page table descriptor for section 0x%x\n",section)
		LinkedList *cptd = requestLinkedListEntry();
		cptd->data = (void*) section;
		addLinkedListEntry(&kernel_space.cptds,cptd);
		
		LinkedList *cpt = requestLinkedListEntry();
		cpt->data = requestCPT();
		k_memset(cpt->data,0,1024);
		addLinkedListEntry(&kernel_space.cpts,cpt);
		
		kernel_space.tt[section >> 20] = newCPTD(0,(uint32_t) getPhysicalAddress(kernel_space.tt,cpt->data));
		
		uint32_t table_index = (uint32_t) (virtual_address-section);
		table_index = table_index / SMALL_PAGE_SIZE;
		uint32_t *table = cpt->data;
		table[table_index] = newSPD(1,1,0b01010101,(uint32_t) page);
		
		
		
		// TODO if adding a new coarse page table descriptor, add it to all virtual address spaces
		
		
	}
	else
	{
		LinkedList *cpt = getLinkedListEntry(&kernel_space.cpts,index);
		if (cpt == NULL)
		{
			panic("no corresponding coarse page table for descriptor!\n");
		}
		uint32_t *table = cpt->data;
		uint32_t table_index = ((uint32_t) (virtual_address-section));
		table_index = table_index / SMALL_PAGE_SIZE;
		table[table_index] = newSPD(1,1,0b01010101,(uint32_t) page);
	}
	clear_caches();
	invalidate_TLB();
}



uint32_t* getKernel_TT_Base()
{
	return kernel_space.tt;
}

void* getPhysicalAddress(uint32_t* space,void* address)
{
	uint32_t adr = (uint32_t) address;
	uint32_t descriptor = space[adr >> 20];
	if ((descriptor  & 0b11) == 0b0) // invalid descriptor
	{
		DEBUGPRINTF_3("invalid descriptor: 0x%x\n",adr >> 20)
		return NULL;
	}
	if ((descriptor  & 0b11) == 0b10)
	{
		uint32_t section_base =  ((descriptor >> 20) << 20);
		DEBUGPRINTF_3("phys. address: 0x%x\n",(section_base + ((adr << 12) >> 12)))
		return (void*) (section_base + ((adr << 12) >> 12));
	}
	if ((descriptor  & 0b11) == 0b01) // coarse page table
	{
		DEBUGPRINTF_3("coarse page table: 0x%x\n",adr >> 20)
		uint32_t *cpt_base = (uint32_t*) (descriptor & (~ 0b1111111111));
		DEBUGPRINTF_3("coarse page table: 0x%x\n",descriptor)
		DEBUGPRINTF_3("coarse page table base: 0x%x\n",cpt_base)
		uint32_t index = ((adr & 0b11111111000000000000) >> 10)/4;
		DEBUGPRINTF_3("coarse page table index: 0x%x\n",index)
		uint32_t page_descriptor = cpt_base[index];
		
		DEBUGPRINTF_3("small page descriptor: 0x%x\n",page_descriptor)
		uint32_t page_offset = adr & 0b111111111111;
		uint32_t phys = (page_descriptor & (~ 0b111111111111)) + page_offset;
		DEBUGPRINTF_3("phys. address: 0x%x\n",phys)
		return (void*) phys;// for small pages
		// TODO for large pages
	}
	if ((descriptor  & 0b11) == 0b11) // fine page table
	{
		DEBUGPRINTF_3("fine page table: 0x%x\n",adr >> 20)
		// never used anyways
		return NULL;
	}
	DEBUGPRINTF_3("no known descriptor type: 0x%x, 0x%x\n",adr >> 20, descriptor)
	return NULL;
}



void freePagesFromCoarsePageTable(uint32_t *cpt)
{
	for (uint32_t i = 0;i<1024;i++)
	{
		uint32_t type = cpt[i] & 0b11;
		if (type == 0b1 || type == 0b11)
		{
			panic("tiny or large page found in coarse page table while freeing!\n");
		}
		if (type == 0b0)
		{
			continue;
		}
		uint32_t descriptor = cpt[i];
		void *page = (void*) (descriptor & (~ 0b111111111111));
		setPageUnused(page);
	}
}

uint32_t newCPTD(unsigned char domain,uint32_t base_address)
{
	const uint32_t cptd_template = 0b000000000000000000000000010001;
	uint32_t desc = cptd_template;
	desc = desc | ((domain & 0b1111) << 5) | (base_address & (~ 0b1111111111));
	return desc;
}

uint32_t newSD(unsigned char c,unsigned char b,unsigned char domain,unsigned char ap,uint32_t base_address)
{
	const uint32_t sd_template = 0b00000000000000000000000000010010;
	uint32_t desc = sd_template;
	desc = desc | ((c & 0b1) << 3) | ((b & 0b1) << 2) | ((domain & 0b1111) << 5) | ((ap & 0b11) << 10) | (base_address & (~ 0b11111111111111111111));
	return desc;
}


uint32_t newLPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address)
{
	const uint32_t lpd_template = 0b00000000000000000000000000000001;
	uint32_t desc = lpd_template;
	desc = desc | ((c & 0b1) << 3) | ((b & 0b1) << 2) | (ap << 4) | ((base_address & 0xffff0000));
	return desc;
}

uint32_t newSPD(unsigned char c,unsigned char b,unsigned char ap,uint32_t base_address)
{
	const uint32_t lpd_template = 0b00000000000000000000000000000010;
	uint32_t desc = lpd_template;
	desc = desc | ((c & 0b1) << 3) | ((b & 0b1) << 2) | (ap << 4) | ((base_address & 0xfffff000));
	return desc;
}













void invalidate_TLB()
{
	asm volatile("mcr p15, 0, r0, c8, c7, 0":::"r0");
}






void clear_caches()
{
	asm volatile(
	"loop: mrc p15, 0, r15, c7, c10, 3 \n" // test and clean d-cache
	"bne loop \n"
	"mcr p15, 0, r0, c7, c7, 0\n" // invalidate cache
	:::"r0");
}




void virtual_mm_self_test()
{
	
	
	
	
}





















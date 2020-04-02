#include "kernel.h"


void* const virtual_base_address = (void* const) 0xe0000000;
const void* remapped_RAM = (const void*) 0xec000000;
const void* old_RAM = (const void*) 0x10000000;

static struct address_space kernel_space;
static bool kernel_space_initialized = false;
static void* physical_kernel_tt = NULL;

uint32_t saved_tt = 0;

// will be mapped when a user requests a read from a new page, it contains only zeros
static void* null_page = NULL;
static LinkedList *address_spaces = NULL;


struct address_space* getKernelSpacePointer()
{
	return &kernel_space;
}
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
	physical_kernel_tt = tt;
	
	kernel_space.cpts = NULL;
	kernel_space.cptds = NULL;
	
	
	for (int i = 0;i<64;i++)
	{
		tt[(((uint32_t) remapped_RAM)+i*SECTION_SIZE)>>20] = newSD(1,1,0,0b01,0x10000000+i*SECTION_SIZE);
	}
	clear_caches();
	invalidate_TLB();
	
	
	initSlabAllocator();
	
	bool b = slab_allocator_self_test_post_initialization();
	if (! b)
	{
		debug_shell_println_rgb("error in slab allocator self-test         aborting",255,0,0);
		keypad_press_release_barrier();
		free_init_pds();
		return;
	}
	
	
	
	
	
	
	//debug_shell_println_rgb("kernel heap next2: 0x%x",255,0,0,getKernelHeapNextPage());
	
	
	
	
	
	
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
	uint32_t pages_left = (kernel_size/SMALL_PAGE_SIZE)+1;
	DEBUGPRINTLN_1("pages for kernel: %d",pages_left)
	for (uint32_t i = 0;i<sections;i++)
	{
		uint32_t pages = 256;
		if (i == sections-1)
			pages = pages_left % 256;
		DEBUGPRINTF_3("migrating section 0x%x\n",section+SECTION_SIZE*i)
		migrateKernelCPT(section+SECTION_SIZE*i,init_pds+(256*i),pages);
	}
	
	
	// map the translation table in the kernel heap, so it can be accessed while out of kernel space
	void* physical_tt = tt;
	//debug_shell_println_rgb("kernel heap next3: 0x%x",255,0,0,getKernelHeapNextPage());
	void* tt_page = getKernelHeapNextPage();
	setKernelHeapNextPage(getKernelHeapNextPage() + SMALL_PAGE_SIZE*5);
	addVirtualKernelPage(physical_tt,tt_page);
	addVirtualKernelPage(physical_tt+SMALL_PAGE_SIZE,tt_page+SMALL_PAGE_SIZE);
	addVirtualKernelPage(physical_tt+SMALL_PAGE_SIZE*2,tt_page+SMALL_PAGE_SIZE*2);
	addVirtualKernelPage(physical_tt+SMALL_PAGE_SIZE*3,tt_page+SMALL_PAGE_SIZE*3);
	
	//asm(".long 0xE1212374"); // bkpt
	kernel_space.tt = tt_page;
	
	
	
	// remapping io
	
	tt[(0xe9000000)>>20] = newSD(0,0,0,0b01,0x90000000);
	tt[(0xe9000000+SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xb0000000);
	tt[(0xe9000000+2*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xc0000000);
	tt[(0xe9000000+3*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xdc000000);
	tt[(0xe9000000+4*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xc4000000);
	tt[(0xe9000000+5*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0x8ff00000);
	
	remappLCD((void*) (0xe9000000+2*SECTION_SIZE));
	remappUART((void*) (0xe9000000+0x00020000));
	
	
	
	register uint32_t domain_register asm("r1");
	asm volatile("mrc p15, 0, r1, c3, c0, 0":"=r" (domain_register)::);
	domain_register &= ~0b1100; // set domain 1 to no access
	asm volatile("mcr p15, 0, r1, c3, c0, 0"::"r" (domain_register):);
	
	kernel_space_initialized = true;
}


void changeAddressSpace(struct address_space *space)
{
	//asm(".long 0xE1212374"); // bkpt
	register uint32_t tt asm("r0") = (uint32_t) getPhysicalAddress(&kernel_space,space->tt);
	//asm(".long 0xE1212374"); // bkpt
	asm volatile("mcr p15,0, r0, c2, c0, 0"::"r" (tt));
	invalidate_TLB();
}

void intoKernelSpace()
{
	//register uint32_t tt asm("r0") = (uint32_t) kernel_space.tt;
	register uint32_t tt asm("r0") = (uint32_t) getPhysicalAddress(&kernel_space,kernel_space.tt);
	asm volatile("mcr p15,0, r0, c2, c0, 0"::"r" (tt));
	invalidate_TLB();
}

void intoKernelSpaceSaveAddressSpace()
{
	//register uint32_t tt asm("r0") = (uint32_t) kernel_space.tt;
	
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	
	if (tt_base == (uint32_t) kernel_space.tt)
	{
		return;
	}
	if (saved_tt != 0)
	{
		panic("saved_tt already used!");
	}
	saved_tt = tt_base;
	
	
	
	
	register uint32_t tt asm("r0") = (uint32_t) getPhysicalAddress(&kernel_space,kernel_space.tt);
	asm volatile("mcr p15,0, r0, c2, c0, 0"::"r" (tt));
	invalidate_TLB();
}


void restoreAddressSpace()
{
	register uint32_t tt asm("r0");
	
	if (saved_tt == 0)
	{
		return;
	}
	
	tt = saved_tt;
	saved_tt = 0;
	asm volatile("mcr p15,0, r0, c2, c0, 0"::"r" (tt));
	invalidate_TLB();
}

struct address_space* createAddressSpace()
{
	struct address_space *space = requestAddressSpace();
	space->tt = requestTranslationTable();
	DEBUGPRINTLN_1("new space: 0x%x",space)
	DEBUGPRINTLN_1("new space tt: 0x%x",space->tt)
	DEBUGPRINTLN_1("new space tt physical address: 0x%x",getPhysicalAddress(&kernel_space,space->tt))
	space->cptds = NULL;
	space->cpts = NULL;
	
	
	uint32_t *tt = space->tt;
	DEBUGPRINTLN_1("new space tt: 0x%x",space->tt)
	
	
	k_memset(space->tt,0,1024*16);
	
	
	
	
	
	
	DEBUGPRINTLN_1("new space tt: 0x%x",space->tt)
	
	// map the RAM into the address space for the kernel to use
	for (int i = 0;i<64;i++)
	{
		tt[(((uint32_t) remapped_RAM)+i*SECTION_SIZE)>>20] = newSD(1,1,0,0b01,0x10000000+i*SECTION_SIZE);
	}
	
	// remapping io
	
	tt[(0xe9000000)>>20] = newSD(0,0,0,0b01,0x90000000);
	tt[(0xe9000000+SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xb0000000);
	tt[(0xe9000000+2*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xc0000000);
	tt[(0xe9000000+3*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xdc000000);
	tt[(0xe9000000+4*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0xc4000000);
	tt[(0xe9000000+5*SECTION_SIZE)>>20] = newSD(0,0,0,0b01,0x8ff00000);
	
	
	DEBUGPRINTLN_1("new space tt: 0x%x",space->tt)
	
	uint32_t i = 0;
	LinkedList *cptd = NULL;
	while ((cptd = getLinkedListEntry(&kernel_space.cptds,i)) != NULL)
	{
		DEBUGPRINTLN_1("mapping kernel section 0x%x to new address space",cptd->data)
		(space->tt[((uint32_t) (cptd->data))>>20]) = kernel_space.tt[((uint32_t) (cptd->data))>>20];
		i++;
	}
	
	DEBUGPRINTLN_1("new space tt: 0x%x",space->tt)
	LinkedList *l = requestLinkedListEntry();
	l->data = space;
	addLinkedListEntry(&address_spaces,l);
	return space;
}
// you should switch out of the address space before destroying it
void destroyAddressSpace(struct address_space *space)
{
	uint32_t index = 0;
	LinkedList *entry = searchLinkedListEntry(&address_spaces,space,&index);
	if (entry != NULL)
	{
		removeLinkedListEntry(&address_spaces,entry);
		freeLinkedListEntry(entry);
	}
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





// IMPORTANT make sure to have enough linkedlist entries and a cpt entry
// only used to initialize the memory allocator and the virtual memory manager
void migrateKernelCPT(uint32_t section,uint32_t *migrate_cpt,uint32_t pages)
{
	if (pages > SECTION_SIZE/SMALL_PAGE_SIZE)
	{
		pages = SECTION_SIZE/SMALL_PAGE_SIZE;
	}
	section = section & (~ 0xfffff);
	
	uint32_t index = 0;
	LinkedList* already_cptd = searchLinkedListEntry(&kernel_space.cptds,(void*) section,&index);
	if (already_cptd != NULL)
	{
		removeLinkedListEntry(&kernel_space.cptds,already_cptd);
		LinkedList* already_cpt = getLinkedListEntry(&kernel_space.cpts,index);
		removeLinkedListEntry(&kernel_space.cpts,already_cpt);
		freeCPT(already_cpt->data);
		freeLinkedListEntry(already_cpt);
		freeLinkedListEntry(already_cptd);
	}
	
	
	DEBUGPRINTF_3("migrating %d pages from section 0x%x from page table 0x%x",pages,section,migrate_cpt)
	
	
	//DEBUGPRINTLN_3("requesting linkedlist entry")
	LinkedList *cptd = requestLinkedListEntry();
	//DEBUGPRINTLN_3("writing section to linkedlist entry")
	cptd->data = (void*) section;
	
	//DEBUGPRINTLN_3("requesting linkedlist entry")
	LinkedList *cpt = requestLinkedListEntry();
	//DEBUGPRINTLN_3("requesting cptd for entry")
	cpt->data = requestCPT();
	DEBUGPRINTF_3(" to page table 0x%x\n",getPhysicalAddress(&kernel_space,cpt->data))
	DEBUGPRINTLN_3("memsetting page table")
	k_memset(cpt->data,0,1024);
	DEBUGPRINTLN_3("adding linkedlist entry to kernel cpts")
	addLinkedListEntry(&kernel_space.cpts,cpt);
	DEBUGPRINTLN_3("adding linkedlist entry to kernel cptds")
	addLinkedListEntry(&kernel_space.cptds,cptd);
	DEBUGPRINTLN_3("copying small page descriptors")
	k_memcpy(cpt->data,migrate_cpt,pages*sizeof(uint32_t));
	
	DEBUGPRINTLN_3("setting the coarse page table descriptor")
	kernel_space.tt[section >> 20] = newCPTD(0,(uint32_t) getPhysicalAddress(&kernel_space,cpt->data));
	DEBUGPRINTLN_3("clearing the caches")
	clear_caches();
	DEBUGPRINTLN_3("invalidating tlb")
	invalidate_TLB();
}


uint32_t* getKernelCPT(void* address)
{
	uint32_t section = (uint32_t) address & (~ 0xfffff);
	uint32_t index = 0;
	LinkedList* cptd = searchLinkedListEntry(&kernel_space.cptds,(void*) section,&index);
	if (cptd == NULL)
	{
		return NULL;
	}
	LinkedList* cpt = getLinkedListEntry(&kernel_space.cpts,index);
	return cpt->data;
}



void addVirtualKernelPage(void* page, void* virtual_address)
{
	//DEBUGPRINTF_3("mapping 0x%x to 0x%x\n",page,virtual_address)
	
	uint32_t section = ((uint32_t) virtual_address) & (~ 0xfffff);
	uint32_t index = 0;
	LinkedList *sec = searchLinkedListEntry(&kernel_space.cptds,(void*) section,&index);
	if (sec == NULL)
	{
		//DEBUGPRINTF_3("adding coarse page table descriptor for section 0x%x\n",section)
		
		ensureCPTCapacity();
		ensureLinkedListCapacity();
		
		LinkedList *cptd = requestLinkedListEntry();
		cptd->data = (void*) section;
		
		LinkedList *cpt = requestLinkedListEntry();
		cpt->data = requestCPT();
		k_memset(cpt->data,0,1024);
		addLinkedListEntry(&kernel_space.cpts,cpt);
		addLinkedListEntry(&kernel_space.cptds,cptd);
		
		
		kernel_space.tt[section >> 20] = newCPTD(0,(uint32_t) getPhysicalAddress(&kernel_space,cpt->data));
		
		uint32_t table_index = (uint32_t) (virtual_address-section);
		table_index = table_index / SMALL_PAGE_SIZE;
		uint32_t *table = cpt->data;
		table[table_index] = newSPD(1,1,0b01010101,(uint32_t) page);
		
		// adding the page table to all address spaces
		LinkedList *next = address_spaces;
		while (next != NULL)
		{
			struct address_space *space = (struct address_space*) next->data;
			space->tt[section >> 20] = kernel_space.tt[section >> 20];
			next = next->next;
		}
		
		
		
	}
	else
	{
		LinkedList *cpt = getLinkedListEntry(&kernel_space.cpts,index);
		if (cpt == NULL)
		{
			DEBUGPRINTF_3("index: %d\n",index)
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





void addVirtualPage(struct address_space *space,void* page, void* virtual_address)
{
	DEBUGPRINTF_3("mapping 0x%x to 0x%x in userspace\n",page,virtual_address)
	uint32_t section = ((uint32_t) virtual_address) & (~ 0xfffff);
	uint32_t index = 0;
	LinkedList *sec = searchLinkedListEntry(&space->cptds,(void*) section,&index);
	if (sec == NULL)
	{
		DEBUGPRINTF_3("adding coarse page table descriptor for section 0x%x\n",section)
		
		ensureCPTCapacity();
		ensureLinkedListCapacity();
		
		
		LinkedList *cptd = requestLinkedListEntry();
		DEBUGPRINTF_3("linkedlist: 0x%x\n",cptd)
		cptd->data = (void*) section;
		addLinkedListEntry(&space->cptds,cptd);
		LinkedList *cpt = requestLinkedListEntry();
		cpt->data = requestCPT();
		k_memset(cpt->data,0,1024);
		addLinkedListEntry(&space->cpts,cpt);
		
		space->tt[section >> 20] = newCPTD(0,(uint32_t) getPhysicalAddress(&kernel_space,cpt->data));
		
		
		uint32_t table_index = (uint32_t) (virtual_address-section);
		table_index = table_index / SMALL_PAGE_SIZE;
		uint32_t *table = cpt->data;
		table[table_index] = newSPD(1,1,0b11111111,(uint32_t) page);
		
		
		
	}
	else
	{
		LinkedList *cpt = getLinkedListEntry(&space->cpts,index);
		if (cpt == NULL)
		{
			panic("no corresponding coarse page table for descriptor!\n");
		}
		uint32_t *table = cpt->data;
		uint32_t table_index = ((uint32_t) (virtual_address-section));
		table_index = table_index / SMALL_PAGE_SIZE;
		table[table_index] = newSPD(1,1,0b11111111,(uint32_t) page);
	}
	clear_caches();
	invalidate_TLB();
}



uint32_t* getKernel_TT_Base()
{
	return kernel_space.tt;
}


void* getKernelPhysicalAddress(void* address)
{
	return getPhysicalAddress(&kernel_space,address);
}

void* getPhysicalAddress(struct address_space *space,void* address)
{
	uint32_t adr = (uint32_t) address;
	
	
	/*
	// first search in the address space struct, then in the translation table itself
	uint32_t index = -1;
	LinkedList *cptds_entry = searchLinkedListEntry(&space->cptds, (void*) ((adr >> 20) << 20),&index);
	if (cptds_entry != NULL)
	{
		LinkedList *cpt_linkedlist = getLinkedListEntry(&space->cpts,index);
		if (cpt_linkedlist != NULL)
		{
			uint32_t cpt_index = ((adr & 0b11111111000000000000) >> 10)/4;
			uint32_t *cpt = cpt_linkedlist->data;
			uint32_t desc = cpt[cpt_index];
			if ((desc & 0b11) == 0b10)
			{
				uint32_t page_offset = adr & 0b111111111111;
				uint32_t phys = (desc & (~ 0b111111111111)) + page_offset;
				return (void*) phys;
			}
			else
			{
				DEBUGPRINTF_3("not a small page descriptor!\n",address)
				//asm(".long 0xE1212374"); // bkpt
			}
		}
		else
		{
			DEBUGPRINTF_3("no corresponding cpt entry found!\n",address)
			//asm(".long 0xE1212374"); // bkpt
		}
	}
	else
	{
		DEBUGPRINTF_3("no coarse page table for address found!\n",address)
		//asm(".long 0xE1212374"); // bkpt
	}
	*/
	
	
	
	//DEBUGPRINTF_3("resolving: 0x%x\n",address)
	uint32_t descriptor = space->tt[adr >> 20];
	if ((descriptor  & 0b11) == 0b0) // invalid descriptor
	{
		//DEBUGPRINTF_3("invalid descriptor: 0x%x\n",adr >> 20)
		return NULL;
	}
	if ((descriptor  & 0b11) == 0b10)
	{
		//DEBUGPRINTF_3("section descriptor: 0x%x\n",descriptor)
		uint32_t section_base =  ((descriptor >> 20) << 20);
		//DEBUGPRINTF_3("phys. address: 0x%x\n",(section_base + ((adr << 12) >> 12)))
		return (void*) (section_base + ((adr << 12) >> 12));
	}
	if ((descriptor  & 0b11) == 0b01) // coarse page table
	{
		//DEBUGPRINTF_3("coarse page table: 0x%x\n",adr >> 20)
		uint32_t *cpt_base = (uint32_t*) (descriptor & (~ 0b1111111111));
		//DEBUGPRINTF_3("coarse page table: 0x%x\n",descriptor)
		//DEBUGPRINTF_3("coarse page table base: 0x%x\n",cpt_base)
		uint32_t index = ((adr & 0b11111111000000000000) >> 10)/4;
		//DEBUGPRINTF_3("coarse page table index: 0x%x\n",index)
		uint32_t page_descriptor =  ((uint32_t*) (((void*) cpt_base) - old_RAM + remapped_RAM))[index];
		if ((page_descriptor & 0b11) == 0b1)
		{
			//DEBUGPRINTF_3("large page descriptor\n")
			uint32_t page_offset = adr & 0b1111111111111111;
			uint32_t phys = (page_descriptor & (~ 0b1111111111111111)) + page_offset;
			//DEBUGPRINTF_3("phys. address: 0x%x\n",phys)
			return (void*) phys;
			return NULL;
		}
		if ((page_descriptor & 0b11) == 0b10)
		{
			//DEBUGPRINTF_3("small page descriptor: 0x%x\n",page_descriptor)
			uint32_t page_offset = adr & 0b111111111111;
			uint32_t phys = (page_descriptor & (~ 0b111111111111)) + page_offset;
			//DEBUGPRINTF_3("phys. address: 0x%x\n",phys)
			return (void*) phys;// for small pages
		}
		return NULL;
	}
	if ((descriptor  & 0b11) == 0b11) // fine page table
	{
		//DEBUGPRINTF_3("fine page table: 0x%x\n",adr >> 20)
		// never used anyways
		return NULL;
	}
	//DEBUGPRINTF_3("no known descriptor type: 0x%x, 0x%x\n",adr >> 20, descriptor)
	return NULL;
}



void freePagesFromCoarsePageTable(uint32_t *cpt)
{
	for (uint32_t i = 0;i<1024;i++)
	{
		uint32_t type = cpt[i] & 0b11;
		if (type == 0b1 || type == 0b11)
		{
			DEBUGPRINTF_3("address: 0x%x\n",cpt+i)
			asm(".long 0xE1212374"); // bkpt
			panic("tiny or large page found in coarse page table while freeing coarse page table!\n");
		}
		if (type == 0b0)
		{
			continue;
		}
		uint32_t descriptor = cpt[i];
		void *page = (void*) (descriptor & (~ 0b111111111111));
		setPageUsedBit(page,false);
	}
}




bool virtual_mm_self_test()
{
	// TODO tests for new(descriptor)
	/*
	register uint32_t tt_base asm("r0");
	asm volatile("mrc p15, 0, r0, c2, c0, 0":"=r" (tt_base));
	
	tt_base = tt_base & (~ 0x3ff); // discard the first 14 bits, because they don't matter
	uint32_t *tt = (uint32_t*) tt_base;
	if (getPhysicalAddress(tt,(void*) 0x0) != (void*) 0xa4000000)
	{
		DEBUGPRINTLN_1("getPhysicalAddress not working!")
		return false;
	}
	if (getPhysicalAddress(tt,(void*) 0x12400000) != (void*) 0x12400000)
	{
		DEBUGPRINTLN_1("getPhysicalAddress not working!")
		return false;
	}
	*/
	// currently not working for lage pages
	/*
	if (getPhysicalAddress(tt,(void*) 0x13e09000) != (void*) 0x13e00000)
	{
		DEBUGPRINTLN_1("getPhysicalAddress not working!")
		return false;
	}
	*/
	// TODO test for getPhysicalAddress with small pages
	
	
	
	
	
	
	return true;
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
	"clear_cache_loop%=: mrc p15, 0, r15, c7, c10, 3 \n" // test and clean d-cache
	"bne clear_cache_loop%= \n"
	"mcr p15, 0, r0, c7, c7, 0 \n" // invalidate cache
	"mov r0, #0 \n"
	"mcr p15, 0, r0, c7, c10, 4 \n" // drain write buffer
	:::"r0");
}







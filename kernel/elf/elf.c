#include "../kernel.h"






// returns true if it's a valid ELF file
bool elf_check_header(struct elf_header *h)
{
	//DEBUGPRINTLN_1("Magic: 0x%x",*((uint32_t*) h->magic))
	if (h->magic[0] != 0x7f || h->magic[1] != 0x45 || h->magic[2] != 0x4c || h->magic[3] != 0x46) // 0x7f ELF
	{
		return false;
	}
	//DEBUGPRINTLN_1("wordwidth: %d",h->wordwidth)
	if (h->wordwidth != 1) // must be 32 bit
	{
		return false;
	}
	//DEBUGPRINTLN_1("endianness: %d",h->endianness)
	if (h->endianness != 1) // must be little endian
	{
		return false;
	}
	//DEBUGPRINTLN_1("instruction set: %d",h->instruction_set)
	if (h->instruction_set != 0x28) // arm
	{
		return false;
	}
	return true;
}



void elf_read_header(NUC_FILE * f,struct elf_header *h)
{
	nuc_fseek(f,0,SEEK_SET);
	
	char header[52]; // the ELF header is 52 bytes for 32 bit
	nuc_fread(header,1,52,f);
	k_memcpy(&h->magic,header,4);
	h->wordwidth = header[4];
	h->endianness = header[5];
	h->elf_h_version = header[6];
	h->abi = header[7];
	k_memcpy(&h->type,header+0x10,2);
	k_memcpy(&h->instruction_set,header+0x12,2);
	k_memcpy(&h->elf_version,header+0x14,4);
	k_memcpy(&h->entry,header+0x18,4);
	k_memcpy(&h->prog_table,header+0x1c,4);
	k_memcpy(&h->sect_table,header+0x20,4);
	k_memcpy(&h->header_size,header+0x28,2);
	k_memcpy(&h->prog_size,header+0x2a,2);
	k_memcpy(&h->prog_count,header+0x2c,2);
	k_memcpy(&h->sect_size,header+0x2e,2);
	k_memcpy(&h->sect_count,header+0x30,2);
	k_memcpy(&h->sect_strtab,header+0x32,2);
}

void elf_read_header_mem(char* header,struct elf_header *h)
{
	k_memcpy(&h->magic,header,4);
	h->wordwidth = header[4];
	h->endianness = header[5];
	h->elf_h_version = header[6];
	h->abi = header[7];
	k_memcpy(&h->type,header+0x10,2);
	k_memcpy(&h->instruction_set,header+0x12,2);
	k_memcpy(&h->elf_version,header+0x14,4);
	k_memcpy(&h->entry,header+0x18,4);
	k_memcpy(&h->prog_table,header+0x1c,4);
	k_memcpy(&h->sect_table,header+0x20,4);
	k_memcpy(&h->header_size,header+0x28,2);
	k_memcpy(&h->prog_size,header+0x2a,2);
	k_memcpy(&h->prog_count,header+0x2c,2);
	k_memcpy(&h->sect_size,header+0x2e,2);
	k_memcpy(&h->sect_count,header+0x30,2);
	k_memcpy(&h->sect_strtab,header+0x32,2);
}




struct elf_desc* elf_load_file(NUC_FILE * f, uint32_t size)
{
	struct elf_desc *elf = kmalloc(sizeof(struct elf_desc));
	if (elf == NULL)
	{
		return NULL;
	}
	elf->pages = (size/SMALL_PAGE_SIZE)+1;
	elf->start = useConsecutivePages((size/SMALL_PAGE_SIZE)+1,0);
	if (elf->start == NULL)
	{
		kfree_hint(elf,sizeof(struct elf_desc));
		return NULL;
	}
	elf->image = NULL;
	elf->image_pages = 0;
	
	nuc_fseek(f,0,SEEK_SET);
	
	nuc_fread(elf->start,1,size,f);
	return elf;
}


void* elf_entry(struct elf_desc* elf)
{
	if (elf == NULL)
	{
		return NULL;
	}
	if (elf->image == NULL)
	{
		return NULL;
	}
	if (elf->start == NULL)
	{
		return NULL;
	}
	struct elf_header h;
	elf_read_header_mem(elf->start,&h);
	if (! elf_check_header(&h))
	{
		DEBUGPRINTLN_1("invalid header!");
		return NULL;
	}
	if (h.entry >= elf->image_pages*SMALL_PAGE_SIZE) // out of bounds
	{
		DEBUGPRINTLN_1("entry point out of bounds: e: 0x%x max: 0x%x",h.entry,elf->image_pages*SMALL_PAGE_SIZE);
		return NULL;
	}
	return elf->image + h.entry;
}



void elf_alloc_image(struct elf_desc *elf)
{
	if (elf == NULL)
	{
		return;
	}
	if (elf->start == NULL)
	{
		return;
	}
	if (elf->image != NULL)
	{
		return;
	}
	struct elf_header h;
	elf_read_header_mem(elf->start,&h);
	if (! elf_check_header(&h))
	{
		DEBUGPRINTLN_1("invalid header!");
		return;
	}
	void* prog_pointer = elf->start + h.prog_table;
	uint32_t headers = h.prog_count;
	DEBUGPRINTLN_1("prog headers: %d",headers);
	DEBUGPRINTLN_1("header table offset: %d",h.prog_table);
	for (uint32_t i = 0;i<headers;i++)
	{
		struct elf_prog_header prog_h;
		if (prog_pointer < elf->start || prog_pointer + sizeof(struct elf_prog_header) >= elf->start + (elf->pages*SMALL_PAGE_SIZE))
		{
			DEBUGPRINTLN_1("program header out of bounds!");
			return;
		}
		DEBUGPRINTLN_1("%d",*((uint32_t*)prog_pointer));
		k_memcpy(&prog_h,prog_pointer,sizeof(struct elf_prog_header));
		DEBUGPRINTLN_1("header type: %d",prog_h.type);
		DEBUGPRINTLN_1("offset: %d",prog_h.offset);
		if (prog_h.type == 1)
		{
			elf->image = useConsecutivePages((prog_h.memsz/SMALL_PAGE_SIZE)+1,0);
			if (elf->image == NULL)
			{
				DEBUGPRINTLN_1("not enough pages!");
				return;
			}
			elf->image_pages = (prog_h.memsz/SMALL_PAGE_SIZE)+1;
			k_memset(elf->image,0,elf->image_pages*SMALL_PAGE_SIZE);
			return;
		}
		prog_pointer += h.prog_size;
	}
}


void elf_assemble_image(struct elf_desc *elf)
{
	if (elf == NULL)
	{
		return;
	}
	if (elf->start == NULL)
	{
		return;
	}
	if (elf->image == NULL)
	{
		return;
	}
	struct elf_header h;
	elf_read_header_mem(elf->start,&h);
	if (! elf_check_header(&h))
	{
		DEBUGPRINTLN_1("invalid header!");
		return;
	}
	void* prog_pointer = elf->start + h.prog_table;
	uint32_t headers = h.prog_count;
	DEBUGPRINTLN_1("prog headers: %d",headers);
	DEBUGPRINTLN_1("header table offset: %d",h.prog_table);
	for (uint32_t i = 0;i<headers;i++)
	{
		struct elf_prog_header prog_h;
		if (prog_pointer < elf->start || prog_pointer + sizeof(struct elf_prog_header) >= elf->start + (elf->pages*SMALL_PAGE_SIZE))
		{
			DEBUGPRINTLN_1("program header out of bounds!");
			return;
		}
		DEBUGPRINTLN_1("%d",*((uint32_t*)prog_pointer));
		k_memcpy(&prog_h,prog_pointer,sizeof(struct elf_prog_header));
		DEBUGPRINTLN_1("header type: %d",prog_h.type);
		DEBUGPRINTLN_1("offset: %d",prog_h.offset);
		if (prog_h.type == 1)
		{
			if (prog_h.offset > elf->pages*SMALL_PAGE_SIZE || prog_h.offset + prog_h.filesz > elf->pages*SMALL_PAGE_SIZE)
			{
				DEBUGPRINTLN_1("image offset out of bounds!");
				return;
			}
			DEBUGPRINTLN_1("size in file: %d",prog_h.filesz);
			
			k_memcpy(elf->image,elf->start+prog_h.offset,prog_h.filesz);
			return;
		}
		prog_pointer += h.prog_size;
	}
}

// returns a pointer to a copy of the section's data, or NULL if an error occurred.
//sect_pages will be set to the number of pages the copy takes in memory
void* elf_copy_section(struct elf_desc *elf,const char* searched_name,uint32_t *sect_pages)
{
	if (elf == NULL)
	{
		return NULL;
	}
	if (elf->start == NULL)
	{
		return NULL;
	}
	if (sect_pages == NULL) // would be no way to free it then
	{
		return NULL;
	}
	struct elf_header h;
	elf_read_header_mem(elf->start,&h);
	if (! elf_check_header(&h))
	{
		DEBUGPRINTLN_1("invalid header!");
		return NULL;
	}
	if (h.sect_count != 0)
	{
		uint32_t sect_headers = h.sect_count;
		void* sect_header = elf->start + h.sect_table;
		struct elf_sect_header strtab;
		bool found = false;
		DEBUGPRINTLN_1("section offset: 0x%x",h.sect_table);
		
		if (h.sect_table + (h.sect_strtab * h.sect_size) + sizeof(struct elf_sect_header) > elf->pages*SMALL_PAGE_SIZE)
		{
			DEBUGPRINTLN_1("strtab header out of bounds!");
			return NULL;
		}
		k_memcpy(&strtab,elf->start + h.sect_table + h.sect_strtab * h.sect_size,sizeof(struct elf_sect_header));
		
		
		if (strtab.offset >= elf->pages*SMALL_PAGE_SIZE) // out of bounds
		{
			DEBUGPRINTLN_1("string table out of bounds!");
			return NULL;
		}
		
		found = false;
		sect_header = elf->start + h.sect_table;
		struct elf_sect_header section;
		// search for the string table, so we can look up the names of the sections to find the sections
		for (uint32_t i = 0;i<sect_headers;i++)
		{
			if (sect_header < elf->start || sect_header + sizeof(struct elf_sect_header) >= elf->start + (elf->pages*SMALL_PAGE_SIZE)) // out of bounds
			{
				return NULL;
			}
			k_memcpy(&section,sect_header,sizeof(struct elf_sect_header));
			char *name = elf->start + strtab.offset + section.name;
			if ((void*) name < elf->start || (void*)  name >= elf->start + (elf->pages*SMALL_PAGE_SIZE))
			{
				return NULL;
			}
			uint32_t maxlen = (uint32_t) ((elf->start + elf->pages*SMALL_PAGE_SIZE) - (void*) name);
			if (k_strcmp(name,searched_name,maxlen-1) == 0 && k_strlen(name,maxlen-1) == k_strlen(searched_name,100))
			{
				found = true;
				break;
			}
			sect_header += h.sect_size;
		}
		if (! found)
		{
			return NULL;
		}
		if (section.offset + section.size > elf->pages*SMALL_PAGE_SIZE)
		{
			return NULL;
		}
		*sect_pages = (section.size/SMALL_PAGE_SIZE)+1;
		void* copy = useConsecutivePages(*sect_pages,0);
		if (copy == NULL)
		{
			*sect_pages = 0;
			return NULL;
		}
		else
		{
			return copy;
		}
	}
	return NULL;
}



// modules run with PIC-code, but the GOT-addresses have to be fixed after loading
void elf_fix_got(struct elf_desc* elf)
{
	if (elf == NULL)
	{
		return;
	}
	if (elf->start == NULL)
	{
		return;
	}
	struct elf_header h;
	elf_read_header_mem(elf->start,&h);
	if (! elf_check_header(&h))
	{
		DEBUGPRINTLN_1("invalid header!");
		return;
	}
	DEBUGPRINTLN_1("sections: %d",h.sect_count);
	if (h.sect_count != 0)
	{
		uint32_t sect_headers = h.sect_count;
		void* sect_header = elf->start + h.sect_table;
		struct elf_sect_header strtab;
		bool found = false;
		DEBUGPRINTLN_1("section offset: 0x%x",h.sect_table);
		
		if (h.sect_table + (h.sect_strtab * h.sect_size) + sizeof(struct elf_sect_header) > elf->pages*SMALL_PAGE_SIZE)
		{
			DEBUGPRINTLN_1("strtab header out of bounds!");
			return;
		}
		k_memcpy(&strtab,elf->start + h.sect_table + h.sect_strtab * h.sect_size,sizeof(struct elf_sect_header));
		
		
		if (strtab.offset >= elf->pages*SMALL_PAGE_SIZE) // out of bounds
		{
			DEBUGPRINTLN_1("string table out of bounds!");
			return;
		}
		
		found = false;
		sect_header = elf->start + h.sect_table;
		struct elf_sect_header got;
		// search for the string table, so we can look up the names of the sections to find the GOT
		for (uint32_t i = 0;i<sect_headers;i++)
		{
			if (sect_header < elf->start || sect_header + sizeof(struct elf_sect_header) >= elf->start + (elf->pages*SMALL_PAGE_SIZE)) // out of bounds
			{
				return;
			}
			k_memcpy(&got,sect_header,sizeof(struct elf_sect_header));
			char *name = elf->start + strtab.offset + got.name;
			if ((void*) name < elf->start || (void*)  name >= elf->start + (elf->pages*SMALL_PAGE_SIZE))
			{
				return;
			}
			uint32_t maxlen = (uint32_t) ((elf->start + elf->pages*SMALL_PAGE_SIZE) - (void*) name);
			//DEBUGPRINTLN_1(name);
			//DEBUGPRINTLN_1("length: %d",k_strlen(name,maxlen-1));
			if (k_strcmp(name,".got",maxlen-1) == 0 && k_strlen(name,maxlen-1) == 4)
			{
				DEBUGPRINTLN_1("Section %d is the GOT",i);
				found = true;
				break;
			}
			sect_header += h.sect_size;
		}
		if (! found)
		{
			return;
		}
		if (got.offset >= elf->pages*SMALL_PAGE_SIZE) // out of bounds
		{
			return;
		}
		if ((got.offset % 4) != 0)
		{
			DEBUGPRINTLN_1("GOT not 4 byte aligned!");
			return;
		}
		if (got.offset+got.size >= elf->pages*SMALL_PAGE_SIZE) // out of bounds
		{
			return;
		}
		DEBUGPRINTLN_1("GOT size: %d",got.size);
		uint32_t* got_entry = elf->start+got.offset;
		DEBUGPRINTLN_1("GOT offset: 0x%x",got.offset);
		for (uint32_t i = 0;i<got.size/4;i++)
		{
			if (*got_entry != 0xffffffff)
			{
				//DEBUGPRINTLN_1("GOT entry: 0x%x, patched: 0x%x",*got_entry,*got_entry+((uint32_t) elf->image));
				*got_entry += ((uint32_t) elf->image);
			}
			got_entry++;
		}
	}
}



void elf_destroy(struct elf_desc* elf)
{
	if (elf == NULL)
	{
		return;
	}
	if (elf->start != NULL)
	{
		freeConsecutivePages(elf->start,elf->pages);
	}
	if (elf->image != NULL)
	{
		freeConsecutivePages(elf->image,elf->image_pages);
	}
	kfree_hint(elf,sizeof(struct elf_desc));
}

// destroys the file in memory and the elf_desc, but not the elf image
void elf_destroy_without_image(struct elf_desc* elf)
{
	if (elf == NULL)
	{
		return;
	}
	if (elf->start != NULL)
	{
		freeConsecutivePages(elf->start,elf->pages);
	}
	kfree_hint(elf,sizeof(struct elf_desc));
}

// to free the file and the image separately
void elf_destroy_file_mem(struct elf_desc* elf)
{
	if (elf == NULL)
	{
		return;
	}
	if (elf->start != NULL)
	{
		freeConsecutivePages(elf->start,elf->pages);
	}
	elf->start = NULL;
	elf->pages = 0;
}
void elf_destroy_image(struct elf_desc* elf)
{
	if (elf == NULL)
	{
		return;
	}
	if (elf->image != NULL)
	{
		freeConsecutivePages(elf->image,elf->image_pages);
	}
	elf->image = NULL;
	elf->image_pages = 0;
}





































































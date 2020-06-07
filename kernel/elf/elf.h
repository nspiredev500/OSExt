#ifndef ELF_H
#define ELF_H



struct elf_header {
	char magic[4];
	uint8_t wordwidth; // 1 = 32 bit, 2 = 64 bit
	uint8_t endianness; // 1 = little endian, 2 = big endian
	uint8_t elf_h_version; // elf header version
	uint8_t abi;
	uint16_t type; // 1 = relocatable, 2 = executable, 3 = shared, 4 = core
	uint16_t instruction_set;
	uint32_t elf_version;
	uint32_t entry; // entry point position
	uint32_t prog_table; // program header table
	uint32_t sect_table;
	uint16_t header_size;
	uint16_t prog_size;
	uint16_t prog_count;
	uint16_t sect_size;
	uint16_t sect_count;
	uint16_t sect_strtab; // .shstrtab section index 
};


struct elf_desc {
	uint32_t pages; // number of pages
	void *start; // start in memory
	void *image; // image from the load program header
	uint32_t image_pages;
};


struct elf_prog_header {
	uint32_t type;
	uint32_t offset;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t filesz;
	uint32_t memsz;
	uint32_t flags;
	uint32_t align;
};
/*
	ELF sections:
	.text: code
	.data: initialized data
	.bss: uninitialized data
	.rodata: read-only data
	
	.symtab: symbol table
	
	.shstrtab: string table for section names
	.strtab: string table for everything else
*/

struct elf_sect_header {
	uint32_t name; // index into the string table section
	uint32_t type;
	uint32_t flags;
	void* addr; // address in memory
	uint32_t offset; // offset from the start of the file to the section data
	uint32_t size; // size in bytes
	uint32_t link;
	uint32_t info;
	uint32_t addraling;
	uint32_t entsize; // size of an entry in the section
};


struct elf_symtab_entry {
	uint32_t name; // offset into the strtab section
	uint32_t value; // offset from the section for PIE executables
	uint32_t size;
	uint8_t info;
	uint8_t other;
	uint16_t sectindex;
};



// returns true if it's a valid ELF file
bool elf_check_header(struct elf_header *h);

void elf_read_header(NUC_FILE * f,struct elf_header *h);
void elf_read_header_mem(char* header,struct elf_header *h);

struct elf_desc* elf_load_file(NUC_FILE * f,uint32_t size);

void elf_alloc_image(struct elf_desc *elf);
void elf_assemble_image(struct elf_desc *elf);

// returns a pointer to a copy of the section's data, or NULL if an error occurred.
//sect_pages will be set to the number of pages the copy takes in memory
void* elf_copy_section(struct elf_desc *elf,const char* searched_name,uint32_t *sect_pages);




void elf_destroy(struct elf_desc* elf);
void elf_destroy_without_image(struct elf_desc* elf); // destroys the file in memory and the elf_desc, but not the elf image

// to free the file and the image separately
void elf_destroy_file_mem(struct elf_desc* elf);
void elf_destroy_image(struct elf_desc* elf);

// modules run with PIC-code, but the GOT-addresses have to be fixed after loading
void elf_fix_got(struct elf_desc* elf);


void* elf_entry(struct elf_desc* elf);





#endif
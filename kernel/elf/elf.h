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
	uint16_t sect_index; // Index in section header table with the section names
};


struct elf_desc {
	uint32_t pages; // number of pages
	void *start; // start in memory
};




// returns true if it's a valid ELF file
bool elf_check_header(struct elf_header *h);



void elf_read_header(NUC_FILE * f,struct elf_header *h);













#endif
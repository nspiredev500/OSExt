#include "../kernel.h"






// returns true if it's a valid ELF file
bool elf_check_header(struct elf_header *h)
{
	if (h->magic[0] != 0x7f || h->magic[1] != 0x45 || h->magic[2] != 0x4c || h->magic[3] != 0x46) // 0x7f ELF
	{
		return false;
	}
	if (h->wordwidth != 1) // must be 32 bit
	{
		return false;
	}
	if (h->endianness != 1) // must be little endian
	{
		return false;
	}
	if (h->instruction_set != 0x28) // arm
	{
		return false;
	}
	return true;
}



void elf_read_header(NUC_FILE * f,struct elf_header *h)
{
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
	k_memcpy(&h->sect_index,header+0x32,2);
}
















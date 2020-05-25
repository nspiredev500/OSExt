#include "../kernel.h"

/* 
	it seems the NAND-controller is at 0x8FFF1000
	and the nand itself is at 0x81000000
	
	
	
	
	commands are send by writing to (NAND base)+(command value) >> 3
	the actual data are the address bytes
	command >> 21 is how many address bytes the transfer has (<= 7)
	
	
	then reading 0x81280000 returns the word/byte at that position and advances the position by 4/1 bytes
	
	
	
	
	NAND commands:
	0x00 read0
	0xff reset
	
	
	
	
	NAND controller:
	0x0: controller configuration status (read-only)
	0x4: memory interface configuration (read-only)
	0x8: set controller configuration (write-only)
	0xc: clear controller configuration bits (write-only)
	0x10: direct command register (write-only)
	0x14: set cycles register (write-only)
	0x18: set opmode register (write-only)
	
	0x200: user config status
	0x204: (user config) setting bit 1 to a 1 enables flash writing (write-only register)
	0x300: ecc status
	0x304: ecc configuration (write-only register)
	
	0xfe0-0xfec: peripheral id registers
	0xff0-0xffc: prime cell id registers
	
	
	
	remapped_nand_ctl[0] = controller configuration status
	remapped_nand_ctl[1] = memory interface configuration status
	remapped_nand_ctl[2] = set controller configuration
	remapped_nand_ctl[3] = clear controller configuration
	remapped_nand_ctl[4] = direct command register
	remapped_nand_ctl[5] = set cycles register
	remapped_nand_ctl[6] = set opmode register
	
	remapped_nand_ctl[128] = user config status
	remapped_nand_ctl[129] = (user config) setting bit 1 to a 1 enables flash writing (write-only register)
	
	
	remapped_nand_ctl[192] = ecc status
	remapped_nand_ctl[193] = ecc configuration (write-only register)
	
	remapped_nand_ctl[0x3f8] = peripheral id 0
	remapped_nand_ctl[0x3f9] = peripheral id 1
	remapped_nand_ctl[0x3fa] = peripheral id 2
	remapped_nand_ctl[0x3fb] = peripheral id 3
	
	remapped_nand_ctl[0x3fc] = primecell id 0
	remapped_nand_ctl[0x3fd] = primecell id 1
	remapped_nand_ctl[0x3fe] = primecell id 2
	remapped_nand_ctl[0x3ff] = primecell id 3
	
*/
const uint32_t NAND_READ0 = 0x0;

static volatile uint32_t *remapped_nand_ctl = (uint32_t*)  (0xe95f1000);
static volatile uint32_t *remapped_nand = (uint32_t*)  (0xe9600000);


// NAND controller stuff

/*
// 0x10101
uint32_t nand_controller_peripheral_id()
{
	uint32_t id = 0;
	id |= remapped_nand_ctl[0x3f8] && 0xff;
	id |= (remapped_nand_ctl[0x3f9] && 0xff) << 8;
	id |= (remapped_nand_ctl[0x3fa] && 0xff) << 16;
	id |= (remapped_nand_ctl[0x3fb] && 0xff) << 24;
	return id;
}

// 0x1010101
// doesn't work in firebird
uint32_t nand_controller_prime_cell_id()
{
	uint32_t id = 0;
	id |= remapped_nand_ctl[0x3fc] && 0xff;
	id |= (remapped_nand_ctl[0x3fd] && 0xff) << 8;
	id |= (remapped_nand_ctl[0x3fe] && 0xff) << 16;
	id |= (remapped_nand_ctl[0x3ff] && 0xff) << 24;
	return id;
}










// NAND stuff


static const uint32_t read_offset = 0x280000;
static const uint32_t command_offset = 0x118000;
// address is the address bytes of the transfer
void nand_command(uint32_t command,uint32_t address)
{
	uint32_t *write_address = (uint32_t*) (((uint32_t) remapped_nand + command_offset + (command << 3)) | (4 << 21)); // can be converted directly uint32_t*, because the << 3 takes care of the alignment
	*write_address = address;
}


uint32_t nand_read_word()
{
	// 0x280000 seems to be the offset the OS uses for reading
	return remapped_nand[read_offset/4];
}

uint8_t nand_read_byte()
{
	// 0x280000 seems to be the offset the OS uses for reading
	uint8_t *nand_bytes = (uint8_t*) remapped_nand;
	return nand_bytes[read_offset];
}
*/
































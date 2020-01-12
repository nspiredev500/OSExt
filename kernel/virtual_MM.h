#ifndef VIRTUAL_MM_H
#define VIRTUAL_MM_H




// don't seem to work

// translation table descriptors


struct section_descriptor {
	uint32_t typ:2;
	uint32_t b:1;
	uint32_t c:1;
	uint32_t one:1;
	uint32_t domain:4;
	uint32_t zero:1;
	uint32_t ap:2;
	uint32_t zeros:8;
	uint32_t base_address:12;
}__attribute__((packed));


// 256 uint32_t entries
struct coarse_ptd {
	uint32_t typ:2;
	uint32_t zeros:2;
	uint32_t one:1;
	uint32_t domain:4;
	uint32_t zero:1;
	uint32_t base_address:22;
}__attribute__((packed));

// 1024 uint32_t entries
struct fine_ptd {
	uint32_t typ:2;
	uint32_t zeros:2;
	uint32_t one:1;
	uint32_t domain:4;
	uint32_t zeros2:3;
	uint32_t base_address:20;
}__attribute__((packed));


struct large_pd {
	uint32_t typ:2;
	uint32_t b:1;
	uint32_t c:1;
	uint32_t ap0:2;
	uint32_t ap1:2;
	uint32_t ap2:2;
	uint32_t ap3:2;
	uint32_t zeros:4;
	uint32_t base_address:16;
}__attribute__((packed));



struct coarse_ptd newCPD(unsigned char domain,uint32_t base_address);

struct large_pd newLPD(unsigned char c,unsigned char b,unsigned char ap,uint16_t base_address);



//uint32_t newCPTD();

//uint32_t newLPD();



void invalidate_TLB();
void clear_caches();










#endif
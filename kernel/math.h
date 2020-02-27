#ifndef MATH_H
#define MATH_H


uint32_t ceil_uint32(double d);
uint32_t k_pow(uint32_t a,uint32_t b);
uint32_t set_bits32(uint32_t b);
uint64_t set_bits64(uint32_t b);
uint32_t pow_of_2(uint32_t exp);
void* make4ByteAligned(void* p);
void* make1KAligned(void* p);

uint32_t signedtounsigned32(int32_t i);
uint64_t signedtounsigned64(int64_t i);







#endif
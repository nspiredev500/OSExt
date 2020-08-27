#include "../kernel.h"


// maximal length to prevent endless scanning for non-terminated strings
uint32_t k_strlen(const char *string,uint32_t max)
{
	uint32_t len = 0;
	for (const char *c = string;*c != '\0';c++)
	{
		if (len >= max)
		{
			return max;
		}
		len++;
	}
	return len;
}


int32_t k_strcmp(const char *s1, const char *s2, uint32_t max)
{
	int32_t count = 0;
	for (uint32_t i = 0;i<max;i++)
	{
		if (s1[i] == '\0' || s2[i] == '\0')
		{
			return count;
		}
		count += s1[i]-s2[i];
	}
	return count;
}


bool k_streq(const char *s1, const char *s2, uint32_t max)
{
	int32_t count = 0;
	if (k_strlen(s1,max) != k_strlen(s2,max))
	{
		return false;
	}
	for (uint32_t i = 0;i<max;i++)
	{
		if (s1[i] == '\0' || s2[i] == '\0')
		{
			return true;
		}
		if (s1[i] != s2[i])
		{
			return false;
		}
	}
	return true;
}

// doesn't check if the lengths are equal, so it's true if one string is the starting part of the other
bool k_streq_len(const char *s1, const char *s2, uint32_t max)
{
	int32_t count = 0;
	for (uint32_t i = 0;i<max;i++)
	{
		if (s1[i] == '\0' || s2[i] == '\0')
		{
			return true;
		}
		if (s1[i] != s2[i])
		{
			return false;
		}
	}
	return true;
}




void sprintf_safe(char *result,char *str,uint32_t length,...)
{
	va_list va;
	va_start(va,length);
	char *c = str;
	uint32_t index = 0;
	while (true)
	{
		if (index >= length-1)
			return;
		if (*c == '\0')
			break;
		if (*c == '%' && *(c+1) != '\0')
		{
			if (*(c+1) == '%')
			{
				if (! (index+1 >= length-1))
				{
					result[index] = '%';
					c += 2;
					index++;
				}
				continue;
			}
			if (*(c+1) == 's')
			{
				char* string = va_arg(va,char*);
				uint32_t len = k_strlen(string,100);
				uint32_t copy = len;
				if (copy >= index+length-1)
				{
					copy = index+length-1;
				}
				k_memcpy(result+index,string,copy);
				index += copy;
				c += 2;
				continue;
			}
			if (*(c+1) == 'd')
			{
				index += sprint_uint32_base_safe(result+index,length-index,va_arg(va,uint32_t),10);
				c += 2;
				continue;
			}
			if (*(c+1) == 'x')
			{
				index += sprint_uint32_base_safe(result+index,length-index,va_arg(va,uint32_t),16);
				c += 2;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'x')
			{
				index += sprint_uint64_base_safe(result+index,length-index,va_arg(va,uint64_t),16);
				c += 4;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'd')
			{
				index += sprint_uint64_base_safe(result+index,length-index,va_arg(va,uint64_t),10);
				c += 4;
				continue;
			}
			if (*(c+1) == 'L' && *(c+2) == 'f')
			{
				index += sprint_double_safe(result+index,length-index,va_arg(va,double),2);
				c += 3;
				continue;
			}
		}
		*(result+index) = *c;
		index++;
		c++;
	}
	va_end(va);
}

void sprintf_safe_va(char *result,char *str,uint32_t length,va_list va)
{
	char *c = str;
	uint32_t index = 0;
	while (true)
	{
		if (index >= length-1)
			return;
		if (*c == '\0')
			break;
		if (*c == '%' && *(c+1) != '\0')
		{
			if (*(c+1) == '%')
			{
				if (! (index+1 >= length-1))
				{
					result[index] = '%';
					c += 2;
					index++;
				}
				continue;
			}
			if (*(c+1) == 's')
			{
				char* string = va_arg(va,char*);
				uint32_t len = k_strlen(string,100);
				uint32_t copy = len;
				if (copy >= index+length-1)
				{
					copy = index+length-1;
				}
				k_memcpy(result+index,string,copy);
				index += copy;
				c += 2;
				continue;
			}
			if (*(c+1) == 'd')
			{
				index += sprint_uint32_base_safe(result+index,length-index,va_arg(va,uint32_t),10);
				c += 2;
				continue;
			}
			if (*(c+1) == 'x')
			{
				index += sprint_uint32_base_safe(result+index,length-index,va_arg(va,uint32_t),16);
				c += 2;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'x')
			{
				index += sprint_uint64_base_safe(result+index,length-index,va_arg(va,uint64_t),16);
				c += 4;
				continue;
			}
			if (*(c+1) == 'l' && *(c+2) == 'l' && *(c+3) == 'd')
			{
				index += sprint_uint64_base_safe(result+index,length-index,va_arg(va,uint64_t),10);
				c += 4;
				continue;
			}
			if (*(c+1) == 'L' && *(c+2) == 'f')
			{
				index += sprint_double_safe(result+index,length-index,va_arg(va,double),2);
				c += 3;
				continue;
			}
		}
		*(result+index) = *c;
		index++;
		c++;
	}
}


uint32_t sprint_double_safe(char* string,uint32_t length,double a,uint32_t roundto)
{
	uint32_t h = sprint_uint64_base_safe(string,length,(uint64_t) a,10);
	if (h >= length-1)
	{
		return h;
	}
	string[h] = '.';
	h++;
	h += sprint_uint64_base_safe(string+h,length-h,((uint64_t) (a*k_pow(10,roundto)))-((uint64_t) (((uint64_t)a)*k_pow(10,roundto))),10);
	return h;
}

uint32_t sprint_uint64_base_safe(char* string,uint32_t length,uint64_t a,uint32_t base)
{
	//max uint32_t: 4 294 967 296
	
	char buff[36];
	char buff2[36];
	k_memset(buff,'\0',34);
	
	
	int digit = 0;
	while (true)
	{
		uint64_t tmp = (a % base);
		if (tmp > 15)
			break;	// something has gone wrong
		char c = (tmp<10) ? 48+tmp:97+tmp-10;
		buff[digit] = c;
		a = a / base;
		if (a == 0)
			break;
		digit++;
	}
	uint32_t h = 0;
	for (int i = digit;i>=0;i--)
	{
		if (h >= length-1 || h >= 34)
			return h;
		string[h] = buff[i];
		h++;
	}
	return h;
}

uint32_t sprint_uint32_base_safe(char* string,uint32_t length,uint32_t a,uint32_t base)
{
	//max uint32_t: 4 294 967 296
	
	char buff[36];
	char buff2[36];
	k_memset(buff,'\0',34);
	
	
	int digit = 0;
	while (true)
	{
		uint32_t tmp = (a % base);
		if (tmp > 15)
			break;	// something has gone wrong
		char c = (tmp<10) ? 48+tmp:97+tmp-10;
		buff[digit] = c;
		a = a / base;
		if (a == 0)
			break;
		digit++;
	}
	uint32_t h = 0;
	for (int i = digit;i>=0;i--)
	{
		if (h >= length-1 || h >= 34)
			return h;
		string[h] = buff[i];
		h++;
	}
	return h;
}









































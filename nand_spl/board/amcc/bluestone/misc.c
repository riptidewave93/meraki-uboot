#include <asm/io.h>

//extern void serial0_putc(const char c);
//extern void serial0_puts(const char* s);
void inline printf(const char * format, ... )
{
//	serial0_puts(format);
}

void inline puts(void* s)
{
//	serial0_puts(s);
}

void inline putc(void* c)
{
//	serial0_putc(c);
}

void inline hang(void)
{
	while (1);
}

void udelay (int useconds)
{
	u32 it;
	for (it = 0; it < (useconds*1000); it++);
}

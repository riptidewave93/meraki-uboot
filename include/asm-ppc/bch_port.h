#ifndef __BCH_PORT_H__
#define __BCH_PORT_H__

#define CONFIG_BCH_CONST_PARAMS
#define CONFIG_BCH_CONST_M	13 /* 2K */
#define CONFIG_BCH_CONST_T	4

#if !defined(DIV_ROUND_UP)
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif

#if !defined(EINVAL)
#define EINVAL			22
#define EBADMSG 		74
#endif
#ifndef GFP_KERNEL
#define GFP_KERNEL		0
#endif

#if !defined(CONFIG_NAND_SPL)
#define KMALLOC(s, f)		malloc(s)
#define KFREE(p)		free(p)
#else		
void *KMALLOC(int size, int flgs);
void KFREE(void *);		
#endif

#endif

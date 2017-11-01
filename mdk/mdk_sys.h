#ifndef _MDK_SYS_H
#define _MDK_SYS_H

#include <cdk/cdk_types.h>

/* 
 * Address mapping macros
 */
#if CFG_L2_RAM   /* Temporarily here for SiByte SOCs running from L2 */
#define PTR_TO_PHYS(x) (((uintptr_t)(x)) + 0xD0000000)
#define PHYS_TO_PTR(a) ((uint8_t *)((a) - 0xD0000000))
#else
#define PTR_TO_PHYS(x) (PHYSADDR((uintptr_t)(x)))
#define PHYS_TO_PTR(a) ((uint8_t *)KERNADDR(a))
#endif

#define PCI_TO_PTR(a)  (PHYS_TO_PTR(PCI_TO_PHYS(a)))
#define PTR_TO_PCI(x)  (PHYS_TO_PCI(PTR_TO_PHYS(x)))


/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

int mdk_sys_udelay(uint32_t usec);
void *mdk_sys_dma_alloc_coherent(void *dvc, size_t size, dma_addr_t *baddr);
void mdk_sys_dma_free_coherent(void *dvc, size_t size, void *laddr, dma_addr_t baddr);
int mdk_sys_probe(void);
int mdk_sys_netdev_init(void);
int mdk_sys_init(void);

#endif /* _MDK_SYS_H */

/*
 * To debug NS ACP hang issue, An experiment test was written to perform following sequence. 
 * 1.Lock 
 * 2.Read, modify and write to Memory location-1 (cached memory access).
 * 3.unlock
 * 4.lock
 * 5.Read, modify and write to Memory location-2 (cached memory access).
 * 6.unlock
 * 7.lock
 * 8.Read from Memory location  3 (cached memory access).
 * 9.unlock
 * 10.lock
 * 11.I/O (PCIe) Read/Write.
 * 12.unlock
 * 13.Write character to Uart (preferably with cpu number)
 * 14.Go to step 1.
*/

#include <common.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <configs/iproc_board.h>
#include <asm/arch/iproc.h>
#include <asm/arch/socregs.h>
#include <asm/arch/reg_utils.h>
#include <asm/system.h>

extern int acpon2ndcore (void);

int acpon2ndcore ()
{
  int *memaddr;
  int rd_data;
  int id;
  int loop = 0x100;
  int lp_cnt;

  lp_cnt = loop;

/*  puts ("ACP Test on 2nd Core \n"); */

  id = get_cpuid();

  do 
   {
/*    memaddr = IPROC_DDR_MEM_BASE2 + 0x100; */
    memaddr = IPROC_DDR_MEM_BASE2; 

    l2c_lockdown_by_line();
    rd_data = *memaddr;
/*    printf("CPU ID - %d : Value Read at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data); */

    *memaddr+=0x1;
    rd_data = *memaddr;
/*    printf("CPU ID - %d : Value Read afer RMW at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data); */

    l2cc_unlock();
    l2cc_invalidate();

/*    memaddr = IPROC_DDR_MEM_BASE2 + 0x1100; */
    memaddr = IPROC_DDR_MEM_BASE2 + 0x1000; 

    l2c_lockdown_by_line();
    rd_data = *memaddr;
/*    printf("CPU ID - %d : Value Read at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data); */

    *memaddr+=0x1;
    rd_data = *memaddr;
/*    printf("CPU ID - %d : Value Read afer RMW at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data); */

    l2cc_unlock();
    l2cc_invalidate();

/*    memaddr = IPROC_DDR_MEM_BASE2 + 0x10100; */
    memaddr = IPROC_DDR_MEM_BASE2 + 0x10000;
    l2c_lockdown_by_line();
    rd_data = *memaddr;
/*    printf("CPU ID - %d : Value Read at addr: 0x%08x is 0x%08x\n", id, memaddr, rd_data); */

    l2cc_unlock();
    l2cc_invalidate();


    l2c_lockdown_by_line();

    loop--; 
    } while (loop != 0);

/*  printf("Executed the loop on core %d %d times \n", id, lp_cnt); */
  return 0;
}

/*
void l2c_lockdown_by_line (void);
*/

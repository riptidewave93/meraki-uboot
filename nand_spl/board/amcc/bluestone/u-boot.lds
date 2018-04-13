/*
 * (C) Copyright 2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

OUTPUT_ARCH(powerpc:common)
SECTIONS
{
  .resetvec 0xE3003FFC :
  {
    *(.resetvec)
  } = 0xffff
 
   .text :
   {
    start.o (.text)
    /*init.o (.text)
    nand_boot.o (.text)
    nand_ecc.o (.text)
    ndfc.o (.text)*/
   }
  _etext = .;
  .spl_data :
  {
    start.o (.rodata*)
    init.o (.rodata*)
    nand_boot.o (.rodata*)
    nand_ecc.o (.rodata*)
    ndfc.o (.rodata*)

    start.o (.data*)
    init.o (.data*)
    nand_boot.o (.data*)
    ndfc.o (.data*)
    
    nand_boot.o (.sdata*)
 
    __got2_start = .;
    *(.got2)
    __got2_end = .;
  }
  
   __bss_start = .;
  .bss (NOLOAD)       :
  {
   *(.sbss)
   *(.bss)
   . = ALIGN(4);
  }
	
  .data 0xE3004000    :
  {
  /*
    *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
    *(.data*)
    *(.sdata*) */
   44x_spd_ddr2.o (.rodata*)
   4xx_ibm_ddr2_autocalib.o (.rodata*)
   cache.o (.rodata*)
   misc.o (.rodata*)
   ppcstring.o (.rodata*)
   ppccache.o (.rodata*)
   tlb.o (.rodata*)
   cpu_init.o (.rodata*)
   speed.o (.rodata*)
   4xx_uart.o (.rodata*)

   44x_spd_ddr2.o (.data*)
   4xx_ibm_ddr2_autocalib.o (.data*)
   cache.o (.data*)
   misc.o (.data*)
   ppcstring.o (.data*)
   ppccache.o (.data*)
   tlb.o (.data*)
   cpu_init.o (.data*)
   speed.o (.data*)
   4xx_uart.o (.data*)

   44x_spd_ddr2.o (.sdata*)
   4xx_ibm_ddr2_autocalib.o (.sdata*)
   cache.o (.sdata*)
   misc.o (.sdata*)
   ppcstring.o (.sdata*)
   ppccache.o (.sdata*)
   tlb.o (.sdata*)
   cpu_init.o (.sdata*)
   speed.o (.sdata*)
   4xx_uart.o (.sdata*)

  }
  _edata  =  .;
   
  .sple :
  { 
   44x_spd_ddr2.o (.text)
   4xx_ibm_ddr2_autocalib.o (.text)
   cache.o (.text)
   misc.o (.text)
   ppcstring.o (.text)
   ppccache.o (.text)
   tlb.o (.text)
   cpu_init.o (.text)
   speed.o (.text)
   4xx_uart.o (.text)
   } 
  
   _end = . ;
}

#include <asm/mmu.h>

#define function_prolog(func_name)	.section .text; \
					.align 2; \
					.globl func_name; \
					func_name:
#define function_epilog(func_name)	.type func_name,@function; \
					.size func_name,.-func_name

/*----------------------------------------------------------------------------+
| ipp_mttlb3.
+----------------------------------------------------------------------------*/
	function_prolog(ipp_mttlb3)
	TLBWE(4,3,2)
	blr
	function_epilog(ipp_mttlb3)

/*----------------------------------------------------------------------------+
| ipp_mftlb3.
+----------------------------------------------------------------------------*/
	function_prolog(ipp_mftlb3)
	TLBRE(3,3,2)
	blr
	function_epilog(ipp_mftlb3)

/*----------------------------------------------------------------------------+
| ipp_mttlb2.
+----------------------------------------------------------------------------*/
	function_prolog(ipp_mttlb2)
	TLBWE(4,3,1)
	blr
	function_epilog(ipp_mttlb2)

/*----------------------------------------------------------------------------+
| ipp_mftlb2.
+----------------------------------------------------------------------------*/
	function_prolog(ipp_mftlb2)
	TLBRE(3,3,1)
	blr
	function_epilog(ipp_mftlb2)

/*----------------------------------------------------------------------------+
| ipp_mttlb1.
+----------------------------------------------------------------------------*/
	function_prolog(ipp_mttlb1)
	TLBWE(4,3,0)
	blr
	function_epilog(ipp_mttlb1)

/*----------------------------------------------------------------------------+
| ipp_mftlb1.
+----------------------------------------------------------------------------*/
	function_prolog(ipp_mftlb1)
	TLBRE(3,3,0)
	blr
	function_epilog(ipp_mftlb1)

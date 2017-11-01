/***************************************************************************
 *     Copyright (c) 2003-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 ***************************************************************************/

#include <common.h>
#include <command.h>
#include <pci.h>
#include <asm/processor.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <malloc.h>
#include <scsi.h>
#include <ata.h>
#include <linux/ctype.h>
#include <ahci.h>
#include "../../arch/arm/include/asm/arch-northstar-plus/socregs.h"
#include "ahci.h"

//#define SATA_DEBUG

#define DUMP_PHY  0

#ifdef SATA_DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif


#define PCIE_P0_CMD_LIST_BASE_ADDR   0x70000000
#define PCIE_P0_CMD_TABLE_DESC_BASE_ADDR  0x70020000
#define PCIE_P1_CMD_LIST_BASE_ADDR   0x70100000
#define PCIE_P1_CMD_TABLE_DESC_BASE_ADDR 0x70120000

#define PCIE_P0_FIS_BASE_ADDR   0x70400000
#define PCIE_P1_FIS_BASE_ADDR   0x70410000


#define PCIE_WR_BUF 0x71800000
#define PCIE_RD_BUF 0x71000000



#define PCIE_SATA_AHCI_PORT0_S2 0x8200100 /*0x18041100*/ //port 0 base
#define PCIE_SATA_AHCI_PORT0_S1 PCIE_SATA_AHCI_PORT0_S2
#if 1
#define AHCI_GHC_HOST_IRQ_STAT  0x8200008  /*0x18041008*/

#define RFIS_OFS 0x40
#define PSFIS_OFS 0x20

#define CMD_TBL_SIZE 0x80

#define AHCI_MAX_CMDS 32


#define AHCI_NUM_PORTS 1
#define SATA_AHCI_GHC  0x8200000  /*0x18041000*/
#define HOST_IS  0x8
#define HOST_GHC  0x4
#define HOST_GHC_HR 0x1
#endif
#define PCIE_SATA_AHCI_PORT1_S1 0x8200180 /*0x18041180*/
#if 1
#define PORT_X_IS PxIS
#define PORT_X_IS_UFS ( 1<< 4 )
#define PORT_X_SERR  PxSERR
#define PORT_X_IE PxIE

#define  PORT_X_IS_DHRS 1
#define PORT_X_IS_PSS ( 1<< 1)

#define PORT_X_CMD PxCMD
#define PORT_X_CMD_ST   1
#define PORT_X_CMD_FRE ( 1<< 4 )
#define PORT_X_CMD_FR  ( 1<< 14 )
#define PORT_X_CMD_CR    ( 1<< 15 )

#define PORT_X_SCTL  PxSCTL
#define PORT_SCR_CTL_SPD_MASK  0xF0
#define PORT_SCR_CTL_SPD_SHFT 4

#define PORT_X_CLB  PxCLB
#define PORT_X_CLBU PxCLBU
#define PORT_X_FB  PxFB
#define PORT_X_FBU  PxFBU

#define PORT_X_IS_PCS  ( 1<< 22 )


#define PORT_X_CMD_SUD  ( 1<< 1)


#define PORT_X_SSTS  PxSSTS
#define PORT_X_TFD PxTFD
#define SATA_AHCI_BASE 0x8200000  /*0x18041000*/
#define  HOST_GHC  0x4
#define HOST_GHC_AHCI_EN ( 1<< 31 )


#define PXSERR_DIAG_X ( 1 << 26 )
#define PXSERR_DIAG_W ( 1 << 18 )


#define SPEED_GEN1 1
#define SPEED_GEN2 2
#define SPEED_GEN3 3




#define true 1
#define false 0

#define MAX_WAIT_COUNT  10000000

#define WR               true
#define RD               false
#define DMA              true
#define PIO              false
#define NCQ              true
#define NON_NCQ          false


#define RST_ON           true
#define RST_OFF          false

#define ID_DEV           0xEC

#define MAX_BUF_SG_SIZE  ((PCIE_WR_BUF - PCIE_RD_BUF)/AHCI_MAX_CMDS)/*0x8000*/   /*WR_buf - rd_buf / 32 -- can be expanded*/

#define CMD_HDR_SIZE     32

#define XFER_PER_PRD     4096
/*#define XFER_PER_PRD     (16*1024)*/
#define PRD_ENTRY_SIZE   16
#define PRD_NUM_MAX      40

#define CMD_HDR_W0_W     0x00010045
#define CMD_HDR_W0_R     0x00010005

#define PORT0_BASE_ADDR             0x100
#define PORT_OFFSET                 0x80


#define PxTFD  0x20
#define PxSSTS 0x28
#define PxSCTL 0x2c
#define PxSERR 0x30
#define PxSACT 0x34


#define RFIS_FIS_TYPE 0x34
#define PSFIS_FIS_TYPE 0x5f

#define FIS_W0_WR_DMA_CMD   0x00CA8027
#define FIS_W0_RD_DMA_CMD   0x00C88027
#define FIS_W0_WR_FPDMA_CMD 0x00618027
#define FIS_W0_RD_FPDMA_CMD 0x00608027
#define FIS_W0_ID_CMD       0x00EC8027
#define FIS_W0_WR_PIO_CMD   0x00308027
#define FIS_W0_RD_PIO_CMD   0x00208027
#define FIS_W0_NOP_CMD      0x00E00027


#define FIS_W0_WR_DMA_EXT_CMD   0x00358027
#define FIS_W0_RD_DMA_EXT_CMD   0x00258027



/*SATA_AHCI_PORT0_S1 REGISTERS*/
#define PxCLB   0
#define PxCLBU  4
#define PxFB    8
#define PxFBU   0xC
#define PxIS    0x10
#define PxIE    0x14
#define PxCMD   0x18
#define PxCI    0x38


/* operation codes for device specific driver */
#define OP_INIT		1
#define OP_TX		2
#define OP_RX		3
#define OP_RXRDY	4
#define OP_TXRDY 	5
#define OP_BAUD		6
#define OP_RXSTOP	7
#define OP_FLUSH	8
#define OP_RESET	9
#define OP_OPEN		10
#define OP_CLOSE	11
#define OP_XBAUD	12


#define FIS_PLOAD_SIZE (8*1024) 
#define DWORD_SIZE 4

#define RWBUFSIZE  ((PCIE_WR_BUF - PCIE_RD_BUF))
#define SECTSIZE   0x200      /* 512 Bytes */
#endif


void pcie_txData0Out(char data);

static int              USED_PORT = 0x0;
static u32              readlx(u32 rAddr);
static void             writelx(u32 wAddr, u32 wData);

void pcie_AhciPrepCmd(int port,int tag,int w,int pio,int blk,u32 lba,int rst);
void pcie_AhciIssueCmd(int port, int tag);

int pcie_ahci_reset_controller(void);
void pcie_ahci_enable_ahci(void);
void pcie_ahci_port_clear_all_serr(int port_no);
void pcie_ahci_port_clear_all_int(int port_no);
void pcie_ahci_global_clear_all_int(void);
void pcie_ahci_clear_global_int_stat(void);
void pcie_delay_ms(int ms);
u32 pcie_ata_wait_register( u32 reg, u32 mask, u32 val);
int pcie_SpinUpAndWait (int port, int speed);

int pcie_CmpBuf(int port, int tag, int blkCnt);
void pcie_wait_command_done(int port, int tag, int pio, int blk);

void pcie_WaitGlobalInt(int port);



int pcie_ahci_init (u32 x)
{
    USED_PORT = x;

    //printf("\nInit port to gen %d\n",SPEED_GEN3);
	return (pcie_ahci_init_one(SPEED_GEN1));

}


u32 readlx(u32 rd_addr)
{
  u32 tmp;
  tmp =  *((volatile u32 *)rd_addr);
  return tmp;
}


void writelx(u32 wr_addr, u32 wr_data)
{
  *((volatile u32 *)wr_addr) = wr_data;
}


void pcie_wait_drive_status_no_bsy (int port)
{
	u32 s2BaseAddr;
	u32 s1BaseAddr, tmpVal;
	volatile u32 *pTmp32;

	s1BaseAddr = PCIE_SATA_AHCI_PORT0_S1 + PORT_OFFSET * port;

	s2BaseAddr = PCIE_SATA_AHCI_PORT0_S2 + PORT_OFFSET * port;

	/* debug("WAIT: drive not busy\n"); */

	pTmp32 = (volatile u32 *) (s2BaseAddr + PxTFD);

	while (*pTmp32 & 0x80)
	{
		printf("Command processing... Busy\n");
		mdelay(100);
	}

	if (*pTmp32 & 1)
	{
		printf("\nERROR: Command complete with error: %x", *pTmp32);
	} else
	{
		tmpVal = readlx(s1BaseAddr + PxIS);
		printf("\n - Command Completed with NO error - PxIS = %x", tmpVal);
	}

}


void pcie_AHCI_DMA_Write(int port, int tag, int blk, u32 lba)
{

	pcie_AhciPrepCmd(port,tag,WR,DMA,blk,lba,RST_OFF);

	/* debug("Doing:  pcie_AhciIssueCmd\n"); */
	pcie_AhciIssueCmd(port, tag);
	printf("Doing:  pcie_wait_command_done tag %d\n",tag);
	pcie_wait_command_done (port, tag, DMA,blk);
 
}

void pcie_AHCI_DMA_read(int port, int tag, int blk, u32 lba)
{

	pcie_AhciPrepCmd(port,tag,RD,DMA,blk,lba,RST_OFF);
	/* debug("Doing:  pcie_AhciIssueCmd\n"); */
	pcie_AhciIssueCmd(port, tag);
	/* debug("Doing:  pcie_wait_command_done\n"); */
	pcie_wait_command_done (port, tag, DMA, blk);
}

void pcie_wait_command_done(int port, int tag, int pio, int blk)
{
	u32 s1BaseAddr, s2BaseAddr, msk, rxFis, fisType;
	volatile u32 *pTmp32;


	s1BaseAddr = PCIE_SATA_AHCI_PORT0_S1 + PORT_OFFSET*port;
	s2BaseAddr = PCIE_SATA_AHCI_PORT0_S2 + PORT_OFFSET*port;


	if ( port == 0 )
	{
		if (pio == DMA )
		{
			rxFis = PCIE_P0_FIS_BASE_ADDR + RFIS_OFS;
			fisType = RFIS_FIS_TYPE;
		}
		else
		{
			rxFis = PCIE_P0_FIS_BASE_ADDR + PSFIS_OFS;
			fisType = PSFIS_FIS_TYPE;
		}
	}
	else
	{
		if (pio == DMA )
		{
			rxFis   = PCIE_P1_FIS_BASE_ADDR + RFIS_OFS;
			fisType = RFIS_FIS_TYPE;
		}
		else
		{
			rxFis   = PCIE_P1_FIS_BASE_ADDR + PSFIS_OFS;
			fisType = PSFIS_FIS_TYPE;
		}
	}

	debug("WAIT: CI clear\n");
	msk = 1 << tag;

	pTmp32 = (volatile u32 *)(s1BaseAddr + PxCI);

	while (*pTmp32 & msk);


	debug("OK: CI cleared\n"); 
	debug("WAIT: drive not busy\n");

	pTmp32 = (volatile u32 *)(s2BaseAddr + PxTFD);

	
	
	while (*pTmp32 & 0x80);

	debug("OK: Drive not busy\n");

	debug("WAIT: RFIS\n"); 


	debug("\n - bsy clear - PxIS = %x",(u32)readlx (s1BaseAddr + PxIS));

	pTmp32 = (u32 *)rxFis;

	while ((*pTmp32 & 0x000000FF) != fisType);

	/*  printf("OK: Got RxFIS addr: %x data %x\n", pTmp32, *pTmp32);  */

	/*Clear it for the next command */
	*pTmp32 = 0;




	debug("OK: Got Global interrupt\n"); 

	/*Check port interrupt set */

	debug("WAIT: Port interrupt set\n"); 

	if (pio == DMA)
		msk = 1;
	else
		msk = 2;

	pTmp32 = (u32 *)(s1BaseAddr + PxIS);

	while(!(*pTmp32 & msk));

	/*debug("OK: Got Port interrupt set\n"); */

	/*debug("Clearing port int\n"); */

	pcie_ahci_port_clear_all_int(port);

	/*  tmp32 = *pTmp32; */
	/*  *pTmp32 = tmp32; */

	debug("WAIT: Port interrupt Clear\n"); 

	/*while(*pTmp32 & msk); */

	/*debug("OK: Port interrupt cleared\n"); 


	debug("WAIT: Global interrupt clear\n"); 
	pTmp32 = (u32 *)AHCI_GHC_HOST_IRQ_STAT;

	if (port == 0)
		msk = 0x00000001;
	else
		msk = 0x00000002;

	/*Clear only that port */
	*pTmp32 = msk;

	/*Wait for just cleared port */
	msk    = 1 << port;
	while(*pTmp32 & msk);

	/*debug("OK: Global interrupt cleared\n"); */

}


void pcie_CreateCmdHeader (int port, int tag, int w, int blk, u32 lba, int rst)
{
	u32 cmd_hdr_dw[8];
	int byteCount,i;
	u32 hdr_addr,prdtl;
	u32 *pTmp32;


	byteCount = blk*512;

	/*debug("Enter: CreateCmdHeader\n"); */
	if (byteCount <= XFER_PER_PRD)
		prdtl = 1;
	else if ( byteCount % XFER_PER_PRD != 0)
		prdtl = byteCount / XFER_PER_PRD + 1;
	else
		prdtl = byteCount / XFER_PER_PRD;

	if (prdtl > PRD_NUM_MAX)
	{
		debug("ERROR: Required to many PRDs");
		return;
	}

	if (w == WR)
		cmd_hdr_dw[0] = (CMD_HDR_W0_W & 0x0000FFFF) |  (prdtl << 16);
	else
		cmd_hdr_dw[0] = (CMD_HDR_W0_R & 0x0000FFFF) |  (prdtl << 16);

	if (rst == RST_ON)
		cmd_hdr_dw[0] |= 0x100;

	cmd_hdr_dw[1] = 0;



	/*Must by physical addr */
	if (port == 0)
		cmd_hdr_dw[2] = (PCIE_P0_CMD_TABLE_DESC_BASE_ADDR + tag*(CMD_TBL_SIZE + PRD_ENTRY_SIZE * PRD_NUM_MAX)); // & 0x1FFFFFFF;
	else
		cmd_hdr_dw[2] = (PCIE_P1_CMD_TABLE_DESC_BASE_ADDR + tag*(CMD_TBL_SIZE + PRD_ENTRY_SIZE * PRD_NUM_MAX));// & 0x1FFFFFFF;

	cmd_hdr_dw[3] = 0;
	cmd_hdr_dw[4] = 0;
	cmd_hdr_dw[5] = 0;
	cmd_hdr_dw[6] = 0;
	cmd_hdr_dw[7] = 0;

	if (port == 0)
		hdr_addr = PCIE_P0_CMD_LIST_BASE_ADDR + tag * CMD_HDR_SIZE;
	else
		hdr_addr = PCIE_P1_CMD_LIST_BASE_ADDR + tag * CMD_HDR_SIZE;

	pTmp32 = (u32 *) hdr_addr;

	/*debug("CreateCmdHeader: Write header\n"); */
	/*Write header to memory */
	for(i = 0; i < 8; i++)
	{
		/*debug("1\n"); */
		*pTmp32= cmd_hdr_dw[i];

		if(i == 1)
			debug("\n->DBC %x",*pTmp32);

		pTmp32++;
	}

  /*debug("Exit: CreateCmdHeader\n"); */
}

void pcie_CreateCmdTable(int port, int tag, int w, int pio, int blk, u32 lba, int rst)
{
	u32 fis_w[5];
	u32 tbl_addr, *pTmp32;
	int i;

	if (port == 0)
	{
		tbl_addr  = PCIE_P0_CMD_TABLE_DESC_BASE_ADDR;
	}
	else
	{
		tbl_addr  = PCIE_P1_CMD_TABLE_DESC_BASE_ADDR;
	}


	if (pio == DMA)
	{
		if (w == WR)
		{
			if (blk <= 127)
				fis_w[0] = FIS_W0_WR_DMA_CMD;
			else
				fis_w[0] = FIS_W0_WR_DMA_EXT_CMD;

		}
		else
		{
			if (blk <= 127)
				fis_w[0] = FIS_W0_RD_DMA_CMD;
			else
				fis_w[0] = FIS_W0_RD_DMA_EXT_CMD;
		}
	}
	else
	{
		if (w == WR)
			fis_w[0] = FIS_W0_WR_PIO_CMD;
		else
			fis_w[0] = FIS_W0_RD_PIO_CMD;
	}


	tbl_addr = tbl_addr + tag*(CMD_TBL_SIZE + PRD_ENTRY_SIZE * PRD_NUM_MAX);



	fis_w[1] = 0xe0000000 | (lba & 0x00FFFFFF);

	fis_w[2] = (lba & 0xFF000000) >> 24;

	fis_w[3] = blk;

	if (rst == RST_ON)
		fis_w[3] = fis_w[3] |  0x04000000;

	fis_w[4] = 0;

	pTmp32 = (u32 *)tbl_addr;

	for(i = 0; i < 5; i++)
	{
		*pTmp32= fis_w[i];
		pTmp32++;
	}

}

void pcie_AhciFillCmdSlot(int port, int tag, int w, int pio, int blk, int ncq, u32 lba, int rst)
{
	pcie_CreateCmdHeader (port, tag, w, blk,lba,rst);

	pcie_CreateCmdTable(port, tag, w, pio , blk, lba,rst);
}

void pcie_AhciFilSg(int port, int tag, u32 addr ,int byteCount)
{
	u32 *prd_addr,prdtl,cmdtbl_base,size, bufAddr,count,bcount, prd_cnt, hdr_addr;
	volatile u32 *pTmp32;
	u32 *p32,acc,j;

	if (byteCount <= XFER_PER_PRD)
		prdtl = 1;
	else if ( byteCount % XFER_PER_PRD != 0)
		prdtl = byteCount / XFER_PER_PRD + 1;
	else
		prdtl = byteCount / XFER_PER_PRD;

	if (prdtl > PRD_NUM_MAX)
	{
		debug("ERROR: Required to many PRDs");
		return;
	}

	if (port == 0)
		cmdtbl_base = PCIE_P0_CMD_TABLE_DESC_BASE_ADDR + tag*(CMD_TBL_SIZE + PRD_ENTRY_SIZE * PRD_NUM_MAX);
	else
		cmdtbl_base = PCIE_P1_CMD_TABLE_DESC_BASE_ADDR + tag*(CMD_TBL_SIZE + PRD_ENTRY_SIZE * PRD_NUM_MAX);

	count    = 0;
	size     = byteCount;
	prd_addr = (u32 *)(cmdtbl_base + 0x80);


	bufAddr  = addr;// & 0x1FFFFFFF; /*Must be physical */

	prd_cnt = 0;

	while (size)
	{
		/*Change the mask.. this can only be used for 4K only */
		bcount = 0x00001000 - (bufAddr & 0x00000fff);

		if (bcount > size)
			bcount = size;

		*prd_addr = bufAddr;

		/*      printf("-->prdaddr %x Buf addr %x", (u32)prd_addr, (u32)bufAddr); */
		prd_addr++;
		*prd_addr = 0;
		prd_addr++;
		*prd_addr = 0;
		prd_addr++;

		if (bcount == 0)
			*prd_addr =  (XFER_PER_PRD-1);
		else
			*prd_addr =  ((bcount - 1) & 0x00000FFF);

		/*      printf("--> count %x pr cnt = %d addr %x\n", *prd_addr, prd_cnt,prd_addr); */
		prd_addr++;

		bufAddr = bufAddr + bcount;

		prd_cnt++;

		if(prd_cnt > 0)
		{
			bufAddr += 0x100;
			/*          printf("Nxt buf %x\n", bufAddr); */
		}

		size    = size - bcount;


	}


	/* set prd count in header */
	if (port == 0)
		hdr_addr = PCIE_P0_CMD_LIST_BASE_ADDR + tag * CMD_HDR_SIZE;
	else
		hdr_addr = PCIE_P1_CMD_LIST_BASE_ADDR + tag * CMD_HDR_SIZE;

	pTmp32 = (u32 *) hdr_addr;

	/* change PRD count */
	*pTmp32 = (*pTmp32 & 0x0000FFFF) | (prd_cnt << 16);

	debug("\nupdate cmd header %x\n", *pTmp32);

	/* init buffer */

	prd_addr = (u32 *)(cmdtbl_base + 0x80);

	for(acc = 0, count = 0; count < prd_cnt; count++)
	{
		/* init buffer */
		p32 = *((u32 *)(prd_addr + count*4));
		bcount = *((u32 *)(prd_addr + count*4 + 3));

		/*      printf("======prd %d bufadr %x count %x prdaddr %x countaddr %x\n", count,p32,bcount,(u32)(prd_addr + count*4),((u32)(prd_addr + count*4 + 3))); */

		for(j=0; j < (bcount + 1)/4; j++)
		{
			*p32 = acc;
			/*          printf("addr %x data %x", p32,acc); */
			p32++;
			acc++;
		}
	}
}

void pcie_AhciPrepCmd_buf(int port,int tag,int w,int pio,int blk,u32 lba,int rst, char * buf)
{

  pcie_AhciFillCmdSlot (port,tag,w,pio,blk,NON_NCQ,lba,rst);
  
  pcie_AhciFilSg(port, tag, (u32)buf, (512 * blk));
}


void pcie_AhciPrepCmd(int port,int tag,int w,int pio,int blk,u32 lba,int rst)
{

  pcie_AhciFillCmdSlot (port,tag,w,pio,blk,NON_NCQ,lba,rst);

  if (w == WR)
  {
	  pcie_AhciFilSg(port, tag, PCIE_WR_BUF + (tag * (512 * blk)), (512 * blk));
  }
  else
  {
	  pcie_AhciFilSg(port, tag, PCIE_RD_BUF + (tag * MAX_BUF_SG_SIZE), (512 * blk));
  }
}

void pcie_AhciIssueCmd(int port, int tag)
{
  u32 *pTmp32, tmp32;

  pTmp32 = (u32 *)(PCIE_SATA_AHCI_PORT0_S1 + PORT_OFFSET*port + PxCI);
  tmp32  = *pTmp32;

  *pTmp32 = tmp32 | (1 << tag);
}

void pcie_AhciIssueCmdAll(int port)
{
  u32 *pTmp32;

  pTmp32  = (u32 *)(PCIE_SATA_AHCI_PORT0_S1 + PORT_OFFSET*port + PxCI);
  *pTmp32 = 0xFFFFFFFF;
}

int pcie_ahci_init_one(int speed)
{

	int i;
	u32 tmp32;
	char str[16];

	printf("\nInit port %d \n", USED_PORT);

	pcie_ahci_reset_controller();

	pcie_ahci_init_controller();

	if ( -1 == pcie_SpinUpAndWait (USED_PORT, speed))
	{
		return -1;
	}
	pcie_ahci_port_clear_all_serr(USED_PORT);     /*Don't do much just clear serr */
	pcie_ahci_port_clear_all_int(USED_PORT);  /*Do it again for the test of the ints */
	pcie_ahci_clear_global_int_stat();

	return 0;
}



void pcie_ahci_clear_global_int_stat(void)
{
  /*Clear global int */
	writelx(SATA_AHCI_GHC + HOST_IS, 0xFFFFFFFF);

}

int pcie_ahci_reset_controller()
{
  u32 ahci_base_addr = (u32)SATA_AHCI_GHC;
	/*u32 ahci_base_addr = (u32)0x181000;*/
  u32 tmp;

  /*printf("In rset controller\n");*/
  /* we must be in AHCI mode, before using anything
   * AHCI-specific, such as HOST_GHC_HR.
   */
  pcie_ahci_enable_ahci();

  /* global controller reset */

  tmp = readlx(ahci_base_addr + HOST_GHC);

  if ((tmp & HOST_GHC_HR) == 0) {
    writelx(ahci_base_addr + HOST_GHC,tmp | HOST_GHC_HR);
    readlx(ahci_base_addr + HOST_GHC); /* flush */
  }

  /*
   * to perform host reset, OS should set HOST_GHC_HR
   * and poll until this bit is read to be "0".
   * reset must complete within 1 second, or
   * the hardware should be considered fried.
   */
  tmp = pcie_ata_wait_register(ahci_base_addr + HOST_GHC, HOST_GHC_HR,HOST_GHC_HR);

  /* turn on AHCI mode */
  pcie_ahci_enable_ahci(/*ahci_base_addr*/);


  return 0;
}


static /*inline*/ u32 ahci_port_base( u32 port_no)
{
  if (port_no == 0)
    return (u32)PCIE_SATA_AHCI_PORT0_S1;
  else
    return (u32)PCIE_SATA_AHCI_PORT1_S1;
}

/*Good */
void pcie_ahci_port_init(u32 port)
{
	u32 baseAddr, tmp32;

  baseAddr = ahci_port_base(port);
	
  /*Clear port int */
  tmp32    = readlx(baseAddr + PORT_X_IS);

  /*UNK FIS must be cleared in SERR */
  if(tmp32 & PORT_X_IS_UFS)
    {
      /*Check make sure it sets before clearing it */
      tmp32    = readlx(baseAddr + PORT_X_SERR);

      if(!(tmp32 & 0x02000000))
	{
	  debug("ERROR: PORT IRQ unknown FIS int set but SERR not SET");
	  while(1);
	}

      /*Just clear one bit */
      writelx(baseAddr + PORT_X_SERR, tmp32 & 0x02000000);

    }

  /*Skip UNK FIS */
  writelx(baseAddr +  PORT_X_IS, tmp32 & (~(PORT_X_IS_UFS)));

  /*Clear SERR */

  tmp32    = readlx(baseAddr + PORT_X_SERR);
  writelx(baseAddr + PORT_X_SERR, tmp32);


  /*Clear global int */
  writelx(SATA_AHCI_GHC + HOST_IS, 0xFFFFFFFF);

  /*Enable port device to host reg FIS int */
  tmp32   = readlx(baseAddr + PORT_X_IE);
  writelx(baseAddr + PORT_X_IE, tmp32 | (PORT_X_IS_DHRS | PORT_X_IS_PSS));

}


void pcie_ahci_init_controller()
{
  int i;

  for (i = 0; i < AHCI_NUM_PORTS; i++) {

		#if (BCHP_CHIP == 7425)
			pcie_ahci_port_init(1);
		#else
			pcie_ahci_port_init(i);
		#endif
	
  }

}

int pcie_SpinUpAndWait (int port, int speed)
{
	u32 s1BaseAddr, s2BaseAddr, s3BaseAddr;
	u32 wait_count,pxCmdSnapShot;
	u32 port_base_addr,i;
	volatile u32 tmp32;



	s1BaseAddr = PCIE_SATA_AHCI_PORT0_S1 + PORT_OFFSET*port;
	s2BaseAddr = PCIE_SATA_AHCI_PORT0_S2 + PORT_OFFSET*port;


	//tmp32 &= ~(PORT_CMD_LIST_ON | PORT_CMD_FIS_ON |
		// PORT_CMD_FIS_RX | PORT_CMD_START);

	//writelx ((s1BaseAddr + PxCMD), tmp32);



	//s3BaseAddr = SATA_AHCI_PORT0_S3 + PORT_OFFSET*port;

	/*printf("Enter Spinup and wait\n");*/
	/*Enable AHCI */
	pcie_ahci_enable_ahci();


	//if (readlx (s2BaseAddr + PxSSTS) & 3 )
	//{
	//	printf("\nDevice is connected, no spin up \n");
	//	return;
	//}

	/*Enable Rx */
	/*printf("Enable Rx\n");*/
	tmp32 = readlx(s1BaseAddr + PxCMD);
	tmp32 |= PORT_X_CMD_FRE;
	writelx(s1BaseAddr + PxCMD, tmp32);


	/*Set speed */
	tmp32 = readlx(s2BaseAddr + PxSCTL);
	tmp32 = tmp32 & PORT_SCR_CTL_SPD_MASK;
	tmp32 = tmp32 | speed << PORT_SCR_CTL_SPD_SHFT;
	writelx ((s2BaseAddr + PxSCTL), tmp32);


	/*5. Set CLB and CLBU */
	if (port == 0)
	{
		writelx(s1BaseAddr + PxCLB,    (PCIE_P0_CMD_LIST_BASE_ADDR));// & 0x1FFFFFFF);

		writelx(s1BaseAddr + PxCLBU, 0);

		/*6. Set FB and FBU */
		writelx(s1BaseAddr + PxFB,    PCIE_P0_FIS_BASE_ADDR);//& 0x1FFFFFFF);
		writelx(s1BaseAddr + PxFBU, 0);
	}
	else
	{

		writelx(s1BaseAddr + PxCLB,    (PCIE_P1_CMD_LIST_BASE_ADDR));// & 0x1FFFFFFF);
		writelx(s1BaseAddr + PxCLBU, 0);

		/*6. Set FB and FBU */
		writelx(s1BaseAddr + PxFB,    (PCIE_P1_FIS_BASE_ADDR));// & 0x1FFFFFFF);
		writelx(s1BaseAddr + PxFBU, 0);

	}


	/*Clear IRQ */
	/*printf("Clear IRQ\n");*/
	writelx ((s1BaseAddr + PxIS), 0xFFFFFFFF);


	/*Make sure DET is cleared before setting SUD */
	tmp32 = readlx (s2BaseAddr + PxSCTL);
	tmp32 = tmp32 & 0xfffffffe; /*Clear DET */
	writelx ((s2BaseAddr + PxSCTL), tmp32);

	/*Check for for PCS -- the device may be up already */
	tmp32 = readlx (s1BaseAddr + PxIS);

	if(tmp32 & PORT_X_IS_PCS)
	{

		/*printf ("Got PCS");*/

		wait_count = MAX_WAIT_COUNT;

		do
		{
			/*Clear SERR */
			tmp32 = readlx (s2BaseAddr + PxSERR);
			writelx ((s2BaseAddr + PxSERR), tmp32);

			tmp32 = readlx (s1BaseAddr + PxIS);
			/*Make sure PCS goes away */
			tmp32 = readlx (s1BaseAddr + PxIS);

			if(!(--wait_count))
			{
				printf("\nERROR: Timeout waiting for SERR to clear\n");
				return -1;
			}
		} while (tmp32 & 0x00000040);
	}

	printf ("Spin up device\n");

	/*Spin up device .. set pxcmd.sud */

	tmp32 = readlx (s1BaseAddr + PxCMD);

	tmp32 |= PORT_X_CMD_SUD;
	writelx ((s1BaseAddr + PxCMD), tmp32);



	/*Wait for PHY ready */
	tmp32 = readlx (s2BaseAddr + PxSSTS);

	wait_count = MAX_WAIT_COUNT;
	do
	{
		tmp32 = readlx (s2BaseAddr + PxSSTS);
		/*printf("tmp32 = 0x%08x \n", tmp32);*/

		if(!(--wait_count))
		{
			printf("\nERROR: Timeout waiting for PHY ready on port %d\n",port);
			return -1;
		}

	}while ((tmp32 & 0x00000003) != 3);

	/*PHY should be ready now.. */
	/*Check for COMWAKE AND COMINIT */

	do
	{

		/*Check for for PCS -- AFTER SPINNED UP  */
		tmp32 = readlx (s1BaseAddr + PxIS);
		mdelay(1);
		if( tmp32 & PORT_X_IS_PCS )
		{
			printf("PCS is 0x%x\n",tmp32 );
			break;
		}
		else
		{
			printf("PCS is 0x%x\n",tmp32 );
		}
	}while(1);

        printf("\n PCS check is done \n");

	if (tmp32 & PORT_X_IS_PCS)
	{
		/*    debug ("Got PCS after spin up -- checking exchange bit"); */
		tmp32 = readlx (s2BaseAddr + PxSERR);

		while(!(tmp32 & PXSERR_DIAG_X));
                printf("\n OK got X bit ");
		/*    debug("OK Got X bit -- checking COMMWAKE\n"); */
		// swang while(!(tmp32 & PXSERR_DIAG_W));
		/*    debug("OK Got COMM WAKE\n"); */ /* done checking */

                printf("\n Got COMM WAKE ");

		/*Clear SERR */
		tmp32 = readlx (s2BaseAddr + PxSERR);
		writelx ((s2BaseAddr + PxSERR), tmp32);

		/*Make sure COMWAKE GOT RESET */
		tmp32 = readlx (s2BaseAddr + PxSERR);

		wait_count = MAX_WAIT_COUNT;

		while(tmp32 & PXSERR_DIAG_W)
		{
			if(!(--wait_count))
			{
				printf("\nERROR: Timeout waiting for DIAG_W\n");
				return -1;
			}

		}
		/*    debug("OK COMM WAKE cleared\n");  */

		/*Make sure COMWAKE X GOT RESET */

		tmp32 = readlx (s2BaseAddr + PxSERR);

		wait_count = MAX_WAIT_COUNT;

		while(tmp32 & PXSERR_DIAG_X)
		{
			if(!(--wait_count))
			{
				printf("\nERROR: Timeout waiting for PXSERR_DIAG_X \n");
				return -1;
			}

			tmp32 = readlx (s2BaseAddr + PxSERR);

		}
		/*    debug("OK X bit cleared\n");  */


		wait_count = MAX_WAIT_COUNT;

		do
		{
			tmp32 = readlx (s1BaseAddr + PxIS);
			/*Make sure PCS goes away */
			tmp32 = readlx (s1BaseAddr + PxIS);

			if(!(--wait_count))
			{
				printf("\nERROR: Timeout wait for PCS\n");
				return -1;
			}


		}
		while (tmp32 & PORT_X_IS_PCS);
	}
	else
	{
		debug ("ERROR: WAIT FOR PCS");
		while(1);
	}

	tmp32 = (tmp32 & ~PORT_SCR_CTL_SPD_MASK) >> PORT_SCR_CTL_SPD_SHFT;;


	tmp32 = readlx (s2BaseAddr + PxSERR);
	writelx ((s2BaseAddr + PxSERR), tmp32);

	/*debug ("Wait for Device ready"); */
	/*Wait for device ready */
	tmp32 = readlx (s2BaseAddr + PxTFD);

	wait_count = MAX_WAIT_COUNT;
	do
	{
		tmp32 = readlx (s2BaseAddr + PxTFD);


		if(!(--wait_count))
		{
			printf("\nERROR: Timeout wait for device ready\n");
			udelay(100);
			return -1;
		}

	}while (tmp32 & 0x00000080);


	/*Clear SERR */
	tmp32 = readlx (s2BaseAddr + PxSERR);
	writelx ((s2BaseAddr + PxSERR), tmp32);

	printf("Enable CMD");
	/*enabl CMD */
	tmp32 = readlx (s1BaseAddr + PxCMD);
	tmp32 = tmp32 | PORT_X_CMD_ST;
	writelx ((s1BaseAddr + PxCMD), tmp32);

	return 0;

	/*debug ("Exit pcie_SpinUpAndWait"); */

}

void pcie_ahci_port_clear_all_serr(int port_no)
{
  u32 port_base_addr, tmp32;

 port_base_addr = ahci_port_base(port_no);
 writelx(port_base_addr + PORT_X_SERR, 0xFFFFFFFF);
 /* debug("PORT: Clear all SERR"); */
 do{
     tmp32 = readlx(port_base_addr + PORT_X_SERR);
 }while(tmp32);

 /*debug("PORT: SRR cleared to 0"); */

}

void pcie_ahci_port_clear_all_int(int port_no)
{
  u32 port_base_addr, tmp32;

  port_base_addr = ahci_port_base(port_no);


  /*Clear port int */
  tmp32    = readlx(port_base_addr + PORT_X_IS);

  /*UNK FIS must be cleared in SERR */
  if(tmp32 & PORT_X_IS_UFS)
    {
      /*Check make sure it sets before clearing it */
      tmp32    = readlx(port_base_addr + PORT_X_SERR);

      if(!(tmp32 & 0x02000000))
	{
	  debug("ERROR: PORT IRQ unknown FIS int set but SERR not SET");
	  while(1);
	}

      /*Just clear one bit */
      writelx(port_base_addr + PORT_X_SERR, tmp32 & 0x02000000);

    }

  /*Skip UNK FIS */
  writelx(port_base_addr +  PORT_X_IS, tmp32 & (~(PORT_X_IS_UFS)));

  /*  debug("PORT: Clearing port ints\n"); */
  do{
    tmp32 = readlx(port_base_addr + PORT_X_IS);
  }while(tmp32);

  /*debug("PORT: Port ints cleared to 0\n"); */

}

void pcie_ahci_global_clear_all_int()
{
    writelx(SATA_AHCI_GHC + HOST_IS, 0xFFFFFFFF );
}

u32 pcie_ata_wait_register( u32 reg, u32 mask, u32 val)
{
  
  u32 tmp;

  tmp = readlx(reg);

	while ((tmp & mask) == val )
    {
        tmp = readlx(reg);
    }

    return tmp;
}


void pcie_ahci_enable_ahci()
{
	u32 ahci_base_addr = (u32) SATA_AHCI_BASE;
	u32 tmp;

	/* turn on AHCI_EN */
	tmp = readlx(ahci_base_addr + HOST_GHC);
	/*tmp = readlx(0x2c1d2000 + 0x00181004);
	printf("tmp=0x%08x\n", tmp);
	printf("tmp=%d\n", (tmp & HOST_GHC_AHCI_EN));
	printf("Set AHCI_ENABLE = %d \n", (tmp |= HOST_GHC_AHCI_EN));
	 */
	if (tmp & HOST_GHC_AHCI_EN)
		return;

	/* Some controllers need AHCI_EN to be written multiple times.
	 * Try a few times before giving up.
	 */
	while(1) {
		tmp |= HOST_GHC_AHCI_EN;
		/*
		printf("tmp inside while %d\n", (tmp |= HOST_GHC_AHCI_EN));
		printf("tmp inside while 0x%08x\n", (tmp |= HOST_GHC_AHCI_EN));
		printf("= to ahci_base_addr + HOST_GHC\n");
		 */
		writelx(ahci_base_addr + HOST_GHC, tmp);
		/*debug("2"); */
		tmp = readlx(ahci_base_addr + HOST_GHC);	/* flush && sanity check */
		if (tmp & HOST_GHC_AHCI_EN)
		{
			/*debug("AHCI EN  SET\n");	 */
			return;
		}


	}


}

void pcie_delay_ms(int ms)
{
  //BSTD_UNUSED(ms);
  mdelay( ms );
  /*  debug("Delay: ms --- NOT IMPLEMENTED YET\n"); */
}



void pcie_WaitGlobalInt(int port)
{
  volatile u32 *pTmp32, msk;
  pTmp32 = (volatile u32 *)AHCI_GHC_HOST_IRQ_STAT;
  msk    = 1 << port;

  while(!(*pTmp32 & msk));

}


int  pcie_SATA_DMA_RD_WR_verify(int port, int tag, int blk, u32 lba)
{

	debug("\nStart wr/rd data ....\n");
	/*debug("Doing:  pcie_PCIE_AhciPrepCmd -- WR\n"); */
	pcie_AhciPrepCmd(port,tag,WR,DMA,blk,lba,RST_OFF);
	/*debug("Doing:  pcie_AhciIssueCmd\n"); */
	pcie_AhciIssueCmd(port, tag);
	/*debug("Doing:  pcie_wait_command_done\n");   */
	pcie_wait_command_done (port, tag, DMA,blk);
	/*debug("Doing:  pcie_AhciPrepCmd -- RD\n"); */
	pcie_AhciPrepCmd(port,tag,RD,DMA,blk,lba,RST_OFF);
	/*debug("Doing:  pcie_AhciIssueCmd\n"); */
	pcie_AhciIssueCmd(port, tag);
	/*debug("Doing:  pcie_wait_command_done\n");   */
	pcie_wait_command_done (port, tag, DMA, blk);
	/*debug("Doing:  pcie_CmpBuf\n");   */
	if (pcie_CmpBuf(port,tag,blk))
	{
		return -1;
	}

	return 0;
}

int pcie_CmpBuf(int port, int tag, int blkCnt)
{
	u32 *pWr, *pRd, cnt, i;

	pWr = (u32 *)PCIE_WR_BUF;
	pRd = (u32 *)PCIE_RD_BUF;

	cnt = blkCnt * 512/4;

	for (i = 0; i < cnt; i++)
	{
		if (*pWr != *pRd)
		{
			printf("** ERROR: Data Miscompared at word offset %x***\n",cnt);
			printf("Expect: 0x%x Read: 0x%x\n",*pWr, *pRd);
			return 1;
		}

		pWr++;
		pRd++;
	}
	return 0;
}




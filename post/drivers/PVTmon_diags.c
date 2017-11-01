/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include <common.h>
#include <post.h>


#if CONFIG_POST & CONFIG_SYS_POST_PVTMON 

#define PVTMON_CONTROL0    0x1803F2C0
#define PVTMON_CONTROL1    0x1803F2C4
#define PVTMON_STATUS      0x1803F2C8

unsigned int pTable[16]={64,128,192,256,320,384,448,512,576,640,704,768,832,896,960,1023};

int pvtmon_post_test(flags)
{

    unsigned long int u32Accumulator = 0;
    unsigned long int u32Max = 0;
    unsigned long int u32Min = 0x3FF;
    unsigned long int u32Readout;
    float flAccumulator;
    float flMax;
    float flMin;
    int i, j;
    int status=0;

    post_log("\nPVT monitor starts, press any key to stop.");
    while (!_serial_tstc(CONFIG_UART0_INDEX)) {
         u32Accumulator = 0;
         u32Max = 0;
         u32Min = 0x3FF;

         u32Readout = readl(PVTMON_CONTROL0);
         u32Readout = u32Readout & 0xFFFFFFF1;      //set [3:1] = 000 to enable temperature monitor
         writel(u32Readout, PVTMON_CONTROL0);

         writel(0x001D2003, PVTMON_CONTROL1);

         udelay(1000);
         for (i = 0; i < 1024; i++) {
              u32Readout = 0x3FF & readl(PVTMON_STATUS);
              u32Accumulator += u32Readout;

              if (u32Readout > u32Max) {
                    u32Max = u32Readout;
              }
              else if(u32Readout < u32Min) {
                    u32Min = u32Readout;
              }
         }
         u32Accumulator >>= 10;

         flMin = (418. - 0.5556 * (float)u32Max);
         flMax = (int) (418. - 0.5556 * (float)u32Min);
         flAccumulator = (int) (418. - 0.5556 * (float) u32Accumulator);

         post_log("\nTemperature: %d C (Max %d,Min %d) ", (int) flAccumulator, (int)flMax, (int) flMin);
         if( flMax < (flAccumulator + 4.5) && flMin > (flAccumulator-4.5)) {
     		post_log("        PASS\n");
	 }
         else {
                status = -1;
                post_log("        FAIL\n");
         }

         for(j = 0; j < 16; j++) {
              u32Accumulator = 0;
              u32Max = 0;
              u32Min = 0x3FF;

              u32Readout = readl(PVTMON_CONTROL0);
              u32Readout = u32Readout & 0xFFFFFFF1;      //set [3:1] = 111 to enable temperature monitor
              u32Readout = u32Readout | 0xE;
              writel(u32Readout, PVTMON_CONTROL0);

              u32Readout = 0x001D2003;
              writel(u32Readout, PVTMON_CONTROL1);

              u32Readout = u32Readout | (j<<3);
              writel(u32Readout, PVTMON_CONTROL1);
              udelay(1000);

              for( i = 0; i < 1024; i++) {
                   u32Readout = 0x3FF & readl(PVTMON_STATUS);
                   u32Accumulator += u32Readout;

                   if (u32Readout > u32Max) {
                        u32Max = u32Readout;
                   }
                   else if(u32Readout < u32Min) {
                        u32Min = u32Readout;
                   }
             }
             u32Accumulator >>= 10;

             post_log("(%2d/16)x i_AVDD1p0_0p9: %3d (code=%4d); ", (j+1), u32Accumulator, pTable[j]);
             if(u32Accumulator < (pTable[j]+15) && u32Accumulator > (pTable[j]-15)) {
                  post_log(" PASS\n");
             }
             else {
                  status = -1;
                  post_log(" FAIL\n");
             }

        }
        udelay(2000000);
    }
        return status;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_PVTMON */

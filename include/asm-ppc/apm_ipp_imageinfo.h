/**
 * APM APM86xxx SlimPRO Firmware Image Format
 *
 * Copyright (c) 2010, Applied Micro Circuits Corporation
 * Author: Anup Patel <apatel@apm.com>
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
 *
 */
#ifndef _IPP_IMAGEINFO_H__
#define _IPP_IMAGEINFO_H__

/** @name IPP Image Signture */
#define IPP_IMAGE_SIGNATURE		0x43434D41

/* iPP ROM image information */
#define IPP_ROM_TYPE			0x01
#define IPP_ROM_MAJOR			1
#define IPP_ROM_MINOR			0
#define IPP_ROM_LOADADDR		0x00000000

/** @name iPP RUNTIME image information */
/*@{*/
#define IPP_RUNTIME_TYPE		0x02
#define IPP_RUNTIME_MAJOR		1
#define IPP_RUNTIME_MINOR		0
#define IPP_RUNTIME_LOADADDR		0x00002000
/*@}*/

/** @name iPP EXTERNAL image information */
/*@{*/
#define IPP_EXTERNAL_TYPE		0x03
#define IPP_EXTERNAL_MAJOR		1
#define IPP_EXTERNAL_MINOR		0
#define IPP_EXTERNAL_LOADADDR		0x00000000
/*@}*/

/** @name iPP useful macro for image header */
/*@{*/
#define IPP_DECLARE_IMAGEINFO(type,major,minor,loadaddr,size) __attribute__ ((section(".info"))) ipp_imageinfo_t ipp_imginfo = {IPP_IMAGE_SIGNATURE,type,major,minor,loadaddr,size};
#define IPP_IMAGEINFO_SIGNATURE			(ipp_imginfo.image_signature)
#define IPP_IMAGEINFO_TYPE			(ipp_imginfo.image_type)
#define IPP_IMAGEINFO_MAJOR			(ipp_imginfo.image_major)
#define IPP_IMAGEINFO_MINOR			(ipp_imginfo.image_minor)
#define IPP_IMAGEINFO_LOADADDR			(ipp_imginfo.image_loadaddr)
#define IPP_IMAGEINFO_SIZE			(ipp_imginfo.image_size)
/*@}*/

/** @name PPC useful macro for image header */
/*@{*/
#define IPP_IMAGEINFO_OFFSET			0xF0
#define IPP_IMAGEINFO_POINTER(base)		((ipp_imageinfo_t *)(base + IPP_IMAGEINFO_OFFSET))
#define IPP_IMAGEINFO_PTR_SIGNATURE(ptr)	(ptr->image_signature)
#define IPP_IMAGEINFO_PTR_TYPE(ptr)		(ptr->image_type)
#define IPP_IMAGEINFO_PTR_MAJOR(ptr)		(ptr->image_major)
#define IPP_IMAGEINFO_PTR_MINOR(ptr)		(ptr->image_minor)
#define IPP_IMAGEINFO_PTR_LOADADDR(ptr)		(ptr->image_loadaddr)
#define IPP_IMAGEINFO_PTR_SIZE(ptr)		(ptr->image_size)
/*@}*/

#ifndef __ASSEMBLY__

/** 
 * iPP image header information 
 *
 * This header is appended to all runtime image binary. It is appended right 
 * before the code entry point. This structure is aligned to 16 bytes and
 * checked by ROM code/EXT Code. \n
 * WARNING: Don't change structure without understanding implications \n
 *          Don't INCREASE THIS STRUCTURE Beyond 16 bytes without changing 
 *           Code execution address
 */
struct ipp_imageinfo {
	unsigned int image_signature; 	/**< Signature of iPP image */
	unsigned short image_type; 	/**< Image type (ROM, RUNTIME, or EXTERNAL) */
	unsigned char image_major; 	/**< Major version number of image */
	unsigned char image_minor; 	/**< Minor version number of image */
	unsigned int image_loadaddr; 	/**< Address where the image is to be loaded */
	unsigned int image_size; 	/**< Size of image in bytes */
}__attribute__((packed));

/** Typename for iPP image header structure */
typedef struct ipp_imageinfo ipp_imageinfo_t;

#endif /* __ASSEMBLY__ */

#endif /* _IPP_IMAGEINFO_H__ */


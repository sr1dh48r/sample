/*HEADER**********************************************************************
******************************************************************************
***
*** Copyright (c) 2013, 2014 Imagination Technologies Ltd.
***
*** This program is free software; you can redistribute it and/or
*** modify it under the terms of the GNU General Public License
*** as published by the Free Software Foundation; either version 2
*** of the License, or (at your option) any later version.
***
*** This program is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*** GNU General Public License for more details.
***
*** You should have received a copy of the GNU General Public License
*** along with this program; if not, write to the Free Software
*** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
*** USA.
***
*** File Name  : ldr_parser.c
***
*** File Description:
*** This file has LDR parsing logic. 
***
******************************************************************************
*END**************************************************************************/

/******************************* Includes ************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "img_types.h"
#include "debug.h"
#include "ldr_info.h"
#include "ldr_parser.h"

#include "uccp_drv.h"

/******************************* Defines *************************************/

/******************************* Typedefs ************************************/

/*!****************************************************************************
 This typedef enumerates the possible types of information contained within an
 .ldr file.
******************************************************************************/

typedef enum
{
	/*! Default. Element is undefined. */
	LDR_NONE = 0,

	/*! Boot header. */
	LDR_BOOT_HEADER,

	/*! Secondary loader executable. */
	LDR_CODE,

	/*! Secondary loader top level data stream. */
	LDR_DATA_L1,

	/*! Secondary loader raw data stream. */
	LDR_DATA_L2

} t_LdrElementType;


/*!****************************************************************************
 This typedef declares a structure that represents a secondary loader top level
 data stream record.
******************************************************************************/

typedef struct s_L1Record
{
	/*! Command TagMember comment goes here. */
	unsigned short CmdTag;

	/*! Total length os this record. */
	unsigned short Length;

	/*! X25 CRC checksum for this record (including the checksum itself). */
	unsigned short CRC;

	/*! Offset within the .ldr to the next L1RECORD. */
	IMG_UINT32  Next;

	/*! The first command argument for the command. */
	IMG_UINT32  Arg1;

	/*! The second command argument for the command. */
	IMG_UINT32  Arg2;

	/*! Offset within the .ldr to the corresponding raw data record. */
	IMG_UINT32  L2Offset;

	/*! The expected length of the raw data record. */
	IMG_UINT32  L2Length;

} L1RECORD;

typedef struct _memhdr_tag 
{
	struct _memhdr_tag *pNext;
	
	/*! Target byte address */
	IMG_UINT32 Addr;		
	
	/*! Data block pointer */
	unsigned char *pData;	
	
	/*! Length of data block */
	IMG_UINT32 Bytes;	

} MEMHDR, *PMEMHDR;


/******************************* Global variables ****************************/

/******************************* Local variables *****************************/

/******************************* Local functions *****************************/

static unsigned short Read2 (unsigned char *pBuf);

static IMG_UINT32  Read4 (unsigned char *pBuf);

static unsigned VirtualToLinearOffset (unsigned PageSize, unsigned Offset);

/******************************* Code ****************************************/

/*!****************************************************************************

@Function    Read2

@Description Reads a 16-bit little endian value from the specified position in
	     a buffer.

@Input       pBuf    Position in buffer at which to read a 16-bit value

@Return      unsigned short  Value read.

******************************************************************************/

static unsigned short Read2 (unsigned char *pBuf)
{
	unsigned short Result = 0;

	Result  = pBuf[0];
	Result |= pBuf[1] << 8;

	return Result;
}

/*!****************************************************************************

@Function    Read4

@Description Reads a 32-bit little endian value from the specified position in
	     a buffer.

@Input       pBuf  Position in buffer at which to read a 32-bit value.

@Return      IMG_UINT32	Value read.

******************************************************************************/

static IMG_UINT32 Read4 (unsigned char *pBuf)
{
	IMG_UINT32 Result;

	Result  = pBuf[0];
	Result |= pBuf[1] << 8;
	Result |= pBuf[2] << 16;
	Result |= pBuf[3] << 24;

	return Result;
}

/*!****************************************************************************

@Function    VirtualToLinearOffset

@Description Converts a virtual (paged) offset to a linear (non-paged) offset.

@Input       PageSize	Prevailing page size.
@Input       Offset		Incoming virtual offset to be converted.

@Return      unsigned	Linear offset.

******************************************************************************/

static unsigned VirtualToLinearOffset (unsigned PageSize, unsigned Offset)
{
	static unsigned VPageSize = 0;

	unsigned Result = Offset;

	if (PageSize) {
		if (VPageSize == 0) {
			VPageSize = 1;
			while (VPageSize < PageSize)
				VPageSize <<= 1;
		}

		Result = ((Offset / VPageSize) * PageSize) + (Offset % VPageSize);
	}

	return Result;
}


/*!****************************************************************************

@Function    LdrParser

@Description Parses a LDR file and invokes corresponding ioctls.

@Input       pLdrFile	LDR file handle.
@Input       uccp_dev	Pointer to uccp char device.

@Return      int        SUCCESS or FAILED.

******************************************************************************/

int LdrParser (FILE *pLdrFile, struct uccp *uccp_dev)
{
	int         Quit;
	IMG_INT32   Next = 0;
	IMG_INT32   FileOffset = 0;
	IMG_INT32   PageSize = 0;
	IMG_INT32   OriginalOffset = 0;
	IMG_INT32   PrevOffset = 0;
	BOOTDEVHDR  BootDevHdr;
	int         Result = SUCCESS;
	
#if FILE_DUMP
	FILE        *file_dump = NULL;	
#endif
    	
	/* Lets really do it! */
	size_t Count = fread(&BootDevHdr, sizeof(BOOTDEVHDR), 1, pLdrFile);

	if(Count != 1) 
		Result = -1;

	Info ("DevID:  0x%08X\n", BootDevHdr.DevID);
	Info ("SLCode: 0x%08X\n", BootDevHdr.SLCode);
	Info ("SLData: 0x%08X\n", BootDevHdr.SLData);
	Info ("PLCtrl: 0x%04X\n", BootDevHdr.PLCtrl);
	Info ("CRC:    0x%04X\n", BootDevHdr.CRC);
	Info ("%d", sizeof(BootDevHdr));
	Info ("\n");

	FileOffset = VirtualToLinearOffset (PageSize, BootDevHdr.SLCode);
	OriginalOffset = VirtualToLinearOffset (PageSize, BootDevHdr.SLData);

	Next = sizeof(BOOTDEVHDR);

	if (Result == SUCCESS) {
		do {
			unsigned char SecLoadCode[PLRECORD_BYTES];

			if(fseek(pLdrFile, FileOffset, SEEK_SET) != 0) {
				Error("Can't seek file\n");
				Result = -1;
				break;
			}
			else {
				size_t Count = fread(&SecLoadCode, 
						     PLRECORD_BYTES,
						     1, 
						     pLdrFile);

				if (Count != 1) { 
					Result = -1;
					break;
				}
				
				Next = Read4(&SecLoadCode[PLRECORD_DATA_BYTES]);
				
				FileOffset = VirtualToLinearOffset(PageSize, 
								   Next);
			}
		} while (Next);

		FileOffset = OriginalOffset;
	}

#if FILE_DUMP
	file_dump = fopen(dump_file, "wb");
#endif
	/* Ok, read the main data stream. */
	Quit = (Result == SUCCESS) ? 0 : 1;

	while (!Quit) {
		char          InfoBuf[256];
		char         *pCfgStr = NULL;
		L1RECORD      L1Record;
		unsigned char L1Buf[L1_MAXSIZE];
		IMG_INT32     SeekOffset;

		if (fseek (pLdrFile, FileOffset, SEEK_SET)) {
			/* It's failed so report the error */
			Error ("Can't seek file\n");
			Result = -1;
			break;
		}
		else {
			unsigned char *pL2Buf = NULL, *pL2Blk = NULL;
			IMG_UINT32 L2Len = 0U;
			size_t Count;

			/* Read the L1 Record */
			Count = fread(&L1Buf, L1_MAXSIZE, 1, pLdrFile);

			if (Count != 1) {
				Result = -1;
				break;
			}

			L1Record.CmdTag = Read2(&L1Buf[L1_CMD]);
			L1Record.Length = Read2(&L1Buf[L1_LENGTH]);
			L1Record.Next   = Read4(&L1Buf[L1_NEXT]);

			if ((L1Record.Length > L1_MAXSIZE) ||
			    (L1Record.Length < L1_L2OFFSET) ||
			    (L1Record.Length < L1_L2LENGTH)){
				Error("Maximum L1 length exceeded\n");
				Result = -1;
				break;
			}

			/* Extract generic L1 fields */
			L1Record.L2Offset = Read4(&L1Buf[L1Record.Length - 
						  L1_L2OFFSET]);
			L1Record.L2Length = Read2(&L1Buf[L1Record.Length - 
						  L1_L2LENGTH]);

			if (L1Record.L2Length > L2_BASIC_SIZE) {
				/* Read the L2 data */
				size_t Count;
				SeekOffset = VirtualToLinearOffset(PageSize, 
								   L1Record.L2Offset);
				
				if (fseek(pLdrFile, SeekOffset, SEEK_SET)) {
					Error("Can't seek file");
					Result = -1;
					break; 
				}
				
				if (L1Record.L2Length > L2_MAXSIZE) {
					Error("Maximum L2 length exceeded\n");
					Result = -1;
					break;
				}

				pL2Blk = malloc(L1Record.L2Length + 1);

				if (pL2Blk == NULL) {
					Error("Not Enough Memory\n");
					Result = -1;
					break;
				}
				
				Count = fread(pL2Blk, 
					      L1Record.L2Length, 
					      1, 
					      pLdrFile);
				
				if (Count != 1) {
					Result = -1;
					free(pL2Blk);
					pL2Blk = NULL;
					break;
				}
					
				pL2Blk[L1Record.L2Length] = '\0';

				pL2Buf = pL2Blk + 
					(L2_CMD_SIZE + L2_LENGTH_SIZE);

				L2Len = L1Record.L2Length - L2_BASIC_SIZE;
			}

			switch (L1Record.CmdTag) {
				case e_CTLoadMem:
					{
					struct img_load_mem lm_v;

					if (!pL2Buf) {
						Error("Invalid params to Load "
						      "Mem\n");
						Result = -1;
						Quit = 1;
						break;
					}

					/* Load mem record */
					L1Record.Arg1 = Read4 (&L1Buf[L1_ARG1]);

					snprintf (InfoBuf, 
						  sizeof(InfoBuf), 
						  "%-12s: Addr: 0x%08X:"
						  " Size: 0x%08X\n", 
						  "LoadMem", 
						  L1Record.Arg1, 
						  L2Len);

					lm_v.DstAddr = L1Record.Arg1;
					lm_v.Length = L2Len; 
					lm_v.pSrcBuf = pL2Buf;
					
#if FILE_DUMP
					fwrite(lm_v.pSrcBuf, 
					       4, 
					       (lm_v.Length / 4), file_dump);
#endif
					
					if (ioctl(uccp_dev->m_fd, 
						  UCCP_LOAD_MEMORY, 
						  &lm_v)) {
						Error("LoadMem ioctl failed\n");
						Result = -1;
						Quit = 1;
					}

					}
					break;

				case e_CTStartThrds:
					{
					/* Start each thread with initial SP */
					char  CfgBuf[256];
					char  *pCfgBuf = NULL;
					
					if (!pL2Buf) {
						Error("Invalid params to Start "
						      "Threads\n");
						Result = -1;
						Quit = 1;
						break;
					}

					CfgBuf[0] = '\0';
					pCfgBuf = CfgBuf;

					while (L2Len > 0) {
						struct img_thrd_info tinfo_v;
						
						snprintf(pCfgBuf, 
							 sizeof(CfgBuf), 
							 "\tThrd %d: "
							 "SP: 0x%08X: "
							 "PC: 0x%08X: "
							 "Catch: 0x%08X\n", 
							 Read4 (pL2Buf), 
							 Read4 (pL2Buf + 4), 
							 Read4 (pL2Buf + 8), 
							 Read4 (pL2Buf + 12));

						tinfo_v.ThreadNumber = 
							Read4(pL2Buf);
						tinfo_v.StackPointer = 
							Read4(pL2Buf + 4);
						tinfo_v.ProgramCounter = 
							Read4(pL2Buf + 8);
						tinfo_v.CatchStateAddress = 
							Read4(pL2Buf + 12);
						
						if(ioctl(uccp_dev->m_fd, 
						      UCCP_START_THREAD, 
						      &tinfo_v)) {
							Error("StartThreads "
							      "ioctl failed\n");
							Result = -1;
							Quit = 1;
							break;
						}

						pL2Buf += (4 * 
							   sizeof(IMG_UINT32));
						L2Len -= (4 * 
							  sizeof(IMG_UINT32));
						pCfgBuf += strlen (pCfgBuf);
					}

					snprintf (InfoBuf, 
						  sizeof(InfoBuf), 
						  "%-12s:\n%s", 
						  "StartThrds", 
						  CfgBuf);

					}
					break;

				case e_CTZeroMem:
					{
					struct img_load_mem zm_v;

					/* Zero memory */
					L1Record.Arg1 = Read4 (&L1Buf[L1_ARG1]);
					L1Record.Arg2 = Read4 (&L1Buf[L1_ARG2]);

					snprintf (InfoBuf, sizeof(InfoBuf),  
						 "%-12s: Addr: 0x%08X: "
						 "Size: 0x%08X\n",
						 "ZeroMem", L1Record.Arg1, 
						 L1Record.Arg2);
					
					zm_v.DstAddr = L1Record.Arg1;
					zm_v.Length = L1Record.Arg2;
					
					if (ioctl(uccp_dev->m_fd, 
					      UCCP_ZERO_MEMORY, 
					      &zm_v)) {
						Error("ZeroMem ioctl failed\n");
						Result = -1;
						Quit = 1;
					}

					}

					break;

				case e_CTConfig:
					{
					/* Configuration commands */
					char *pCfgStrCur = NULL; 
					char *pCfgStrEnd = NULL;
					char *pCfgStrNew = NULL;

					int BufLen = (L1Record.L2Length/8)*40;

					if (!pL2Buf) {
						Error("Invalid params to "
						      "Config command\n");
						Result = -1;
						Quit = 1;
						break;
					}

					pCfgStr = malloc(BufLen);
					
					if (pCfgStr == NULL) {
						Error("Not enough memory to "
						      "allocate pCfgStr\n");
					}
					else {
						pCfgStrCur = pCfgStr;
						pCfgStrEnd = pCfgStr + BufLen;
					}

					do {
						int RecLen = 0, Len = 0;
						IMG_UINT32 Cmd = Read4(pL2Buf);

						if (pCfgStrCur && pCfgStr) {
							if ((pCfgStrEnd - 
							     pCfgStrCur) < 256) {
								size_t Posn = pCfgStrCur - 
									pCfgStr;

								/* Extend buffer */
								BufLen *= 2;
								pCfgStrNew = realloc(pCfgStr, 
										     BufLen);

								if (pCfgStrNew == NULL) {
									Error("Not enough memory to allocate pCfgStr\n");
									free(pCfgStr);
									pCfgStr = NULL;
									pCfgStrCur = NULL;
									pCfgStrEnd = NULL;
								}
								else {
									pCfgStr	= pCfgStrNew;
									/* Relocate pointers */
									pCfgStrCur = pCfgStr + 
										Posn;
									pCfgStrEnd = pCfgStr + 
										BufLen;
								}
							}
						}

						switch (Cmd) {
							case e_CIRead:
								{
								struct img_cfg_rw rw_v;
								
								if (pCfgStrCur) {
									Len = snprintf(pCfgStrCur, 
										       BufLen, 
										       "\tRead : 0x%08X\n", 
										       Read4 (&pL2Buf[4]));
								}

								RecLen = 8;

								rw_v.Addr = Read4(&pL2Buf[4]);
								
								if (ioctl(uccp_dev->m_fd, 
									  UCCP_CONFIG_READ, 
									  &rw_v)) {
									Error("Config Read ioctl failed\n");
									Result = -1;
									Quit = 1;
								}

								/* Value read is in rw_v */
								}
									
								break;

							case e_CIWrite:
							 	{
								struct img_cfg_rw rw_v;
								
								if (pCfgStrCur) {
									Len = snprintf(pCfgStrCur, 
										       BufLen, 
										       "\tWrite: 0x%08X: 0x%08X\n", 
										       Read4 (&pL2Buf[4]), 
										       Read4 (&pL2Buf[8]));
								}

								RecLen = 12;

								rw_v.Addr = Read4(&pL2Buf[4]);
								rw_v.Val =  Read4(&pL2Buf[8]);
								
#if FILE_DUMP
								fwrite(&rw_v.Val, 
								       4, 1, 
								       file_dump);
#endif
								
								if (ioctl(uccp_dev->m_fd, 
									  UCCP_CONFIG_WRITE, 
									  &rw_v)) {
									Error("Config Write ioctl failed\n");
									Result = -1;
									Quit = 1;
								}

								}
								
							 	break;

							case e_CIUser:
								if (pCfgStrCur) {
									Len = snprintf(pCfgStrCur, 
										       BufLen, 
										       "\tUser: 0x%08X: 0x%08X: 0x%08X: 0x%08X\n", 
										       Read4 (&pL2Buf[4]), 
										       Read4 (&pL2Buf[8]), 
										       Read4 (&pL2Buf[12]), 
										       Read4 (&pL2Buf[16]));
								}

								RecLen = 20;
							 	
								break;

							default:
								if (pCfgStrCur) {
									Len = snprintf(pCfgStrCur, 
										       BufLen, 
										       "\tUnknown: %08X (%d bytes remain)\n", 
										       Cmd, 
										       L2Len);
								}

							 	break;
						}

						if ((RecLen == 0) || 
						    (Result == -1)) 
							break;

						if (pCfgStrCur)
							pCfgStrCur += Len;

						pL2Buf += RecLen;
						L2Len -= RecLen;
					} while( L2Len > 0 );

					snprintf(InfoBuf, sizeof(InfoBuf),  
						 "%-12s: %d bytes %s\n", 
						 "Config",
						 (unsigned int)(pL2Buf-pL2Blk), 
						 ((L2Len != 0 ) ? ": ERROR!!" : ""));
					}

					break;

				case e_CTFileName:
					{
					if (!pL2Blk) {
						Error("Invalid params to "
						      "Filename\n");
						Result = -1;
						Quit = 1;
						break;
					}

					snprintf(InfoBuf, sizeof(InfoBuf), 
						 "%-12s: %s\n", "FileName", 
						 pL2Blk + 8);
					}
					break;

				default:
					/* Not expected */
					snprintf(InfoBuf, sizeof(InfoBuf), 
						 "%-12s\n", "Unknown");
					break;
			}

			if (pL2Blk) { 
				free (pL2Blk);
				pL2Blk = NULL;
			}
		}

		if (pCfgStr) {
			Info("0x%08X: %s%s", FileOffset, InfoBuf, pCfgStr);
			free(pCfgStr);
			pCfgStr = NULL;
		}
		else {
			Info("0x%08X: %s ", FileOffset, InfoBuf);
		}

		if (L1Record.Next == L1_TERMINATE) {
			IMG_UINT32 OverlayOffset = 0;

			/* There is the possibility of further overlays. 
			 * Without additional information, the best guess is 
			 * that they start immediately after the L2 data of the 
			 * last record. */
			L1Record.L2Offset = Read4(&L1Buf[L1Record.Length - 
						  L1_L2OFFSET]);
			L1Record.L2Length = Read2(&L1Buf[L1Record.Length - 
						  L1_L2LENGTH]);

			OverlayOffset = VirtualToLinearOffset (PageSize, 
							       L1Record.L2Offset);
			OverlayOffset += L1Record.L2Length;

			/* Round to next 32-bit boundary */
			OverlayOffset += 3;
			OverlayOffset &= ~3;

			Info ("\n");
			Info ("Next L1 Record Sequence Best Guess at "
			      "File Offset 0x%08X\n",
			      OverlayOffset);

			Quit = 1;
		}
		else {
			/* Move on to next L1 record */
			PrevOffset = FileOffset;
			FileOffset = VirtualToLinearOffset(PageSize, 
							   L1Record.Next);

			if (FileOffset <= PrevOffset) {
				Error ("Out of sequence record found. Possibly "
				       "incorrect page size specified. "
				       "Stopping.\n");
				Quit = 1;
			}
		}
	}

	
#if FILE_DUMP
	fclose(file_dump);	
#endif
	return Result;
}

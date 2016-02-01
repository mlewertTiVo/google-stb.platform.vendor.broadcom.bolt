/***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __DISASM_H__
#define __DISASM_H__

/* Returns a pointer to a read-only string containing the intstruction name */
char *disasm_inst_name(uint32_t inst);

/* Copies a disassembled version of the instruction into buf, null terminating the
string.  Will not exceed buf_size bytes written; if the disassembled string is
longer than buf_size, buf_size-1 bytes of the string will be written and that string
will be null-terminated */
void disasm_inst(char *buf, int buf_size, uint32_t inst, uint64_t pc);

#endif /* __DISASM_H__ */


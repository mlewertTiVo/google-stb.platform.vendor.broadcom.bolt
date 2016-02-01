/***************************************************************************
 *     Copyright (c) 2012-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************/

#ifndef __MACADDR_H__
#define __MACADDR_H__

/* older chips used a 0x100 increment
*/
#define MACADDR_INCREMENT 1


/* MAC flash reading. Write is in the ui commands as 
 customers may take that out but we might require
 this feature on its own.
*/
void macaddr_flash_get(int addrnum, char *macstr, int flashprog_off);
int  macaddr_flash_verify(int flashprog_off);
void macaddr_increment(uint8_t *macaddr, int increment);
void macaddr_decrement(uint8_t *macaddr, int decrement);

#endif /* __MACADDR_H__ */

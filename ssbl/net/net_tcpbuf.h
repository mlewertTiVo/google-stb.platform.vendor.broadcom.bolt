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

/*  *********************************************************************
    *  Macros
    ********************************************************************* */

#define tmb_remlen(tmb) ((tmb)->tmb_bufsize - (tmb)->tmb_len)
#define tmb_curlen(tmb) ((tmb)->tmb_len)

/*  *********************************************************************
    *  Modulo Buffer
    ********************************************************************* */

typedef struct tcpmodbuf_s {
	uint8_t *tmb_buf;	/* Buffer */
	int tmb_bufsize;	/* size of buffer */
	int tmb_addptr;		/* current "add" pointer */
	unsigned int tmb_remptr;	/* current "remove" pointer */
	int tmb_len;		/* amount of data in the buffer */
} tcpmodbuf_t;

/*  *********************************************************************
    *  Prototypes
    ********************************************************************* */

void tmb_init(tcpmodbuf_t * buf);
void tmb_adjust(tcpmodbuf_t * buf, int amt);
int tmb_alloc(tcpmodbuf_t * buf, int size);
void tmb_free(tcpmodbuf_t * buf);
int tmb_copyin(tcpmodbuf_t * tmb, uint8_t * buf, int len, int update);
int tmb_copyout(tcpmodbuf_t * tmb, uint8_t * buf, int len, int update);
int tmb_copyout2(tcpmodbuf_t * tmb, uint8_t * buf, int offset, int len);

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
    *  TCP Constants
    ********************************************************************* */

#define TCP_MAX_PORTS	8
#define TCP_MAX_TCBS	16
#define TCP_BUF_SIZE	(32 * 1024)

#define TCP_MAX_SEG_SIZE 1400

#define TCP_CONNECT_TIMER  (30*BOLT_HZ)
#define TCP_RETX_TIMER	   (1*BOLT_HZ)
#define TCP_TIMEWAIT_TIMER (30*BOLT_HZ)
#define TCP_SEND_TIMER	   (BOLT_HZ)

#define TCP_FAST_TIMER	   (BOLT_HZ/5)	/* 200ms */
#define TCP_KEEPALIVE_TIMER  (BOLT_HZ*60)	/* one minute */

/*  *********************************************************************
    *  TCP Protocol
    ********************************************************************* */

#define TCPFLG_FIN	0x0001
#define TCPFLG_SYN	0x0002
#define TCPFLG_RST	0x0004
#define TCPFLG_PSH	0x0008
#define TCPFLG_ACK	0x0010
#define TCPFLG_URG	0x0020
#define TCPHDRSIZE(flg)	(((flg) >> 12)*4)
#define TCPHDRFLG(size) (((size)/4)<<12)

#define TCP_HDR_LENGTH	20

#define TCP_MAX_SEG_OPT	0x0204

#define TCP_MAX_OOS_BUFS 64

/*  *********************************************************************
    *  TCP State machine
    ********************************************************************* */

/* 
 * TCB states, see RFC 
 */

#define TCPSTATE_CLOSED		0
#define TCPSTATE_LISTEN		1
#define TCPSTATE_SYN_SENT	2
#define TCPSTATE_SYN_RECEIVED	3
#define TCPSTATE_ESTABLISHED	4
#define TCPSTATE_CLOSE_WAIT	5
#define TCPSTATE_FINWAIT_1	6
#define TCPSTATE_FINWAIT_2	7
#define TCPSTATE_CLOSING	8
#define TCPSTATE_LAST_ACK	9
#define TCPSTATE_TIME_WAIT	10

/*
 * Masks for TCP states - we use these so we can make 
 * bit vectors of states for easy tests.
 */

#define M_TCPSTATE_CLOSED	(1 << TCPSTATE_CLOSED)
#define M_TCPSTATE_LISTEN	(1 << TCPSTATE_LISTEN)
#define M_TCPSTATE_SYN_SENT	(1 << TCPSTATE_SYN_SENT)
#define M_TCPSTATE_SYN_RECEIVED	(1 << TCPSTATE_SYN_RECEIVED)
#define M_TCPSTATE_ESTABLISHED	(1 << TCPSTATE_ESTABLISHED)
#define M_TCPSTATE_CLOSE_WAIT	(1 << TCPSTATE_CLOSE_WAIT)
#define M_TCPSTATE_FINWAIT_1	(1 << TCPSTATE_FINWAIT_1)
#define M_TCPSTATE_FINWAIT_2	(1 << TCPSTATE_FINWAIT_2)
#define M_TCPSTATE_CLOSING	(1 << TCPSTATE_CLOSING)
#define M_TCPSTATE_LAST_ACK	(1 << TCPSTATE_LAST_ACK)
#define M_TCPSTATE_TIME_WAIT	(1 << TCPSTATE_TIME_WAIT)
#define M_TCPSTATE_CLOSED	(1 << TCPSTATE_CLOSED)

/*
 * This macro returns true if a given state is in a 
 * set of states (defined below)
 */

#define TCPSTATE_IN_SET(state,set) ((1 << (state)) & (set))

/*
 * Intresting groups of TCP states
 */

/* ABORTSTATES - tcp_abort will send a RST if our TCB is one of these. */
#define M_TCPSTATE_ABORTSTATES \
       M_TCPSTATE_SYN_SENT | M_TCPSTATE_SYN_RECEIVED | M_TCPSTATE_ESTABLISHED | \
       M_TCPSTATE_FINWAIT_1 | M_TCPSTATE_FINWAIT_2 | M_TCPSTATE_CLOSING | \
       M_TCPSTATE_LAST_ACK | M_TCPSTATE_CLOSE_WAIT

/* SEND_OK - tcp_send will send data if our TCB is one of these */
#define M_TCPSTATE_SEND_OK \
       M_TCPSTATE_ESTABLISHED | M_TCPSTATE_CLOSE_WAIT

/* RECV_OK - tcp_recv will pass up data if our TCB is in one of these */
#define M_TCPSTATE_RECV_OK \
       M_TCPSTATE_ESTABLISHED | M_TCPSTATE_CLOSE_WAIT

/* PEERADDR_OK - tcp_peeraddr will return a value if TCB is in one of these */
#define M_TCPSTATE_PEERADDR_OK \
       M_TCPSTATE_SYN_SENT | M_TCPSTATE_SYN_RECEIVED | M_TCPSTATE_ESTABLISHED | \
       M_TCPSTATE_FINWAIT_1 | M_TCPSTATE_FINWAIT_2 | M_TCPSTATE_CLOSING | \
       M_TCPSTATE_LAST_ACK | M_TCPSTATE_CLOSE_WAIT

/* RESETKEEPALIVE - reset keepalive timer in these states */
#define M_TCPSTATE_RESETKEEPALIVE \
       M_TCPSTATE_ESTABLISHED | M_TCPSTATE_CLOSE_WAIT | M_TCPSTATE_FINWAIT_1 | \
       M_TCPSTATE_FINWAIT_2 | M_TCPSTATE_CLOSING | M_TCPSTATE_LAST_ACK

#define CONNINPROGRESS - connection is in progress in these states */
#define M_TCPSTATE_CONNINPROGRESS \
       M_TCPSTATE_LISTEN | M_TCPSTATE_SYN_SENT | M_TCPSTATE_SYN_RECEIVED

/*
 * TCP flags for the control block 
 */

#define TCB_FLG_OUTPUT		1	/* need to call tcp_output */
#define TCB_FLG_SENDMSG		2
#define TCB_FLG_DLYACK		4	/* delayed ack is pending */

/*  *********************************************************************
    *  TCP Control Block
    ********************************************************************* */

typedef struct tcb_s {
	queue_t tcb_qb;		/* next/previous in list */
	int tcb_socknum;	/* socket number, index into table */

	int tcb_state;		/* current connection state */

	uint8_t tcb_peeraddr[IP_ADDR_LEN];	/* Peer's IP Address */
	uint16_t tcb_peerport;	/* Peer's port address */
	uint16_t tcb_lclport;	/* My port address */

	uint16_t tcb_txflags;	/* packet flags for next tx packet */

	bolt_timer_t tcb_timer_2msl;	/* 2MSL timer, used in TIME_WAIT */
	bolt_timer_t tcb_timer_keep;	/* Timer for keepalives and connections */
	bolt_timer_t tcb_timer_retx;	/* send retransmission timer */
	bolt_timer_t tcb_timer_pers;	/* Persist timer */

	int tcb_retrycnt;	/* Retry counter */

	int tcb_mtu;		/* MTU if underlying link */
	unsigned int tcb_flags;	/* Misc protocol flags */
	unsigned int tcb_sockflags;	/* flags set by user api */

	/*
	 * Buffers
	 */

	tcpmodbuf_t tcb_txbuf;	/* Transmit buffer */
	tcpmodbuf_t tcb_rxbuf;	/* Receive buffer */

	/*
	 * Send sequence variables
	 */

	uint32_t tcb_sendunack;	/* oldest unacknowledged seqnum */
	uint32_t tcb_sendnext;	/* Next seqnum to send */
	uint32_t tcb_sendwindow;	/* Last advertised send window */

	/*
	 * Receive sequence variables
	 */

	uint32_t tcb_rcvnext;	/* next in-order receive seq num */
	uint32_t tcb_rcvack;	/* last transmitted acknowledgement */

	/*
	 * Window management variables
	 */
	int tcb_dup_ackcnt;	/* Duplicate ack counter */

	/*
	 * Out-of-sequence variables
	 */
	struct {
		uint32_t seqnum;
		uint16_t flags;
		uint16_t datalen;
		uint8_t *buf;
	} tcb_oos_buffers[TCP_MAX_OOS_BUFS];
	int tcb_oos;
	int tcb_oos_max;
	int tcb_oos_err_toomany;
	int tcb_oos_err_nomemory;
	int tcb_oos_err_badsync;
} tcb_t;

/*  *********************************************************************
    *  Macros to muck with sequence numbers
    ********************************************************************* */

#define TCPSEQ_LT(a,b)     ((int)((a)-(b)) < 0)
#define TCPSEQ_LEQ(a,b)    ((int)((a)-(b)) <= 0)
#define TCPSEQ_GT(a,b)     ((int)((a)-(b)) > 0)
#define TCPSEQ_GEQ(a,b)    ((int)((a)-(b)) >= 0)

#define TCPSEQ_DIFF(a,b)   ((int)((a)-(b)))

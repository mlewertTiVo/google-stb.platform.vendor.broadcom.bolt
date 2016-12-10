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

#include <common.h>
#include <lib_types.h>
#include <fsbl-common.h>
#include <fsbl.h>

#include <bchp_bsca.h> /* for masks etc. h/w is the same */
#include <fsbl-i2c.h>


/* This value should never change.
*/
#define BOARD_ID_CHIP_ADDR 0x3E

/*
	Keep this in sync with existing hardware
*/
typedef struct bsc_regs_s {
	uint32_t chip_address; // BSC Chip Address And Read/Write Control
	uint32_t data_in0;     // BSC Write Data Register 0
	uint32_t data_in1;     // BSC Write Data Register 1
	uint32_t data_in2;     // BSC Write Data Register 2
	uint32_t data_in3;     // BSC Write Data Register 3
	uint32_t data_in4;     // BSC Write Data Register 4
	uint32_t data_in5;     // BSC Write Data Register 5
	uint32_t data_in6;     // BSC Write Data Register 6
	uint32_t data_in7;     // BSC Write Data Register 7
	uint32_t cnt_reg;	   // BSC Transfer Count Register
	uint32_t ctl_reg;	   // BSC Control Register
	uint32_t iic_enable;   // BSC Read/Write Enable And Interrupt
	uint32_t data_out0;    // BSC Read Data Register 0
	uint32_t data_out1;    // BSC Read Data Register 1
	uint32_t data_out2;    // BSC Read Data Register 2
	uint32_t data_out3;    // BSC Read Data Register 3
	uint32_t data_out4;    // BSC Read Data Register 4
	uint32_t data_out5;    // BSC Read Data Register 5
	uint32_t data_out6;    // BSC Read Data Register 6
	uint32_t data_out7;    // BSC Read Data Register 7
	uint32_t ctlhi_reg;    // BSC Control Register
	uint32_t scl_param;    // BSC SCL Parameter Register
} bsc_regs_t;


static uint32_t get_i2c_id(volatile bsc_regs_t *i2c)
{
	volatile uint32_t v;
	signed int count = 10; /* ~10msec */

	report_hex("@I2C @ ", (uint32_t)i2c);

	/* make sure its off
	*/
	i2c->iic_enable = 0;

	/* set clocks, everything else is set to its default value (0)
	*/
	v = (0x03 << BCHP_BSCA_CTL_REG_SCL_SEL_SHIFT); /* 50Khz/200Khz */
	v |= BCHP_BSCA_CTL_REG_DIV_CLK_MASK; /* 50Khz */
	/*
		Combined Write-then-Read format. Master repeats START condition
		without asserting STOP condition to change operation from write to read
	*/
	i2c->ctl_reg = v | 3;

	/* 7 bit address
	*/
	i2c->chip_address = (uint32_t)(((BOARD_ID_CHIP_ADDR & 0x7f) << 1));

	/* write one byte (command), read one byte (register)
	*/
	i2c->cnt_reg =
			(1 << BCHP_BSCA_CNT_REG_CNT_REG1_SHIFT)|
			(1 << BCHP_BSCA_CNT_REG_CNT_REG2_SHIFT);

	/* write cmd to select reg #0 to read
	*/
	i2c->data_in0 = 0;

	/* start the i2c state machine
	*/
	i2c->iic_enable |= BCHP_BSCA_IIC_ENABLE_ENABLE_MASK;

	do {
		__puts(".");
		sleep_ms(1);

		v = i2c->iic_enable;

	} while (!(v & BCHP_BSCA_IIC_ENABLE_INTRP_MASK) && count--);

	/* turn it off
	*/
	i2c->iic_enable = 0;

	if (0 > count)
		putchar('X'); /* timeout */

	__puts("\nBID: ");
	if (v & BCHP_BSCA_IIC_ENABLE_NO_ACK_MASK) {
		__puts("no ack!");
		v = 0;
	} else {
		v = i2c->data_out0 & 0xff;
		writehex(v);
	}

	crlf();
	return v;
}


uint8_t get_ext_board_id(void)
{
	uint32_t v = 0, n = 0;
	volatile bsc_regs_t *i2c;

	while (fsbl_i2c_bus[n] != 0) {
		i2c = (volatile bsc_regs_t *)BPHYSADDR(fsbl_i2c_bus[n]);
		v = get_i2c_id(i2c);
		if (v)
			break;
		n++;
	}
	return (uint8_t)v;
}

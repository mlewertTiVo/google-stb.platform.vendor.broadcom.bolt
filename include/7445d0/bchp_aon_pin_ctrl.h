/***************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on              Mon Apr 28 03:11:15 2014
 *                 Full Compile MD5 Checksum bfcf7125bf05811fa35815b8286b23a0
 *                   (minus title and desc)  
 *                 MD5 Checksum              6043e9abcf818590a0d0d2cb13ebaf4c
 *
 * Compiled with:  RDB Utility               combo_header.pl
 *                 RDB Parser                3.0
 *                 unknown                   unknown
 *                 Perl Interpreter          5.008008
 *                 Operating System          linux
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/

#ifndef BCHP_AON_PIN_CTRL_H__
#define BCHP_AON_PIN_CTRL_H__

/***************************************************************************
 *AON_PIN_CTRL - AON Pinmux Control Registers
 ***************************************************************************/
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0         0x00410700 /* Pinmux control register 0 */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1         0x00410704 /* Pinmux control register 1 */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2         0x00410708 /* Pinmux control register 2 */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0     0x0041070c /* Pad pull-up/pull-down control register 0 */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1     0x00410710 /* Pad pull-up/pull-down control register 1 */
#define BCHP_AON_PIN_CTRL_BYP_CLK_UNSELECT_0     0x00410714 /* Bypass clock unselect register 0 */

/***************************************************************************
 *PIN_MUX_CTRL_0 - Pinmux control register 0
 ***************************************************************************/
/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_07 [31:28] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_MASK          0xf0000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_SHIFT         28
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_AON_GPIO_07   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_LED_LS_3      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_UART_RTS_0    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_07_LED_LD_15     3

/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_06 [27:24] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_MASK          0x0f000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_SHIFT         24
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_AON_GPIO_06   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_LED_LS_2      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_LED_LD_14     2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_06_SPI_M_SS2B    3

/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_05 [23:20] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_MASK          0x00f00000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_SHIFT         20
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_AON_GPIO_05   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_LED_LS_1      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_LED_LD_13     2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_05_SPI_M_SS1B    3

/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_04 [19:16] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_MASK          0x000f0000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_SHIFT         16
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_AON_GPIO_04   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_LED_LS_0      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_04_LED_LD_12     2

/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_03 [15:12] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_MASK          0x0000f000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_SHIFT         12
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_AON_GPIO_03   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_LED_KD_3      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_LED_LD_11     2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_I2S_LR0_IN    3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_I2S_LR0_OUT   4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_GPHY_ACTIVITY 5
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_03_PM_AON_GPIO_03 6

/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_02 [11:08] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_MASK          0x00000f00
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_SHIFT         8
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_AON_GPIO_02   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_LED_KD_2      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_LED_LD_10     2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_I2S_DATA0_IN  3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_I2S_DATA0_OUT 4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_MDIO_SATA     5
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_02_PM_AON_GPIO_02 6

/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_01 [07:04] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_MASK          0x000000f0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_SHIFT         4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_AON_GPIO_01   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_LED_KD_1      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_LED_LD_9      2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_I2S_CLK0_IN   3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_I2S_CLK0_OUT  4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_SATA_MDCLK    5
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_01_PM_AON_GPIO_01 6

/* AON_PIN_CTRL :: PIN_MUX_CTRL_0 :: aon_gpio_00 [03:00] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_MASK          0x0000000f
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_SHIFT         0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_AON_GPIO_00   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_LED_KD_0      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_LED_LD_8      2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_0_aon_gpio_00_PM_AON_GPIO_00 3

/***************************************************************************
 *PIN_MUX_CTRL_1 - Pinmux control register 1
 ***************************************************************************/
/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_15 [31:28] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_MASK          0xf0000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_SHIFT         28
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_AON_GPIO_15   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_LED_LD_6      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_UART_CTS_2    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_SPI_M_MISO    3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_GPHY_ACTIVITY 4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_MOCA_ACTIVITY 5
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_SW_LED_DATA   6
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_15_PM_AON_GPIO_15 7

/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_14 [27:24] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_MASK          0x0f000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_SHIFT         24
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_AON_GPIO_14   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_LED_LD_5      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_UART_TXD_2    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_SPI_M_MOSI    3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_14_ALT_TP_OUT_23 4

/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_13 [23:20] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_MASK          0x00f00000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_SHIFT         20
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_AON_GPIO_13   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_LED_LD_4      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_UART_RXD_2    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_SPI_M_SCK     3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_GPHY_LINK     4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_MOCA_LINK     5
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_SW_LED_CLK    6
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_ALT_TP_IN_22  7
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_13_PM_AON_GPIO_13 8

/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_12 [19:16] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_MASK          0x000f0000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_SHIFT         16
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_AON_GPIO_12   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_LED_LD_3      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_UART_CTS_1    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_I2S_LR0_OUT   3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_I2S_LR0_IN    4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_12_PM_AON_GPIO_12 5

/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_11 [15:12] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_MASK          0x0000f000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_SHIFT         12
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_AON_GPIO_11   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_LED_LD_2      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_11_UART_RTS_1    2

/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_10 [11:08] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_MASK          0x00000f00
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_SHIFT         8
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_AON_GPIO_10   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_LED_LD_1      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_UART_CTS_0    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_I2S_DATA0_OUT 3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_I2S_DATA0_IN  4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_10_PM_AON_GPIO_10 5

/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_09 [07:04] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_MASK          0x000000f0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_SHIFT         4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_AON_GPIO_09   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_LED_LD_0      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_UART_RXD_1    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_I2S_CLK0_OUT  3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_I2S_CLK0_IN   4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_IR_IN1        5
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_TP_IN_01      6
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_09_PM_AON_GPIO_09 7

/* AON_PIN_CTRL :: PIN_MUX_CTRL_1 :: aon_gpio_08 [03:00] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_MASK          0x0000000f
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_SHIFT         0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_AON_GPIO_08   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_LED_LS_4      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_UART_TXD_1    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_GPHY_LINK     3
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_1_aon_gpio_08_TP_OUT_01     4

/***************************************************************************
 *PIN_MUX_CTRL_2 - Pinmux control register 2
 ***************************************************************************/
/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_sgpio_05 [31:28] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_05_MASK         0xf0000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_05_SHIFT        28
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_05_DEFAULT      0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_05_AON_SGPIO_05 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_05_BSC_M2_SDA   1

/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_sgpio_04 [27:24] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_04_MASK         0x0f000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_04_SHIFT        24
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_04_DEFAULT      0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_04_AON_SGPIO_04 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_04_BSC_M2_SCL   1

/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_sgpio_03 [23:20] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_MASK         0x00f00000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_SHIFT        20
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_DEFAULT      0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_AON_SGPIO_03 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_BSC_M1_SDA   1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_HDMI_RX_BSC_SDA 2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_03_BSC_S1_SDA   3

/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_sgpio_02 [19:16] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_MASK         0x000f0000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_SHIFT        16
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_DEFAULT      0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_AON_SGPIO_02 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_BSC_M1_SCL   1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_HDMI_RX_BSC_SCL 2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_02_BSC_S1_SCL   3

/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_sgpio_01 [15:12] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_MASK         0x0000f000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_SHIFT        12
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_DEFAULT      0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_AON_SGPIO_01 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_BSC_M0_SDA   1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_01_HDMI_TX_BSC_SDA 2

/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_sgpio_00 [11:08] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_MASK         0x00000f00
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_SHIFT        8
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_DEFAULT      0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_AON_SGPIO_00 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_BSC_M0_SCL   1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_sgpio_00_HDMI_TX_BSC_SCL 2

/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_gpio_17 [07:04] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_MASK          0x000000f0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_SHIFT         4
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_DEFAULT       0x00000001
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_AON_GPIO_17   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_LED_OUT       1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_17_MOCA_LINK     2

/* AON_PIN_CTRL :: PIN_MUX_CTRL_2 :: aon_gpio_16 [03:00] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_MASK          0x0000000f
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_SHIFT         0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_DEFAULT       0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_AON_GPIO_16   0
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_LED_LD_7      1
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_UART_RTS_2    2
#define BCHP_AON_PIN_CTRL_PIN_MUX_CTRL_2_aon_gpio_16_SPI_M_SS0B    3

/***************************************************************************
 *PIN_MUX_PAD_CTRL_0 - Pad pull-up/pull-down control register 0
 ***************************************************************************/
/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: spare_pad_ctrl_0 [31:30] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_spare_pad_ctrl_0_MASK 0xc0000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_spare_pad_ctrl_0_SHIFT 30
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_spare_pad_ctrl_0_DEFAULT 0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_spare_pad_ctrl_0_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_spare_pad_ctrl_0_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_spare_pad_ctrl_0_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: aon_gpio_12_pad_ctrl [29:28] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_12_pad_ctrl_MASK 0x30000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_12_pad_ctrl_SHIFT 28
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_12_pad_ctrl_DEFAULT 0x00000001
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_12_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_12_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_12_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: reserved0 [27:26] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_reserved0_MASK        0x0c000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_reserved0_SHIFT       26

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: aon_gpio_10_pad_ctrl [25:24] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_10_pad_ctrl_MASK 0x03000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_10_pad_ctrl_SHIFT 24
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_10_pad_ctrl_DEFAULT 0x00000001
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_10_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_10_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_10_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: aon_gpio_09_pad_ctrl [23:22] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_09_pad_ctrl_MASK 0x00c00000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_09_pad_ctrl_SHIFT 22
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_09_pad_ctrl_DEFAULT 0x00000001
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_09_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_09_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_09_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: reserved1 [21:12] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_reserved1_MASK        0x003ff000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_reserved1_SHIFT       12

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: aon_gpio_03_pad_ctrl [11:10] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_03_pad_ctrl_MASK 0x00000c00
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_03_pad_ctrl_SHIFT 10
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_03_pad_ctrl_DEFAULT 0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_03_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_03_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_03_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: aon_gpio_02_pad_ctrl [09:08] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_02_pad_ctrl_MASK 0x00000300
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_02_pad_ctrl_SHIFT 8
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_02_pad_ctrl_DEFAULT 0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_02_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_02_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_02_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: aon_gpio_01_pad_ctrl [07:06] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_01_pad_ctrl_MASK 0x000000c0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_01_pad_ctrl_SHIFT 6
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_01_pad_ctrl_DEFAULT 0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_01_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_01_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_01_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: aon_gpio_00_pad_ctrl [05:04] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_00_pad_ctrl_MASK 0x00000030
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_00_pad_ctrl_SHIFT 4
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_00_pad_ctrl_DEFAULT 0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_00_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_00_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_aon_gpio_00_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_0 :: reserved2 [03:00] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_reserved2_MASK        0x0000000f
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_0_reserved2_SHIFT       0

/***************************************************************************
 *PIN_MUX_PAD_CTRL_1 - Pad pull-up/pull-down control register 1
 ***************************************************************************/
/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: reserved0 [31:24] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved0_MASK        0xff000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved0_SHIFT       24

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: spare_pad_ctrl_1 [23:22] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_spare_pad_ctrl_1_MASK 0x00c00000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_spare_pad_ctrl_1_SHIFT 22
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_spare_pad_ctrl_1_DEFAULT 0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_spare_pad_ctrl_1_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_spare_pad_ctrl_1_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_spare_pad_ctrl_1_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: reserved1 [21:10] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved1_MASK        0x003ffc00
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved1_SHIFT       10

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: aon_gpio_17_pad_ctrl [09:08] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_17_pad_ctrl_MASK 0x00000300
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_17_pad_ctrl_SHIFT 8
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_17_pad_ctrl_DEFAULT 0x00000000
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_17_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_17_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_17_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: reserved2 [07:06] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved2_MASK        0x000000c0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved2_SHIFT       6

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: aon_gpio_15_pad_ctrl [05:04] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_15_pad_ctrl_MASK 0x00000030
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_15_pad_ctrl_SHIFT 4
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_15_pad_ctrl_DEFAULT 0x00000001
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_15_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_15_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_15_pad_ctrl_PULL_UP 2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: reserved3 [03:02] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved3_MASK        0x0000000c
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_reserved3_SHIFT       2

/* AON_PIN_CTRL :: PIN_MUX_PAD_CTRL_1 :: aon_gpio_13_pad_ctrl [01:00] */
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_13_pad_ctrl_MASK 0x00000003
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_13_pad_ctrl_SHIFT 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_13_pad_ctrl_DEFAULT 0x00000001
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_13_pad_ctrl_PULL_NONE 0
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_13_pad_ctrl_PULL_DOWN 1
#define BCHP_AON_PIN_CTRL_PIN_MUX_PAD_CTRL_1_aon_gpio_13_pad_ctrl_PULL_UP 2

/***************************************************************************
 *BYP_CLK_UNSELECT_0 - Bypass clock unselect register 0
 ***************************************************************************/
/* AON_PIN_CTRL :: BYP_CLK_UNSELECT_0 :: reserved0 [31:01] */
#define BCHP_AON_PIN_CTRL_BYP_CLK_UNSELECT_0_reserved0_MASK        0xfffffffe
#define BCHP_AON_PIN_CTRL_BYP_CLK_UNSELECT_0_reserved0_SHIFT       1

/* AON_PIN_CTRL :: BYP_CLK_UNSELECT_0 :: unsel_byp_clk_on_aon_gpio_01 [00:00] */
#define BCHP_AON_PIN_CTRL_BYP_CLK_UNSELECT_0_unsel_byp_clk_on_aon_gpio_01_MASK 0x00000001
#define BCHP_AON_PIN_CTRL_BYP_CLK_UNSELECT_0_unsel_byp_clk_on_aon_gpio_01_SHIFT 0
#define BCHP_AON_PIN_CTRL_BYP_CLK_UNSELECT_0_unsel_byp_clk_on_aon_gpio_01_DEFAULT 0x00000000

#endif /* #ifndef BCHP_AON_PIN_CTRL_H__ */

/* End of File */

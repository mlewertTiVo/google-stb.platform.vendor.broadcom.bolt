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
 * Date:           Generated on              Thu May  1 03:10:38 2014
 *                 Full Compile MD5 Checksum 8857ca01a33020a966a61a719d48dd0d
 *                   (minus title and desc)  
 *                 MD5 Checksum              bca26501368a2f4627fe3f712f15f6b0
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

#ifndef BCHP_UARTB_H__
#define BCHP_UARTB_H__

/***************************************************************************
 *UARTB - UART B
 ***************************************************************************/
#define BCHP_UARTB_RBR                           0x2040a940 /* Receive Buffer Register */
#define BCHP_UARTB_THR                           0x2040a940 /* Transmit Holding Register */
#define BCHP_UARTB_DLH                           0x2040a944 /* Divisor Latch High */
#define BCHP_UARTB_DLL                           0x2040a940 /* Divisor Latch Low */
#define BCHP_UARTB_IER                           0x2040a944 /* Interrupt Enable Register */
#define BCHP_UARTB_IIR                           0x2040a948 /* Interrupt Identity Register */
#define BCHP_UARTB_FCR                           0x2040a948 /* FIFO Control Register */
#define BCHP_UARTB_LCR                           0x2040a94c /* Line Control Register */
#define BCHP_UARTB_MCR                           0x2040a950 /* Modem Control Register */
#define BCHP_UARTB_LSR                           0x2040a954 /* Line Status Register */
#define BCHP_UARTB_MSR                           0x2040a958 /* Modem Status Register */
#define BCHP_UARTB_SCR                           0x2040a95c /* Scratchpad Register */

/***************************************************************************
 *RBR - Receive Buffer Register
 ***************************************************************************/
/* UARTB :: RBR :: reserved0 [31:08] */
#define BCHP_UARTB_RBR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_RBR_reserved0_SHIFT                             8

/* UARTB :: RBR :: RBR [07:00] */
#define BCHP_UARTB_RBR_RBR_MASK                                    0x000000ff
#define BCHP_UARTB_RBR_RBR_SHIFT                                   0
#define BCHP_UARTB_RBR_RBR_DEFAULT                                 0x00000000

/***************************************************************************
 *THR - Transmit Holding Register
 ***************************************************************************/
/* UARTB :: THR :: reserved0 [31:08] */
#define BCHP_UARTB_THR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_THR_reserved0_SHIFT                             8

/* UARTB :: THR :: THR [07:00] */
#define BCHP_UARTB_THR_THR_MASK                                    0x000000ff
#define BCHP_UARTB_THR_THR_SHIFT                                   0
#define BCHP_UARTB_THR_THR_DEFAULT                                 0x00000000

/***************************************************************************
 *DLH - Divisor Latch High
 ***************************************************************************/
/* UARTB :: DLH :: reserved0 [31:08] */
#define BCHP_UARTB_DLH_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_DLH_reserved0_SHIFT                             8

/* UARTB :: DLH :: DLH [07:00] */
#define BCHP_UARTB_DLH_DLH_MASK                                    0x000000ff
#define BCHP_UARTB_DLH_DLH_SHIFT                                   0
#define BCHP_UARTB_DLH_DLH_DEFAULT                                 0x00000000

/***************************************************************************
 *DLL - Divisor Latch Low
 ***************************************************************************/
/* UARTB :: DLL :: reserved0 [31:08] */
#define BCHP_UARTB_DLL_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_DLL_reserved0_SHIFT                             8

/* UARTB :: DLL :: DLL [07:00] */
#define BCHP_UARTB_DLL_DLL_MASK                                    0x000000ff
#define BCHP_UARTB_DLL_DLL_SHIFT                                   0
#define BCHP_UARTB_DLL_DLL_DEFAULT                                 0x00000000

/***************************************************************************
 *IER - Interrupt Enable Register
 ***************************************************************************/
/* UARTB :: IER :: reserved0 [31:08] */
#define BCHP_UARTB_IER_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_IER_reserved0_SHIFT                             8

/* UARTB :: IER :: PTIME [07:07] */
#define BCHP_UARTB_IER_PTIME_MASK                                  0x00000080
#define BCHP_UARTB_IER_PTIME_SHIFT                                 7
#define BCHP_UARTB_IER_PTIME_DEFAULT                               0x00000000

/* UARTB :: IER :: reserved1 [06:04] */
#define BCHP_UARTB_IER_reserved1_MASK                              0x00000070
#define BCHP_UARTB_IER_reserved1_SHIFT                             4

/* UARTB :: IER :: EDSSI [03:03] */
#define BCHP_UARTB_IER_EDSSI_MASK                                  0x00000008
#define BCHP_UARTB_IER_EDSSI_SHIFT                                 3
#define BCHP_UARTB_IER_EDSSI_DEFAULT                               0x00000000

/* UARTB :: IER :: ELSI [02:02] */
#define BCHP_UARTB_IER_ELSI_MASK                                   0x00000004
#define BCHP_UARTB_IER_ELSI_SHIFT                                  2
#define BCHP_UARTB_IER_ELSI_DEFAULT                                0x00000000

/* UARTB :: IER :: ETBEI [01:01] */
#define BCHP_UARTB_IER_ETBEI_MASK                                  0x00000002
#define BCHP_UARTB_IER_ETBEI_SHIFT                                 1
#define BCHP_UARTB_IER_ETBEI_DEFAULT                               0x00000000

/* UARTB :: IER :: ERBFI [00:00] */
#define BCHP_UARTB_IER_ERBFI_MASK                                  0x00000001
#define BCHP_UARTB_IER_ERBFI_SHIFT                                 0
#define BCHP_UARTB_IER_ERBFI_DEFAULT                               0x00000000

/***************************************************************************
 *IIR - Interrupt Identity Register
 ***************************************************************************/
/* UARTB :: IIR :: reserved0 [31:08] */
#define BCHP_UARTB_IIR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_IIR_reserved0_SHIFT                             8

/* UARTB :: IIR :: FIFOSE [07:06] */
#define BCHP_UARTB_IIR_FIFOSE_MASK                                 0x000000c0
#define BCHP_UARTB_IIR_FIFOSE_SHIFT                                6
#define BCHP_UARTB_IIR_FIFOSE_DEFAULT                              0x00000000

/* UARTB :: IIR :: reserved1 [05:04] */
#define BCHP_UARTB_IIR_reserved1_MASK                              0x00000030
#define BCHP_UARTB_IIR_reserved1_SHIFT                             4

/* UARTB :: IIR :: IID [03:00] */
#define BCHP_UARTB_IIR_IID_MASK                                    0x0000000f
#define BCHP_UARTB_IIR_IID_SHIFT                                   0
#define BCHP_UARTB_IIR_IID_DEFAULT                                 0x00000001

/***************************************************************************
 *FCR - FIFO Control Register
 ***************************************************************************/
/* UARTB :: FCR :: reserved0 [31:08] */
#define BCHP_UARTB_FCR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_FCR_reserved0_SHIFT                             8

/* UARTB :: FCR :: RT [07:06] */
#define BCHP_UARTB_FCR_RT_MASK                                     0x000000c0
#define BCHP_UARTB_FCR_RT_SHIFT                                    6
#define BCHP_UARTB_FCR_RT_DEFAULT                                  0x00000000

/* UARTB :: FCR :: TET [05:04] */
#define BCHP_UARTB_FCR_TET_MASK                                    0x00000030
#define BCHP_UARTB_FCR_TET_SHIFT                                   4
#define BCHP_UARTB_FCR_TET_DEFAULT                                 0x00000000

/* UARTB :: FCR :: DMAM [03:03] */
#define BCHP_UARTB_FCR_DMAM_MASK                                   0x00000008
#define BCHP_UARTB_FCR_DMAM_SHIFT                                  3
#define BCHP_UARTB_FCR_DMAM_DEFAULT                                0x00000000

/* UARTB :: FCR :: XFIFOR [02:02] */
#define BCHP_UARTB_FCR_XFIFOR_MASK                                 0x00000004
#define BCHP_UARTB_FCR_XFIFOR_SHIFT                                2
#define BCHP_UARTB_FCR_XFIFOR_DEFAULT                              0x00000000

/* UARTB :: FCR :: RFIFOR [01:01] */
#define BCHP_UARTB_FCR_RFIFOR_MASK                                 0x00000002
#define BCHP_UARTB_FCR_RFIFOR_SHIFT                                1
#define BCHP_UARTB_FCR_RFIFOR_DEFAULT                              0x00000000

/* UARTB :: FCR :: FIFOE [00:00] */
#define BCHP_UARTB_FCR_FIFOE_MASK                                  0x00000001
#define BCHP_UARTB_FCR_FIFOE_SHIFT                                 0
#define BCHP_UARTB_FCR_FIFOE_DEFAULT                               0x00000000

/***************************************************************************
 *LCR - Line Control Register
 ***************************************************************************/
/* UARTB :: LCR :: reserved0 [31:08] */
#define BCHP_UARTB_LCR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_LCR_reserved0_SHIFT                             8

/* UARTB :: LCR :: DLAB [07:07] */
#define BCHP_UARTB_LCR_DLAB_MASK                                   0x00000080
#define BCHP_UARTB_LCR_DLAB_SHIFT                                  7
#define BCHP_UARTB_LCR_DLAB_DEFAULT                                0x00000000

/* UARTB :: LCR :: BC [06:06] */
#define BCHP_UARTB_LCR_BC_MASK                                     0x00000040
#define BCHP_UARTB_LCR_BC_SHIFT                                    6
#define BCHP_UARTB_LCR_BC_DEFAULT                                  0x00000000

/* UARTB :: LCR :: reserved1 [05:05] */
#define BCHP_UARTB_LCR_reserved1_MASK                              0x00000020
#define BCHP_UARTB_LCR_reserved1_SHIFT                             5

/* UARTB :: LCR :: EPS [04:04] */
#define BCHP_UARTB_LCR_EPS_MASK                                    0x00000010
#define BCHP_UARTB_LCR_EPS_SHIFT                                   4
#define BCHP_UARTB_LCR_EPS_DEFAULT                                 0x00000000

/* UARTB :: LCR :: PEN [03:03] */
#define BCHP_UARTB_LCR_PEN_MASK                                    0x00000008
#define BCHP_UARTB_LCR_PEN_SHIFT                                   3
#define BCHP_UARTB_LCR_PEN_DEFAULT                                 0x00000000

/* UARTB :: LCR :: STOP [02:02] */
#define BCHP_UARTB_LCR_STOP_MASK                                   0x00000004
#define BCHP_UARTB_LCR_STOP_SHIFT                                  2
#define BCHP_UARTB_LCR_STOP_DEFAULT                                0x00000000

/* UARTB :: LCR :: DLS [01:00] */
#define BCHP_UARTB_LCR_DLS_MASK                                    0x00000003
#define BCHP_UARTB_LCR_DLS_SHIFT                                   0
#define BCHP_UARTB_LCR_DLS_DEFAULT                                 0x00000000

/***************************************************************************
 *MCR - Modem Control Register
 ***************************************************************************/
/* UARTB :: MCR :: reserved0 [31:07] */
#define BCHP_UARTB_MCR_reserved0_MASK                              0xffffff80
#define BCHP_UARTB_MCR_reserved0_SHIFT                             7

/* UARTB :: MCR :: SIRE [06:06] */
#define BCHP_UARTB_MCR_SIRE_MASK                                   0x00000040
#define BCHP_UARTB_MCR_SIRE_SHIFT                                  6
#define BCHP_UARTB_MCR_SIRE_DEFAULT                                0x00000000

/* UARTB :: MCR :: AFCE [05:05] */
#define BCHP_UARTB_MCR_AFCE_MASK                                   0x00000020
#define BCHP_UARTB_MCR_AFCE_SHIFT                                  5
#define BCHP_UARTB_MCR_AFCE_DEFAULT                                0x00000000

/* UARTB :: MCR :: LB [04:04] */
#define BCHP_UARTB_MCR_LB_MASK                                     0x00000010
#define BCHP_UARTB_MCR_LB_SHIFT                                    4
#define BCHP_UARTB_MCR_LB_DEFAULT                                  0x00000000

/* UARTB :: MCR :: OUT2 [03:03] */
#define BCHP_UARTB_MCR_OUT2_MASK                                   0x00000008
#define BCHP_UARTB_MCR_OUT2_SHIFT                                  3
#define BCHP_UARTB_MCR_OUT2_DEFAULT                                0x00000000

/* UARTB :: MCR :: OUT1 [02:02] */
#define BCHP_UARTB_MCR_OUT1_MASK                                   0x00000004
#define BCHP_UARTB_MCR_OUT1_SHIFT                                  2
#define BCHP_UARTB_MCR_OUT1_DEFAULT                                0x00000000

/* UARTB :: MCR :: RTS [01:01] */
#define BCHP_UARTB_MCR_RTS_MASK                                    0x00000002
#define BCHP_UARTB_MCR_RTS_SHIFT                                   1
#define BCHP_UARTB_MCR_RTS_DEFAULT                                 0x00000000

/* UARTB :: MCR :: DTR [00:00] */
#define BCHP_UARTB_MCR_DTR_MASK                                    0x00000001
#define BCHP_UARTB_MCR_DTR_SHIFT                                   0
#define BCHP_UARTB_MCR_DTR_DEFAULT                                 0x00000000

/***************************************************************************
 *LSR - Line Status Register
 ***************************************************************************/
/* UARTB :: LSR :: reserved0 [31:08] */
#define BCHP_UARTB_LSR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_LSR_reserved0_SHIFT                             8

/* UARTB :: LSR :: RFE [07:07] */
#define BCHP_UARTB_LSR_RFE_MASK                                    0x00000080
#define BCHP_UARTB_LSR_RFE_SHIFT                                   7
#define BCHP_UARTB_LSR_RFE_DEFAULT                                 0x00000000

/* UARTB :: LSR :: TEMT [06:06] */
#define BCHP_UARTB_LSR_TEMT_MASK                                   0x00000040
#define BCHP_UARTB_LSR_TEMT_SHIFT                                  6
#define BCHP_UARTB_LSR_TEMT_DEFAULT                                0x00000001

/* UARTB :: LSR :: THRE [05:05] */
#define BCHP_UARTB_LSR_THRE_MASK                                   0x00000020
#define BCHP_UARTB_LSR_THRE_SHIFT                                  5
#define BCHP_UARTB_LSR_THRE_DEFAULT                                0x00000001

/* UARTB :: LSR :: BI [04:04] */
#define BCHP_UARTB_LSR_BI_MASK                                     0x00000010
#define BCHP_UARTB_LSR_BI_SHIFT                                    4
#define BCHP_UARTB_LSR_BI_DEFAULT                                  0x00000000

/* UARTB :: LSR :: FE [03:03] */
#define BCHP_UARTB_LSR_FE_MASK                                     0x00000008
#define BCHP_UARTB_LSR_FE_SHIFT                                    3
#define BCHP_UARTB_LSR_FE_DEFAULT                                  0x00000000

/* UARTB :: LSR :: PE [02:02] */
#define BCHP_UARTB_LSR_PE_MASK                                     0x00000004
#define BCHP_UARTB_LSR_PE_SHIFT                                    2
#define BCHP_UARTB_LSR_PE_DEFAULT                                  0x00000000

/* UARTB :: LSR :: OE [01:01] */
#define BCHP_UARTB_LSR_OE_MASK                                     0x00000002
#define BCHP_UARTB_LSR_OE_SHIFT                                    1
#define BCHP_UARTB_LSR_OE_DEFAULT                                  0x00000000

/* UARTB :: LSR :: DR [00:00] */
#define BCHP_UARTB_LSR_DR_MASK                                     0x00000001
#define BCHP_UARTB_LSR_DR_SHIFT                                    0
#define BCHP_UARTB_LSR_DR_DEFAULT                                  0x00000000

/***************************************************************************
 *MSR - Modem Status Register
 ***************************************************************************/
/* UARTB :: MSR :: reserved0 [31:08] */
#define BCHP_UARTB_MSR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_MSR_reserved0_SHIFT                             8

/* UARTB :: MSR :: DCD [07:07] */
#define BCHP_UARTB_MSR_DCD_MASK                                    0x00000080
#define BCHP_UARTB_MSR_DCD_SHIFT                                   7
#define BCHP_UARTB_MSR_DCD_DEFAULT                                 0x00000000

/* UARTB :: MSR :: RI [06:06] */
#define BCHP_UARTB_MSR_RI_MASK                                     0x00000040
#define BCHP_UARTB_MSR_RI_SHIFT                                    6
#define BCHP_UARTB_MSR_RI_DEFAULT                                  0x00000000

/* UARTB :: MSR :: DSR [05:05] */
#define BCHP_UARTB_MSR_DSR_MASK                                    0x00000020
#define BCHP_UARTB_MSR_DSR_SHIFT                                   5
#define BCHP_UARTB_MSR_DSR_DEFAULT                                 0x00000000

/* UARTB :: MSR :: CTS [04:04] */
#define BCHP_UARTB_MSR_CTS_MASK                                    0x00000010
#define BCHP_UARTB_MSR_CTS_SHIFT                                   4
#define BCHP_UARTB_MSR_CTS_DEFAULT                                 0x00000000

/* UARTB :: MSR :: DDCD [03:03] */
#define BCHP_UARTB_MSR_DDCD_MASK                                   0x00000008
#define BCHP_UARTB_MSR_DDCD_SHIFT                                  3
#define BCHP_UARTB_MSR_DDCD_DEFAULT                                0x00000000

/* UARTB :: MSR :: TERI [02:02] */
#define BCHP_UARTB_MSR_TERI_MASK                                   0x00000004
#define BCHP_UARTB_MSR_TERI_SHIFT                                  2
#define BCHP_UARTB_MSR_TERI_DEFAULT                                0x00000000

/* UARTB :: MSR :: DDSR [01:01] */
#define BCHP_UARTB_MSR_DDSR_MASK                                   0x00000002
#define BCHP_UARTB_MSR_DDSR_SHIFT                                  1
#define BCHP_UARTB_MSR_DDSR_DEFAULT                                0x00000000

/* UARTB :: MSR :: DCTS [00:00] */
#define BCHP_UARTB_MSR_DCTS_MASK                                   0x00000001
#define BCHP_UARTB_MSR_DCTS_SHIFT                                  0
#define BCHP_UARTB_MSR_DCTS_DEFAULT                                0x00000000

/***************************************************************************
 *SCR - Scratchpad Register
 ***************************************************************************/
/* UARTB :: SCR :: reserved0 [31:08] */
#define BCHP_UARTB_SCR_reserved0_MASK                              0xffffff00
#define BCHP_UARTB_SCR_reserved0_SHIFT                             8

/* UARTB :: SCR :: SCR [07:00] */
#define BCHP_UARTB_SCR_SCR_MASK                                    0x000000ff
#define BCHP_UARTB_SCR_SCR_SHIFT                                   0
#define BCHP_UARTB_SCR_SCR_DEFAULT                                 0x00000000

#endif /* #ifndef BCHP_UARTB_H__ */

/* End of File */

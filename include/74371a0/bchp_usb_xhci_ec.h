/****************************************************************************
 *     Copyright (c) 1999-2014, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * Module Description:
 *                     DO NOT EDIT THIS FILE DIRECTLY
 *
 * This module was generated magically with RDB from a source description
 * file. You must edit the source file for changes to be made to this file.
 *
 *
 * Date:           Generated on               Thu Feb 19 03:16:48 2015
 *                 Full Compile MD5 Checksum  a5e4ec59635428e29d8011e790dc32b8
 *                     (minus title and desc)
 *                 MD5 Checksum               db361cb744c5e0e017160da1bb1a3291
 *
 * Compiled with:  RDB Utility                combo_header.pl
 *                 RDB.pm                     15517
 *                 unknown                    unknown
 *                 Perl Interpreter           5.008008
 *                 Operating System           linux
 *
 *
 ***************************************************************************/

#ifndef BCHP_USB_XHCI_EC_H__
#define BCHP_USB_XHCI_EC_H__

/***************************************************************************
 *USB_XHCI_EC - USB XHCI Extended Capability Registers
 ***************************************************************************/
#define BCHP_USB_XHCI_EC_ECHSPT                  0x00481940 /* [RW] Extended Capability Supported Protocols (SPT) Header */
#define BCHP_USB_XHCI_EC_PNSTR                   0x00481944 /* [RW] Extended Capability SPT Protocol name string register */
#define BCHP_USB_XHCI_EC_PSUM                    0x00481948 /* [RW] Extended Capability SPT Protocol summary register */
#define BCHP_USB_XHCI_EC_ECHPMG                  0x00481960 /* [RW] Extended Capability Power Management (PMG) Header */
#define BCHP_USB_XHCI_EC_PMGCSD                  0x00481964 /* [RW] Extended Capability PMG control, status and data register */
#define BCHP_USB_XHCI_EC_ECHIOV                  0x00481968 /* [RW] Extended Capability IO Virtualization (IOV) */
#define BCHP_USB_XHCI_EC_ECHMSI                  0x00481ae8 /* [RW] Extended Capability Message Signaled Interrupt (MSI) */
#define BCHP_USB_XHCI_EC_ECHDBC                  0x00481af8 /* [RW] Extended Capability Debug (DBC) */
#define BCHP_USB_XHCI_EC_ECHRSVD                 0x00481b38 /* [RW] Extended Capability  Reserved */
#define BCHP_USB_XHCI_EC_ECHCTT                  0x00481bf0 /* [RW] Extended Capability Controller Testing */
#define BCHP_USB_XHCI_EC_ECHBIU                  0x00481c00 /* [RW] Extended Capability  BIU Header */
#define BCHP_USB_XHCI_EC_BIUSPC                  0x00481c04 /* [RW] Extended Capability  BIU Bus Specification */
#define BCHP_USB_XHCI_EC_BIUSPC1                 0x00481c08 /* [RW] Extended Capability  BIU AXIWRA */
#define BCHP_USB_XHCI_EC_BIUSPC2                 0x00481c0c /* [RW] Extended Capability  BIU AXIRDA */
#define BCHP_USB_XHCI_EC_BIULPM                  0x00481c10 /* [RW] Extended Capability  BIU AXILPM */
#define BCHP_USB_XHCI_EC_ECHCSR                  0x00481c20 /* [RW] Extended capability CSR Header */
#define BCHP_USB_XHCI_EC_ECHAIU                  0x00481c30 /* [RW] Extended Capability  AIU Header */
#define BCHP_USB_XHCI_EC_AIUDMA                  0x00481c34 /* [RW] Extended Capability AIU DMA priority specification */
#define BCHP_USB_XHCI_EC_AIUSTM                  0x00481c38 /* [RW] Extended Capability AIU Bulk stream configuration */
#define BCHP_USB_XHCI_EC_AIUCFG                  0x00481c3c /* [RW] Extended Capability AIU Resource Configuration and Policy Spec */
#define BCHP_USB_XHCI_EC_ECHFSC                  0x00481c40 /* [RW] Extended Capability Fifo and Sram Controller (FSC) Header */
#define BCHP_USB_XHCI_EC_FSCPOC                  0x00481c54 /* [RW] Extended Capability FSC Periodic OUT Channel Configuration */
#define BCHP_USB_XHCI_EC_FSCGOC                  0x00481c58 /* [RW] Extended Capability FSC Generic OUT Channel Configuration */
#define BCHP_USB_XHCI_EC_FSCNOC                  0x00481c5c /* [RW] Extended Capability FSC Non-Periodic OUT Channel Configuration */
#define BCHP_USB_XHCI_EC_FSCAIC                  0x00481c60 /* [RW] Extended Capability FSC All IN Channel Configuration */
#define BCHP_USB_XHCI_EC_FSCGIC                  0x00481c68 /* [RW] Extended Capability FSC Generic IN Channel Configuration */
#define BCHP_USB_XHCI_EC_FSCNIC                  0x00481c6c /* [RW] Extended Capability FSC Non-Periodic IN Channel Configuration */
#define BCHP_USB_XHCI_EC_ECHPRT                  0x00481c70 /* [RW] Extended capability SS Protocol Layer (PRT) Header */
#define BCHP_USB_XHCI_EC_PRTHSC                  0x00481c78 /* [RW] Extended capability PRT histogram timer and control */
#define BCHP_USB_XHCI_EC_PRTHSR                  0x00481c7c /* [RW] Extended capability PRT histogram reports */
#define BCHP_USB_XHCI_EC_ECHSRH                  0x00481c80 /* [RW] Extended capability SS Root Hub (SRH) Header */
#define BCHP_USB_XHCI_EC_SRHDES                  0x00481c84 /* [RW] Extended capability SRH SS Root Hub descriptor */
#define BCHP_USB_XHCI_EC_ECHPHY                  0x00481cb0 /* [RW] Extended capability SS Physical Layer (PHY) Header */
#define BCHP_USB_XHCI_EC_ECHRSVD2                0x00481cc0 /* [RW] Extended capability Reserved 2 */
#define BCHP_USB_XHCI_EC_ECHHRH                  0x00481f50 /* [RW] Extended Capability HS Root Hub (HRH) Header */
#define BCHP_USB_XHCI_EC_ECHU2P                  0x00481f80 /* [RW] Extended Capability USB20 Physical Layer Hub (HRH) Header */
#define BCHP_USB_XHCI_EC_ECHIRA                  0x00481f90 /* [RW] Extended Capability Internal Register Access (IRA) Header */
#define BCHP_USB_XHCI_EC_IRAADR                  0x00481f98 /* [RW] Extended Capability IRA Address Register */
#define BCHP_USB_XHCI_EC_IRADAT                  0x00481f9c /* [RW] Extended Capability IRA Data Register */
#define BCHP_USB_XHCI_EC_ECHHST                  0x00481fa0 /* [RW] Extended Capability Host Operation (HST) Header */
#define BCHP_USB_XHCI_EC_ECHRBV                  0x00481fb0 /* [RW] Extended Capability Release and Build Version (RBV) Header */
#define BCHP_USB_XHCI_EC_ECHSPARE                0x00481ffc /* [RW] Extended Capability SPARE */

/***************************************************************************
 *ECHSPT - Extended Capability Supported Protocols (SPT) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHSPT :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHSPT_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHSPT_NA_SHIFT                           0

/***************************************************************************
 *PNSTR - Extended Capability SPT Protocol name string register
 ***************************************************************************/
/* USB_XHCI_EC :: PNSTR :: NA [31:00] */
#define BCHP_USB_XHCI_EC_PNSTR_NA_MASK                             0xffffffff
#define BCHP_USB_XHCI_EC_PNSTR_NA_SHIFT                            0

/***************************************************************************
 *PSUM - Extended Capability SPT Protocol summary register
 ***************************************************************************/
/* USB_XHCI_EC :: PSUM :: NA [31:00] */
#define BCHP_USB_XHCI_EC_PSUM_NA_MASK                              0xffffffff
#define BCHP_USB_XHCI_EC_PSUM_NA_SHIFT                             0

/***************************************************************************
 *ECHPMG - Extended Capability Power Management (PMG) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHPMG :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHPMG_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHPMG_NA_SHIFT                           0

/***************************************************************************
 *PMGCSD - Extended Capability PMG control, status and data register
 ***************************************************************************/
/* USB_XHCI_EC :: PMGCSD :: NA [31:00] */
#define BCHP_USB_XHCI_EC_PMGCSD_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_PMGCSD_NA_SHIFT                           0

/***************************************************************************
 *ECHIOV - Extended Capability IO Virtualization (IOV)
 ***************************************************************************/
/* USB_XHCI_EC :: ECHIOV :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHIOV_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHIOV_NA_SHIFT                           0

/***************************************************************************
 *ECHMSI - Extended Capability Message Signaled Interrupt (MSI)
 ***************************************************************************/
/* USB_XHCI_EC :: ECHMSI :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHMSI_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHMSI_NA_SHIFT                           0

/***************************************************************************
 *ECHDBC - Extended Capability Debug (DBC)
 ***************************************************************************/
/* USB_XHCI_EC :: ECHDBC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHDBC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHDBC_NA_SHIFT                           0

/***************************************************************************
 *ECHRSVD - Extended Capability  Reserved
 ***************************************************************************/
/* USB_XHCI_EC :: ECHRSVD :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHRSVD_NA_MASK                           0xffffffff
#define BCHP_USB_XHCI_EC_ECHRSVD_NA_SHIFT                          0

/***************************************************************************
 *ECHCTT - Extended Capability Controller Testing
 ***************************************************************************/
/* USB_XHCI_EC :: ECHCTT :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHCTT_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHCTT_NA_SHIFT                           0

/***************************************************************************
 *ECHBIU - Extended Capability  BIU Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHBIU :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHBIU_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHBIU_NA_SHIFT                           0

/***************************************************************************
 *BIUSPC - Extended Capability  BIU Bus Specification
 ***************************************************************************/
/* USB_XHCI_EC :: BIUSPC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_BIUSPC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_BIUSPC_NA_SHIFT                           0

/***************************************************************************
 *BIUSPC1 - Extended Capability  BIU AXIWRA
 ***************************************************************************/
/* USB_XHCI_EC :: BIUSPC1 :: NA [31:00] */
#define BCHP_USB_XHCI_EC_BIUSPC1_NA_MASK                           0xffffffff
#define BCHP_USB_XHCI_EC_BIUSPC1_NA_SHIFT                          0

/***************************************************************************
 *BIUSPC2 - Extended Capability  BIU AXIRDA
 ***************************************************************************/
/* USB_XHCI_EC :: BIUSPC2 :: NA [31:00] */
#define BCHP_USB_XHCI_EC_BIUSPC2_NA_MASK                           0xffffffff
#define BCHP_USB_XHCI_EC_BIUSPC2_NA_SHIFT                          0

/***************************************************************************
 *BIULPM - Extended Capability  BIU AXILPM
 ***************************************************************************/
/* USB_XHCI_EC :: BIULPM :: NA [31:00] */
#define BCHP_USB_XHCI_EC_BIULPM_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_BIULPM_NA_SHIFT                           0

/***************************************************************************
 *ECHCSR - Extended capability CSR Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHCSR :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHCSR_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHCSR_NA_SHIFT                           0

/***************************************************************************
 *ECHAIU - Extended Capability  AIU Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHAIU :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHAIU_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHAIU_NA_SHIFT                           0

/***************************************************************************
 *AIUDMA - Extended Capability AIU DMA priority specification
 ***************************************************************************/
/* USB_XHCI_EC :: AIUDMA :: NA [31:00] */
#define BCHP_USB_XHCI_EC_AIUDMA_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_AIUDMA_NA_SHIFT                           0

/***************************************************************************
 *AIUSTM - Extended Capability AIU Bulk stream configuration
 ***************************************************************************/
/* USB_XHCI_EC :: AIUSTM :: NA [31:00] */
#define BCHP_USB_XHCI_EC_AIUSTM_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_AIUSTM_NA_SHIFT                           0

/***************************************************************************
 *AIUCFG - Extended Capability AIU Resource Configuration and Policy Spec
 ***************************************************************************/
/* USB_XHCI_EC :: AIUCFG :: NA [31:00] */
#define BCHP_USB_XHCI_EC_AIUCFG_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_AIUCFG_NA_SHIFT                           0

/***************************************************************************
 *ECHFSC - Extended Capability Fifo and Sram Controller (FSC) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHFSC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHFSC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHFSC_NA_SHIFT                           0

/***************************************************************************
 *FSCPOC - Extended Capability FSC Periodic OUT Channel Configuration
 ***************************************************************************/
/* USB_XHCI_EC :: FSCPOC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_FSCPOC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_FSCPOC_NA_SHIFT                           0

/***************************************************************************
 *FSCGOC - Extended Capability FSC Generic OUT Channel Configuration
 ***************************************************************************/
/* USB_XHCI_EC :: FSCGOC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_FSCGOC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_FSCGOC_NA_SHIFT                           0

/***************************************************************************
 *FSCNOC - Extended Capability FSC Non-Periodic OUT Channel Configuration
 ***************************************************************************/
/* USB_XHCI_EC :: FSCNOC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_FSCNOC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_FSCNOC_NA_SHIFT                           0

/***************************************************************************
 *FSCAIC - Extended Capability FSC All IN Channel Configuration
 ***************************************************************************/
/* USB_XHCI_EC :: FSCAIC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_FSCAIC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_FSCAIC_NA_SHIFT                           0

/***************************************************************************
 *FSCGIC - Extended Capability FSC Generic IN Channel Configuration
 ***************************************************************************/
/* USB_XHCI_EC :: FSCGIC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_FSCGIC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_FSCGIC_NA_SHIFT                           0

/***************************************************************************
 *FSCNIC - Extended Capability FSC Non-Periodic IN Channel Configuration
 ***************************************************************************/
/* USB_XHCI_EC :: FSCNIC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_FSCNIC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_FSCNIC_NA_SHIFT                           0

/***************************************************************************
 *ECHPRT - Extended capability SS Protocol Layer (PRT) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHPRT :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHPRT_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHPRT_NA_SHIFT                           0

/***************************************************************************
 *PRTHSC - Extended capability PRT histogram timer and control
 ***************************************************************************/
/* USB_XHCI_EC :: PRTHSC :: NA [31:00] */
#define BCHP_USB_XHCI_EC_PRTHSC_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_PRTHSC_NA_SHIFT                           0

/***************************************************************************
 *PRTHSR - Extended capability PRT histogram reports
 ***************************************************************************/
/* USB_XHCI_EC :: PRTHSR :: NA [31:00] */
#define BCHP_USB_XHCI_EC_PRTHSR_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_PRTHSR_NA_SHIFT                           0

/***************************************************************************
 *ECHSRH - Extended capability SS Root Hub (SRH) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHSRH :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHSRH_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHSRH_NA_SHIFT                           0

/***************************************************************************
 *SRHDES - Extended capability SRH SS Root Hub descriptor
 ***************************************************************************/
/* USB_XHCI_EC :: SRHDES :: NA [31:00] */
#define BCHP_USB_XHCI_EC_SRHDES_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_SRHDES_NA_SHIFT                           0

/***************************************************************************
 *ECHPHY - Extended capability SS Physical Layer (PHY) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHPHY :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHPHY_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHPHY_NA_SHIFT                           0

/***************************************************************************
 *ECHRSVD2 - Extended capability Reserved 2
 ***************************************************************************/
/* USB_XHCI_EC :: ECHRSVD2 :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHRSVD2_NA_MASK                          0xffffffff
#define BCHP_USB_XHCI_EC_ECHRSVD2_NA_SHIFT                         0

/***************************************************************************
 *ECHHRH - Extended Capability HS Root Hub (HRH) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHHRH :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHHRH_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHHRH_NA_SHIFT                           0

/***************************************************************************
 *ECHU2P - Extended Capability USB20 Physical Layer Hub (HRH) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHU2P :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHU2P_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHU2P_NA_SHIFT                           0

/***************************************************************************
 *ECHIRA - Extended Capability Internal Register Access (IRA) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHIRA :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHIRA_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHIRA_NA_SHIFT                           0

/***************************************************************************
 *IRAADR - Extended Capability IRA Address Register
 ***************************************************************************/
/* USB_XHCI_EC :: IRAADR :: NA [31:00] */
#define BCHP_USB_XHCI_EC_IRAADR_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_IRAADR_NA_SHIFT                           0

/***************************************************************************
 *IRADAT - Extended Capability IRA Data Register
 ***************************************************************************/
/* USB_XHCI_EC :: IRADAT :: NA [31:00] */
#define BCHP_USB_XHCI_EC_IRADAT_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_IRADAT_NA_SHIFT                           0

/***************************************************************************
 *ECHHST - Extended Capability Host Operation (HST) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHHST :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHHST_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHHST_NA_SHIFT                           0

/***************************************************************************
 *ECHRBV - Extended Capability Release and Build Version (RBV) Header
 ***************************************************************************/
/* USB_XHCI_EC :: ECHRBV :: NA [31:00] */
#define BCHP_USB_XHCI_EC_ECHRBV_NA_MASK                            0xffffffff
#define BCHP_USB_XHCI_EC_ECHRBV_NA_SHIFT                           0

/***************************************************************************
 *ECHSPARE - Extended Capability SPARE
 ***************************************************************************/
/* USB_XHCI_EC :: ECHSPARE :: SPARE [31:00] */
#define BCHP_USB_XHCI_EC_ECHSPARE_SPARE_MASK                       0xffffffff
#define BCHP_USB_XHCI_EC_ECHSPARE_SPARE_SHIFT                      0
#define BCHP_USB_XHCI_EC_ECHSPARE_SPARE_DEFAULT                    0x00000000

#endif /* #ifndef BCHP_USB_XHCI_EC_H__ */

/* End of File */

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
 * Date:           Generated on              Mon Oct 20 03:10:22 2014
 *                 Full Compile MD5 Checksum eb28ce4f1e21a5a2f696ae3965f6bf92
 *                   (minus title and desc)  
 *                 MD5 Checksum              fb1aa74dc14cfad64e08221bb4891f7d
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

#ifndef BCHP_GIO_H__
#define BCHP_GIO_H__

/***************************************************************************
 *GIO - GPIO
 ***************************************************************************/
#define BCHP_GIO_ODEN_LO                         0x2040a200 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[31:0] */
#define BCHP_GIO_DATA_LO                         0x2040a204 /* GENERAL PURPOSE I/O DATA FOR  GPIO[31:0] */
#define BCHP_GIO_IODIR_LO                        0x2040a208 /* GENERAL PURPOSE I/O DIRECTION FOR  GPIO[31:0] */
#define BCHP_GIO_EC_LO                           0x2040a20c /* GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[31:0] */
#define BCHP_GIO_EI_LO                           0x2040a210 /* GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[31:0] */
#define BCHP_GIO_MASK_LO                         0x2040a214 /* GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[31:0] */
#define BCHP_GIO_LEVEL_LO                        0x2040a218 /* GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[31:0] */
#define BCHP_GIO_STAT_LO                         0x2040a21c /* GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[31:0] */
#define BCHP_GIO_ODEN_HI                         0x2040a220 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[63:32] */
#define BCHP_GIO_DATA_HI                         0x2040a224 /* GENERAL PURPOSE I/O DATA FOR  GPIO[63:32] */
#define BCHP_GIO_IODIR_HI                        0x2040a228 /* GENERAL PURPOSE I/O DIRECTION FOR  GPIO[63:32] */
#define BCHP_GIO_EC_HI                           0x2040a22c /* GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[63:32] */
#define BCHP_GIO_EI_HI                           0x2040a230 /* GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[63:32] */
#define BCHP_GIO_MASK_HI                         0x2040a234 /* GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[63:32] */
#define BCHP_GIO_LEVEL_HI                        0x2040a238 /* GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[63:32] */
#define BCHP_GIO_STAT_HI                         0x2040a23c /* GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[63:32] */
#define BCHP_GIO_ODEN_EXT_HI                     0x2040a240 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[95:64] */
#define BCHP_GIO_DATA_EXT_HI                     0x2040a244 /* GENERAL PURPOSE I/O DATA FOR  GPIO[95:64] */
#define BCHP_GIO_IODIR_EXT_HI                    0x2040a248 /* GENERAL PURPOSE I/O DIRECTION FOR  GPIO[95:64] */
#define BCHP_GIO_EC_EXT_HI                       0x2040a24c /* GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[95:64] */
#define BCHP_GIO_EI_EXT_HI                       0x2040a250 /* GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[95:64] */
#define BCHP_GIO_MASK_EXT_HI                     0x2040a254 /* GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[95:64] */
#define BCHP_GIO_LEVEL_EXT_HI                    0x2040a258 /* GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[95:64] */
#define BCHP_GIO_STAT_EXT_HI                     0x2040a25c /* GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[95:64] */
#define BCHP_GIO_ODEN_EXT2                       0x2040a260 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[102:96] */
#define BCHP_GIO_DATA_EXT2                       0x2040a264 /* GENERAL PURPOSE I/O DATA FOR  GPIO[102:96] */
#define BCHP_GIO_IODIR_EXT2                      0x2040a268 /* GENERAL PURPOSE I/O DIRECTION FOR  GPIO[102:96] */
#define BCHP_GIO_EC_EXT2                         0x2040a26c /* GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[102:96] */
#define BCHP_GIO_EI_EXT2                         0x2040a270 /* GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[102:96] */
#define BCHP_GIO_MASK_EXT2                       0x2040a274 /* GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[102:96] */
#define BCHP_GIO_LEVEL_EXT2                      0x2040a278 /* GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[102:96] */
#define BCHP_GIO_STAT_EXT2                       0x2040a27c /* GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[102:96] */
#define BCHP_GIO_ODEN_EXT                        0x2040a280 /* GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR SGPIO[3:0] and AON_SGPIO[1:0] */
#define BCHP_GIO_DATA_EXT                        0x2040a284 /* GENERAL PURPOSE I/O DATA FOR SGPIO[3:0] and AON_SGPIO[1:0] */
#define BCHP_GIO_IODIR_EXT                       0x2040a288 /* GENERAL PURPOSE I/O DIRECTION FOR SGPIO[3:0] and AON_SGPIO[1:0] */
#define BCHP_GIO_EC_EXT                          0x2040a28c /* GENERAL PURPOSE I/O EDGE CONFIGURATION FOR SGPIO[3:0] and AON_SGPIO[1:0] */
#define BCHP_GIO_EI_EXT                          0x2040a290 /* GENERAL PURPOSE I/O EDGE INSENSITIVE FOR SGPIO[3:0] and AON_SGPIO[1:0] */
#define BCHP_GIO_MASK_EXT                        0x2040a294 /* GENERAL PURPOSE I/O INTERRUPT MASK FOR SGPIO[3:0] and AON_SGPIO[1:0] */
#define BCHP_GIO_LEVEL_EXT                       0x2040a298 /* GENERAL PURPOSE I/O INTERRUPT TYPE FOR SGPIO[3:0] and AON_SGPIO[1:0] */
#define BCHP_GIO_STAT_EXT                        0x2040a29c /* GENERAL PURPOSE I/O INTERRUPT STATUS FOR SGPIO[3:0] and AON_SGPIO[1:0] */

/***************************************************************************
 *ODEN_LO - GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: ODEN_LO :: oden [31:00] */
#define BCHP_GIO_ODEN_LO_oden_MASK                                 0xffffffff
#define BCHP_GIO_ODEN_LO_oden_SHIFT                                0
#define BCHP_GIO_ODEN_LO_oden_DEFAULT                              0x00000000

/***************************************************************************
 *DATA_LO - GENERAL PURPOSE I/O DATA FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: DATA_LO :: data [31:00] */
#define BCHP_GIO_DATA_LO_data_MASK                                 0xffffffff
#define BCHP_GIO_DATA_LO_data_SHIFT                                0
#define BCHP_GIO_DATA_LO_data_DEFAULT                              0x00000000

/***************************************************************************
 *IODIR_LO - GENERAL PURPOSE I/O DIRECTION FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: IODIR_LO :: iodir [31:00] */
#define BCHP_GIO_IODIR_LO_iodir_MASK                               0xffffffff
#define BCHP_GIO_IODIR_LO_iodir_SHIFT                              0
#define BCHP_GIO_IODIR_LO_iodir_DEFAULT                            0xffffffff

/***************************************************************************
 *EC_LO - GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: EC_LO :: edge_config [31:00] */
#define BCHP_GIO_EC_LO_edge_config_MASK                            0xffffffff
#define BCHP_GIO_EC_LO_edge_config_SHIFT                           0
#define BCHP_GIO_EC_LO_edge_config_DEFAULT                         0x00000000

/***************************************************************************
 *EI_LO - GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: EI_LO :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_LO_edge_insensitive_MASK                       0xffffffff
#define BCHP_GIO_EI_LO_edge_insensitive_SHIFT                      0
#define BCHP_GIO_EI_LO_edge_insensitive_DEFAULT                    0x00000000

/***************************************************************************
 *MASK_LO - GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: MASK_LO :: irq_mask [31:00] */
#define BCHP_GIO_MASK_LO_irq_mask_MASK                             0xffffffff
#define BCHP_GIO_MASK_LO_irq_mask_SHIFT                            0
#define BCHP_GIO_MASK_LO_irq_mask_DEFAULT                          0x00000000

/***************************************************************************
 *LEVEL_LO - GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: LEVEL_LO :: level [31:00] */
#define BCHP_GIO_LEVEL_LO_level_MASK                               0xffffffff
#define BCHP_GIO_LEVEL_LO_level_SHIFT                              0
#define BCHP_GIO_LEVEL_LO_level_DEFAULT                            0x00000000

/***************************************************************************
 *STAT_LO - GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[31:0]
 ***************************************************************************/
/* GIO :: STAT_LO :: irq_status [31:00] */
#define BCHP_GIO_STAT_LO_irq_status_MASK                           0xffffffff
#define BCHP_GIO_STAT_LO_irq_status_SHIFT                          0
#define BCHP_GIO_STAT_LO_irq_status_DEFAULT                        0x00000000

/***************************************************************************
 *ODEN_HI - GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: ODEN_HI :: oden [31:00] */
#define BCHP_GIO_ODEN_HI_oden_MASK                                 0xffffffff
#define BCHP_GIO_ODEN_HI_oden_SHIFT                                0
#define BCHP_GIO_ODEN_HI_oden_DEFAULT                              0x00000000

/***************************************************************************
 *DATA_HI - GENERAL PURPOSE I/O DATA FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: DATA_HI :: data [31:00] */
#define BCHP_GIO_DATA_HI_data_MASK                                 0xffffffff
#define BCHP_GIO_DATA_HI_data_SHIFT                                0
#define BCHP_GIO_DATA_HI_data_DEFAULT                              0x00000000

/***************************************************************************
 *IODIR_HI - GENERAL PURPOSE I/O DIRECTION FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: IODIR_HI :: iodir [31:00] */
#define BCHP_GIO_IODIR_HI_iodir_MASK                               0xffffffff
#define BCHP_GIO_IODIR_HI_iodir_SHIFT                              0
#define BCHP_GIO_IODIR_HI_iodir_DEFAULT                            0xffffffff

/***************************************************************************
 *EC_HI - GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: EC_HI :: edge_config [31:00] */
#define BCHP_GIO_EC_HI_edge_config_MASK                            0xffffffff
#define BCHP_GIO_EC_HI_edge_config_SHIFT                           0
#define BCHP_GIO_EC_HI_edge_config_DEFAULT                         0x00000000

/***************************************************************************
 *EI_HI - GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: EI_HI :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_HI_edge_insensitive_MASK                       0xffffffff
#define BCHP_GIO_EI_HI_edge_insensitive_SHIFT                      0
#define BCHP_GIO_EI_HI_edge_insensitive_DEFAULT                    0x00000000

/***************************************************************************
 *MASK_HI - GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: MASK_HI :: irq_mask [31:00] */
#define BCHP_GIO_MASK_HI_irq_mask_MASK                             0xffffffff
#define BCHP_GIO_MASK_HI_irq_mask_SHIFT                            0
#define BCHP_GIO_MASK_HI_irq_mask_DEFAULT                          0x00000000

/***************************************************************************
 *LEVEL_HI - GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: LEVEL_HI :: level [31:00] */
#define BCHP_GIO_LEVEL_HI_level_MASK                               0xffffffff
#define BCHP_GIO_LEVEL_HI_level_SHIFT                              0
#define BCHP_GIO_LEVEL_HI_level_DEFAULT                            0x00000000

/***************************************************************************
 *STAT_HI - GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[63:32]
 ***************************************************************************/
/* GIO :: STAT_HI :: irq_status [31:00] */
#define BCHP_GIO_STAT_HI_irq_status_MASK                           0xffffffff
#define BCHP_GIO_STAT_HI_irq_status_SHIFT                          0
#define BCHP_GIO_STAT_HI_irq_status_DEFAULT                        0x00000000

/***************************************************************************
 *ODEN_EXT_HI - GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: ODEN_EXT_HI :: oden [31:00] */
#define BCHP_GIO_ODEN_EXT_HI_oden_MASK                             0xffffffff
#define BCHP_GIO_ODEN_EXT_HI_oden_SHIFT                            0
#define BCHP_GIO_ODEN_EXT_HI_oden_DEFAULT                          0x00000000

/***************************************************************************
 *DATA_EXT_HI - GENERAL PURPOSE I/O DATA FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: DATA_EXT_HI :: data [31:00] */
#define BCHP_GIO_DATA_EXT_HI_data_MASK                             0xffffffff
#define BCHP_GIO_DATA_EXT_HI_data_SHIFT                            0
#define BCHP_GIO_DATA_EXT_HI_data_DEFAULT                          0x00000000

/***************************************************************************
 *IODIR_EXT_HI - GENERAL PURPOSE I/O DIRECTION FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: IODIR_EXT_HI :: iodir [31:00] */
#define BCHP_GIO_IODIR_EXT_HI_iodir_MASK                           0xffffffff
#define BCHP_GIO_IODIR_EXT_HI_iodir_SHIFT                          0
#define BCHP_GIO_IODIR_EXT_HI_iodir_DEFAULT                        0xffffffff

/***************************************************************************
 *EC_EXT_HI - GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: EC_EXT_HI :: edge_config [31:00] */
#define BCHP_GIO_EC_EXT_HI_edge_config_MASK                        0xffffffff
#define BCHP_GIO_EC_EXT_HI_edge_config_SHIFT                       0
#define BCHP_GIO_EC_EXT_HI_edge_config_DEFAULT                     0x00000000

/***************************************************************************
 *EI_EXT_HI - GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: EI_EXT_HI :: edge_insensitive [31:00] */
#define BCHP_GIO_EI_EXT_HI_edge_insensitive_MASK                   0xffffffff
#define BCHP_GIO_EI_EXT_HI_edge_insensitive_SHIFT                  0
#define BCHP_GIO_EI_EXT_HI_edge_insensitive_DEFAULT                0x00000000

/***************************************************************************
 *MASK_EXT_HI - GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: MASK_EXT_HI :: irq_mask [31:00] */
#define BCHP_GIO_MASK_EXT_HI_irq_mask_MASK                         0xffffffff
#define BCHP_GIO_MASK_EXT_HI_irq_mask_SHIFT                        0
#define BCHP_GIO_MASK_EXT_HI_irq_mask_DEFAULT                      0x00000000

/***************************************************************************
 *LEVEL_EXT_HI - GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: LEVEL_EXT_HI :: level [31:00] */
#define BCHP_GIO_LEVEL_EXT_HI_level_MASK                           0xffffffff
#define BCHP_GIO_LEVEL_EXT_HI_level_SHIFT                          0
#define BCHP_GIO_LEVEL_EXT_HI_level_DEFAULT                        0x00000000

/***************************************************************************
 *STAT_EXT_HI - GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[95:64]
 ***************************************************************************/
/* GIO :: STAT_EXT_HI :: irq_status [31:00] */
#define BCHP_GIO_STAT_EXT_HI_irq_status_MASK                       0xffffffff
#define BCHP_GIO_STAT_EXT_HI_irq_status_SHIFT                      0
#define BCHP_GIO_STAT_EXT_HI_irq_status_DEFAULT                    0x00000000

/***************************************************************************
 *ODEN_EXT2 - GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: ODEN_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_ODEN_EXT2_reserved0_MASK                          0xffffff80
#define BCHP_GIO_ODEN_EXT2_reserved0_SHIFT                         7

/* GIO :: ODEN_EXT2 :: oden [06:00] */
#define BCHP_GIO_ODEN_EXT2_oden_MASK                               0x0000007f
#define BCHP_GIO_ODEN_EXT2_oden_SHIFT                              0
#define BCHP_GIO_ODEN_EXT2_oden_DEFAULT                            0x00000000

/***************************************************************************
 *DATA_EXT2 - GENERAL PURPOSE I/O DATA FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: DATA_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_DATA_EXT2_reserved0_MASK                          0xffffff80
#define BCHP_GIO_DATA_EXT2_reserved0_SHIFT                         7

/* GIO :: DATA_EXT2 :: data [06:00] */
#define BCHP_GIO_DATA_EXT2_data_MASK                               0x0000007f
#define BCHP_GIO_DATA_EXT2_data_SHIFT                              0
#define BCHP_GIO_DATA_EXT2_data_DEFAULT                            0x00000000

/***************************************************************************
 *IODIR_EXT2 - GENERAL PURPOSE I/O DIRECTION FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: IODIR_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_IODIR_EXT2_reserved0_MASK                         0xffffff80
#define BCHP_GIO_IODIR_EXT2_reserved0_SHIFT                        7

/* GIO :: IODIR_EXT2 :: iodir [06:00] */
#define BCHP_GIO_IODIR_EXT2_iodir_MASK                             0x0000007f
#define BCHP_GIO_IODIR_EXT2_iodir_SHIFT                            0
#define BCHP_GIO_IODIR_EXT2_iodir_DEFAULT                          0x0000007f

/***************************************************************************
 *EC_EXT2 - GENERAL PURPOSE I/O EDGE CONFIGURATION FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: EC_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_EC_EXT2_reserved0_MASK                            0xffffff80
#define BCHP_GIO_EC_EXT2_reserved0_SHIFT                           7

/* GIO :: EC_EXT2 :: edge_config [06:00] */
#define BCHP_GIO_EC_EXT2_edge_config_MASK                          0x0000007f
#define BCHP_GIO_EC_EXT2_edge_config_SHIFT                         0
#define BCHP_GIO_EC_EXT2_edge_config_DEFAULT                       0x00000000

/***************************************************************************
 *EI_EXT2 - GENERAL PURPOSE I/O EDGE INSENSITIVE FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: EI_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_EI_EXT2_reserved0_MASK                            0xffffff80
#define BCHP_GIO_EI_EXT2_reserved0_SHIFT                           7

/* GIO :: EI_EXT2 :: edge_insensitive [06:00] */
#define BCHP_GIO_EI_EXT2_edge_insensitive_MASK                     0x0000007f
#define BCHP_GIO_EI_EXT2_edge_insensitive_SHIFT                    0
#define BCHP_GIO_EI_EXT2_edge_insensitive_DEFAULT                  0x00000000

/***************************************************************************
 *MASK_EXT2 - GENERAL PURPOSE I/O INTERRUPT MASK FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: MASK_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_MASK_EXT2_reserved0_MASK                          0xffffff80
#define BCHP_GIO_MASK_EXT2_reserved0_SHIFT                         7

/* GIO :: MASK_EXT2 :: irq_mask [06:00] */
#define BCHP_GIO_MASK_EXT2_irq_mask_MASK                           0x0000007f
#define BCHP_GIO_MASK_EXT2_irq_mask_SHIFT                          0
#define BCHP_GIO_MASK_EXT2_irq_mask_DEFAULT                        0x00000000

/***************************************************************************
 *LEVEL_EXT2 - GENERAL PURPOSE I/O INTERRUPT TYPE FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: LEVEL_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_LEVEL_EXT2_reserved0_MASK                         0xffffff80
#define BCHP_GIO_LEVEL_EXT2_reserved0_SHIFT                        7

/* GIO :: LEVEL_EXT2 :: level [06:00] */
#define BCHP_GIO_LEVEL_EXT2_level_MASK                             0x0000007f
#define BCHP_GIO_LEVEL_EXT2_level_SHIFT                            0
#define BCHP_GIO_LEVEL_EXT2_level_DEFAULT                          0x00000000

/***************************************************************************
 *STAT_EXT2 - GENERAL PURPOSE I/O INTERRUPT STATUS FOR  GPIO[102:96]
 ***************************************************************************/
/* GIO :: STAT_EXT2 :: reserved0 [31:07] */
#define BCHP_GIO_STAT_EXT2_reserved0_MASK                          0xffffff80
#define BCHP_GIO_STAT_EXT2_reserved0_SHIFT                         7

/* GIO :: STAT_EXT2 :: irq_status [06:00] */
#define BCHP_GIO_STAT_EXT2_irq_status_MASK                         0x0000007f
#define BCHP_GIO_STAT_EXT2_irq_status_SHIFT                        0
#define BCHP_GIO_STAT_EXT2_irq_status_DEFAULT                      0x00000000

/***************************************************************************
 *ODEN_EXT - GENERAL PURPOSE I/O OPEN DRAIN ENABLE FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: ODEN_EXT :: reserved0 [31:06] */
#define BCHP_GIO_ODEN_EXT_reserved0_MASK                           0xffffffc0
#define BCHP_GIO_ODEN_EXT_reserved0_SHIFT                          6

/* GIO :: ODEN_EXT :: oden [05:00] */
#define BCHP_GIO_ODEN_EXT_oden_MASK                                0x0000003f
#define BCHP_GIO_ODEN_EXT_oden_SHIFT                               0
#define BCHP_GIO_ODEN_EXT_oden_DEFAULT                             0x00000000

/***************************************************************************
 *DATA_EXT - GENERAL PURPOSE I/O DATA FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: DATA_EXT :: reserved0 [31:06] */
#define BCHP_GIO_DATA_EXT_reserved0_MASK                           0xffffffc0
#define BCHP_GIO_DATA_EXT_reserved0_SHIFT                          6

/* GIO :: DATA_EXT :: data [05:00] */
#define BCHP_GIO_DATA_EXT_data_MASK                                0x0000003f
#define BCHP_GIO_DATA_EXT_data_SHIFT                               0
#define BCHP_GIO_DATA_EXT_data_DEFAULT                             0x00000000

/***************************************************************************
 *IODIR_EXT - GENERAL PURPOSE I/O DIRECTION FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: IODIR_EXT :: reserved0 [31:06] */
#define BCHP_GIO_IODIR_EXT_reserved0_MASK                          0xffffffc0
#define BCHP_GIO_IODIR_EXT_reserved0_SHIFT                         6

/* GIO :: IODIR_EXT :: iodir [05:00] */
#define BCHP_GIO_IODIR_EXT_iodir_MASK                              0x0000003f
#define BCHP_GIO_IODIR_EXT_iodir_SHIFT                             0
#define BCHP_GIO_IODIR_EXT_iodir_DEFAULT                           0x0000003f

/***************************************************************************
 *EC_EXT - GENERAL PURPOSE I/O EDGE CONFIGURATION FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: EC_EXT :: reserved0 [31:06] */
#define BCHP_GIO_EC_EXT_reserved0_MASK                             0xffffffc0
#define BCHP_GIO_EC_EXT_reserved0_SHIFT                            6

/* GIO :: EC_EXT :: edge_config [05:00] */
#define BCHP_GIO_EC_EXT_edge_config_MASK                           0x0000003f
#define BCHP_GIO_EC_EXT_edge_config_SHIFT                          0
#define BCHP_GIO_EC_EXT_edge_config_DEFAULT                        0x00000000

/***************************************************************************
 *EI_EXT - GENERAL PURPOSE I/O EDGE INSENSITIVE FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: EI_EXT :: reserved0 [31:06] */
#define BCHP_GIO_EI_EXT_reserved0_MASK                             0xffffffc0
#define BCHP_GIO_EI_EXT_reserved0_SHIFT                            6

/* GIO :: EI_EXT :: edge_insensitive [05:00] */
#define BCHP_GIO_EI_EXT_edge_insensitive_MASK                      0x0000003f
#define BCHP_GIO_EI_EXT_edge_insensitive_SHIFT                     0
#define BCHP_GIO_EI_EXT_edge_insensitive_DEFAULT                   0x00000000

/***************************************************************************
 *MASK_EXT - GENERAL PURPOSE I/O INTERRUPT MASK FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: MASK_EXT :: reserved0 [31:06] */
#define BCHP_GIO_MASK_EXT_reserved0_MASK                           0xffffffc0
#define BCHP_GIO_MASK_EXT_reserved0_SHIFT                          6

/* GIO :: MASK_EXT :: irq_mask [05:00] */
#define BCHP_GIO_MASK_EXT_irq_mask_MASK                            0x0000003f
#define BCHP_GIO_MASK_EXT_irq_mask_SHIFT                           0
#define BCHP_GIO_MASK_EXT_irq_mask_DEFAULT                         0x00000000

/***************************************************************************
 *LEVEL_EXT - GENERAL PURPOSE I/O INTERRUPT TYPE FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: LEVEL_EXT :: reserved0 [31:06] */
#define BCHP_GIO_LEVEL_EXT_reserved0_MASK                          0xffffffc0
#define BCHP_GIO_LEVEL_EXT_reserved0_SHIFT                         6

/* GIO :: LEVEL_EXT :: level [05:00] */
#define BCHP_GIO_LEVEL_EXT_level_MASK                              0x0000003f
#define BCHP_GIO_LEVEL_EXT_level_SHIFT                             0
#define BCHP_GIO_LEVEL_EXT_level_DEFAULT                           0x00000000

/***************************************************************************
 *STAT_EXT - GENERAL PURPOSE I/O INTERRUPT STATUS FOR SGPIO[3:0] and AON_SGPIO[1:0]
 ***************************************************************************/
/* GIO :: STAT_EXT :: reserved0 [31:06] */
#define BCHP_GIO_STAT_EXT_reserved0_MASK                           0xffffffc0
#define BCHP_GIO_STAT_EXT_reserved0_SHIFT                          6

/* GIO :: STAT_EXT :: irq_status [05:00] */
#define BCHP_GIO_STAT_EXT_irq_status_MASK                          0x0000003f
#define BCHP_GIO_STAT_EXT_irq_status_SHIFT                         0
#define BCHP_GIO_STAT_EXT_irq_status_DEFAULT                       0x00000000

#endif /* #ifndef BCHP_GIO_H__ */

/* End of File */

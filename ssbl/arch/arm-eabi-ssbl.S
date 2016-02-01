# ***************************************************************************
# *     Copyright (c) 2012-2013, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# *
# ***************************************************************************


.global __aeabi_unwind_cpp_pr0
__aeabi_unwind_cpp_pr0:
	mov		pc, lr


/* int raise(int sig); */
.global raise
raise:
	mov		r0, #0
	mov		pc, lr

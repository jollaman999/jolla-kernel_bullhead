/*
 * cpu reset routines
 *
 * Copyright (C) 2001 Deep Blue Solutions Ltd.
 * Copyright (C) 2012 ARM Ltd.
 * Copyright (C) 2015 Huawei Futurewei Technologies.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/linkage.h>
#include <asm/assembler.h>
#include <asm/virt.h>

#define SCTLR_ELx_I	(1 << 12)
#define SCTLR_ELx_SA	(1 << 3)
#define SCTLR_ELx_C	(1 << 2)
#define SCTLR_ELx_A	(1 << 1)
#define SCTLR_ELx_M	1
#define SCTLR_ELx_FLAGS	(SCTLR_ELx_M | SCTLR_ELx_A | SCTLR_ELx_C |	\
			 SCTLR_ELx_SA | SCTLR_ELx_I)

.text
.pushsection    .idmap.text, "ax"

.align 5

/*
 * cpu_soft_restart(cpu_reset, el2_switch, entry, arg0, arg1) - Perform a cpu
 * soft reset.
 *
 * @cpu_reset: Physical address of the cpu_reset routine.
 * @el2_switch: Flag to indicate a swich to EL2 is needed, passed to cpu_reset.
 * @entry: Location to jump to for soft reset, passed to cpu_reset.
 * arg0: First argument passed to @entry.
 * arg1: Second argument passed to @entry.
 */

ENTRY(cpu_soft_restart)
	mov	x18, x0				// cpu_reset
	mov	x0, x1				// el2_switch
	mov	x1, x2				// entry
	mov	x2, x3				// arg0
	mov	x3, x4				// arg1
	ret	x18
ENDPROC(cpu_soft_restart)

/*
 * cpu_reset(el2_switch, entry, arg0, arg1) - Helper for cpu_soft_restart.
 *
 * @el2_switch: Flag to indicate a swich to EL2 is needed.
 * @entry: Location to jump to for soft reset.
 * arg0: First argument passed to @entry.
 * arg1: Second argument passed to @entry.
 *
 * Put the CPU into the same state as it would be if it had been reset, and
 * branch to what would be the reset vector. It must be executed with the
 * flat identity mapping.
 */

ENTRY(cpu_reset)
	/* Clear sctlr_el1 flags. */
	mrs	x12, sctlr_el1
	ldr	x13, =SCTLR_ELx_FLAGS
	bic	x12, x12, x13
	msr	sctlr_el1, x12
	isb

	cbz	x0, 1f				// el2_switch?

	mov	x0, x1				// entry
	mov	x1, x2				// arg0
	mov	x2, x3				// arg1
	hvc	#HVC_CALL_FUNC			// no return

1:	mov	x18, x1				// entry
	mov	x0, x2				// arg0
	mov	x1, x3				// arg1
	ret	x18
ENDPROC(cpu_reset)

.popsection

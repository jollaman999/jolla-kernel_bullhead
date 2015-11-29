/*
 * kexec for arm64
 *
 * Copyright (C) Linaro.
 * Copyright (C) Huawei Futurewei Technologies.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kexec.h>

#include <asm/assembler.h>
#include <asm/kexec.h>
#include <asm/kvm_arm.h>
#include <linux/const.h>
#include <asm/page.h>

/*
 * arm64_relocate_new_kernel - Put a 2nd stage image in place and boot it.
 *
 * The memory that the old kernel occupies may be overwritten when coping the
 * new image to its final location.  To assure that the
 * arm64_relocate_new_kernel routine which does that copy is not overwritten,
 * all code and data needed by arm64_relocate_new_kernel must be between the
 * symbols arm64_relocate_new_kernel and arm64_relocate_new_kernel_end.  The
 * machine_kexec() routine will copy arm64_relocate_new_kernel to the kexec
 * control_code_page, a special page which has been set up to be preserved
 * during the copy operation.
 */
.globl arm64_relocate_new_kernel
arm64_relocate_new_kernel:

	/* Setup the list loop variables. */
	mov	x18, x0				/* x18 = kimage_head */
	mov	x17, x1				/* x17 = kimage_start */
	dcache_line_size x16, x0		/* x16 = dcache line size */
	mov	x15, xzr			/* x15 = segment start */
	mov	x14, xzr			/* x14 = entry ptr */
	mov	x13, xzr			/* x13 = copy dest */

	/* Clear the sctlr_el2 flags. */
	mrs	x0, CurrentEL
	cmp	x0, #CurrentEL_EL2
	b.ne	1f
	mrs	x0, sctlr_el2
	ldr	x1, =SCTLR_EL2_FLAGS
	bic	x0, x0, x1
	msr	sctlr_el2, x0
	isb
1:

	/* Check if the new image needs relocation. */
	cbz	x18, .Ldone
	tbnz	x18, IND_DONE_BIT, .Ldone

.Lloop:
	and	x12, x18, PAGE_MASK		/* x12 = addr */

	/* Test the entry flags. */
.Ltest_source:
	tbz	x18, IND_SOURCE_BIT, .Ltest_indirection

	mov x20, x13				/*  x20 = copy dest */
	mov x21, x12				/*  x21 = copy src */

	/* Copy page. */
1:	ldp	x22, x23, [x21]
	ldp	x24, x25, [x21, #16]
	ldp	x26, x27, [x21, #32]
	ldp	x28, x29, [x21, #48]
	add	x21, x21, #64
	stnp	x22, x23, [x20]
	stnp	x24, x25, [x20, #16]
	stnp	x26, x27, [x20, #32]
	stnp	x28, x29, [x20, #48]
	add	x20, x20, #64
	tst	x21, #(PAGE_SIZE - 1)
	b.ne	1b

	/* dest += PAGE_SIZE */
	add	x13, x13, PAGE_SIZE
	b	.Lnext

.Ltest_indirection:
	tbz	x18, IND_INDIRECTION_BIT, .Ltest_destination

	/* ptr = addr */
	mov	x14, x12
	b	.Lnext

.Ltest_destination:
	tbz	x18, IND_DESTINATION_BIT, .Lnext

	mov	x15, x12

	/* dest = addr */
	mov	x13, x12

.Lnext:
	/* entry = *ptr++ */
	ldr	x18, [x14], #8

	/* while (!(entry & DONE)) */
	tbz	x18, IND_DONE_BIT, .Lloop

.Ldone:
	dsb	sy
	ic	ialluis
	dsb	sy
	isb

	/* Start new image. */
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
	br	x17

.align 3	/* To keep the 64-bit values below naturally aligned. */

.Lcopy_end:
.org	KEXEC_CONTROL_PAGE_SIZE

/*
 * arm64_relocate_new_kernel_size - Number of bytes to copy to the
 * control_code_page.
 */
.globl arm64_relocate_new_kernel_size
arm64_relocate_new_kernel_size:
	.quad	.Lcopy_end - arm64_relocate_new_kernel

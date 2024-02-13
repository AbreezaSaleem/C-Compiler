	.data
	.text
	.globl main
_Global_f:
	addiu	$sp, $sp, -104
	sw	$fp, 48($sp)
	or	$fp, $sp, $0
	sw	$a0, 0($fp)
	sw	$a1, 4($fp)
	sw	$a2, 8($fp)
	sw	$a3, 12($fp)
	sw	$s0, 16($fp)
	sw	$s1, 20($fp)
	sw	$s2, 24($fp)
	sw	$s3, 28($fp)
	sw	$s4, 32($fp)
	sw	$s5, 36($fp)
	sw	$s6, 40($fp)
	sw	$s7, 44($fp)
	sw	$ra, 52($fp)

	la	$s0, 0($fp)
	la	$s1, 0($fp)
	li	$s2, 2
	lw	$s3, 0($s1)
	mul	$s4, $s3, $s2
	la	$s5, 4($fp)
	lw	$s6, 0($s5)
	mul	$s7, $s4, $s6
	sw	$s7, 0($s0)
	la	$s0, 0($fp)
	lw	$s1, 0($s0)
	move	$v0, $s1

	b _GeneratedLabel_1

_GeneratedLabel_1:

	lw	$ra, 52($fp)
	lw	$s7, 44($fp)
	lw	$s6, 40($fp)
	lw	$s5, 36($fp)
	lw	$s4, 32($fp)
	lw	$s3, 28($fp)
	lw	$s2, 24($fp)
	lw	$s1, 20($fp)
	lw	$s0, 16($fp)
	lw	$fp, 48($fp)
	addiu	$sp, $sp, 104
	jr	$ra
main:
	addiu	$sp, $sp, -104
	sw	$fp, 48($sp)
	or	$fp, $sp, $0
	sw	$a0, 0($fp)
	sw	$a1, 4($fp)
	sw	$a2, 8($fp)
	sw	$a3, 12($fp)
	sw	$s0, 16($fp)
	sw	$s1, 20($fp)
	sw	$s2, 24($fp)
	sw	$s3, 28($fp)
	sw	$s4, 32($fp)
	sw	$s5, 36($fp)
	sw	$s6, 40($fp)
	sw	$s7, 44($fp)
	sw	$ra, 52($fp)

	la	$s0, 100($fp)
	li	$s1, 1
	sw	$s1, 0($s0)
	la	$s0, 96($fp)
	li	$s1, 4
	sw	$s1, 0($s0)
	la	$s0, 100($fp)
	la	$s1, 100($fp)
	lw	$s2, 0($s1)

	move	$a0, $s2

	la	$s3, 96($fp)
	lw	$s4, 0($s3)

	move	$a1, $s4

	sw	$t0, 56($fp)
	sw	$t1, 60($fp)
	sw	$t2, 64($fp)
	sw	$t3, 68($fp)
	sw	$t4, 72($fp)
	sw	$t5, 76($fp)
	sw	$t6, 80($fp)
	sw	$t7, 84($fp)
	sw	$t8, 88($fp)
	sw	$t9, 92($fp)
	jal	_Global_f
	lw	$t9, 92($fp)
	lw	$t8, 88($fp)
	lw	$t7, 84($fp)
	lw	$t6, 80($fp)
	lw	$t5, 76($fp)
	lw	$t4, 72($fp)
	lw	$t3, 68($fp)
	lw	$t2, 64($fp)
	lw	$t1, 60($fp)
	lw	$t0, 56($fp)
	move	$s5, $v0
	sw	$s5, 0($s0)
	la	$s0, 100($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	li	$s0, 0
	move	$v0, $s0

	b _GeneratedLabel_2

_GeneratedLabel_2:

	lw	$ra, 52($fp)
	lw	$s7, 44($fp)
	lw	$s6, 40($fp)
	lw	$s5, 36($fp)
	lw	$s4, 32($fp)
	lw	$s3, 28($fp)
	lw	$s2, 24($fp)
	lw	$s1, 20($fp)
	lw	$s0, 16($fp)
	lw	$fp, 48($fp)
	addiu	$sp, $sp, 104
	jr	$ra

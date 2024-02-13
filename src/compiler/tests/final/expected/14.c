	.data
	.text
	.globl main
_Global_foo:
	addiu	$sp, $sp, -96
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
	addiu	$sp, $sp, 96
	jr	$ra
main:
	addiu	$sp, $sp, -128
	sw	$fp, 52($sp)
	or	$fp, $sp, $0
	sw	$a0, 4($fp)
	sw	$a1, 8($fp)
	sw	$a2, 12($fp)
	sw	$a3, 16($fp)
	sw	$s0, 20($fp)
	sw	$s1, 24($fp)
	sw	$s2, 28($fp)
	sw	$s3, 32($fp)
	sw	$s4, 36($fp)
	sw	$s5, 40($fp)
	sw	$s6, 44($fp)
	sw	$s7, 48($fp)
	sw	$ra, 56($fp)

	la	$s0, 124($fp)
	li	$s1, 2
	sw	$s1, 0($s0)
	la	$s0, 120($fp)
	li	$s1, 3
	sw	$s1, 0($s0)
	la	$s0, 116($fp)
	li	$s1, 4
	sw	$s1, 0($s0)
	la	$s0, 112($fp)
	li	$s1, 5
	sw	$s1, 0($s0)
	la	$s0, 108($fp)
	li	$s1, 6
	sw	$s1, 0($s0)
	la	$s0, 104($fp)
	li	$s1, 7
	sw	$s1, 0($s0)
	la	$s0, 100($fp)
	li	$s1, 8
	sw	$s1, 0($s0)
	la	$s0, 124($fp)
	la	$s1, 120($fp)
	la	$s2, 116($fp)
	la	$s3, 112($fp)
	lw	$s4, 0($s2)
	lw	$s5, 0($s3)
	mul	$s6, $s4, $s5
	la	$s7, 108($fp)
	lw	$t0, 0($s7)
	div	$t1, $s6, $t0
	la	$t2, 104($fp)
	lw	$t3, 0($t2)
	rem	$t4, $t1, $t3
	lw	$t5, 0($s1)
	addu	$t6, $t5, $t4
	la	$t7, 100($fp)
	lw	$t8, 0($t7)
	sgt	$t9, $t6, $t8
	sw	$t9, 0($s0)
	sw	$s7, 60($fp)
	sw	$t0, 64($fp)
	sw	$t1, 68($fp)
	sw	$t2, 72($fp)
	sw	$t3, 76($fp)
	sw	$t4, 80($fp)
	sw	$t5, 84($fp)
	sw	$t6, 88($fp)
	sw	$t7, 92($fp)
	sw	$t8, 96($fp)
	sw	$t9, 100($fp)
	jal	_Global_foo
	lw	$s7, 100($fp)
	lw	$t0, 96($fp)
	lw	$t1, 92($fp)
	lw	$t2, 88($fp)
	lw	$t3, 84($fp)
	lw	$t4, 80($fp)
	lw	$t5, 76($fp)
	lw	$t6, 72($fp)
	lw	$t7, 68($fp)
	lw	$t8, 64($fp)
	lw	$t9, 60($fp)
	move	$s0, $v0
	la	$s0, 124($fp)
	lw	$s1, 0($s0)


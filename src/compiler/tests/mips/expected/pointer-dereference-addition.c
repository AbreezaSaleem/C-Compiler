	.data
	.text
	.globl main
main:
	addiu	$sp, $sp, -112
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

	la	$s0, 108($fp)
	la	$s1, 104($fp)
	sw	$s1, 0($s0)
	la	$s0, 100($fp)
	la	$s1, 96($fp)
	sw	$s1, 0($s0)
	la	$s0, 108($fp)
	lw	$s1, 0($s0)
	li	$s2, 12
	sw	$s2, 0($s1)
	la	$s0, 100($fp)
	lw	$s1, 0($s0)
	li	$s2, 8
	sw	$s2, 0($s1)
	la	$s0, 104($fp)
	la	$s1, 108($fp)
	lw	$s2, 0($s1)
	la	$s3, 100($fp)
	lw	$s4, 0($s3)
	lw	$s5, 0($s2)
	lw	$s6, 0($s4)
	addu	$s7, $s5, $s6
	sw	$s7, 0($s0)
	la	$s0, 104($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	li	$s0, 1
	move	$v0, $s0

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
	addiu	$sp, $sp, 112
	jr	$ra

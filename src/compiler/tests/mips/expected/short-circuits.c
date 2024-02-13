	.data
	.text
	.globl main
main:
	addiu	$sp, $sp, -112
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

	la	$s0, 108($fp)
	li	$s1, 0
	sw	$s1, 0($s0)
	la	$s0, 104($fp)
	li	$s1, 1
	sw	$s1, 0($s0)
	la	$s0, 100($fp)
	la	$s1, 108($fp)
	lw	$s2, 0($s1)

	beqz	$s2 _GeneratedLabel_2
	la	$s3, 104($fp)
	lw	$s4, 0($s3)

	beqz	$s4 _GeneratedLabel_2
	li	$s5, 1

	b _GeneratedLabel_3

_GeneratedLabel_2:
	li	$s5, 0

_GeneratedLabel_3:
	sw	$s5, 0($s0)
	la	$s0, 100($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	move	$s2, $v0
	la	$s0, 100($fp)
	la	$s1, 108($fp)
	lw	$s2, 0($s1)

	bgtz	$s2 _GeneratedLabel_4
	la	$s3, 104($fp)
	lw	$s4, 0($s3)

	bgtz	$s4 _GeneratedLabel_4
	li	$s5, 0

	b _GeneratedLabel_5

_GeneratedLabel_4:
	li	$s5, 1

_GeneratedLabel_5:
	sw	$s5, 0($s0)
	la	$s0, 100($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	move	$s2, $v0
	li	$s0, 0
	move	$v0, $s0

	b _GeneratedLabel_1

_GeneratedLabel_1:

	lw	$ra, 56($fp)
	lw	$s7, 48($fp)
	lw	$s6, 44($fp)
	lw	$s5, 40($fp)
	lw	$s4, 36($fp)
	lw	$s3, 32($fp)
	lw	$s2, 28($fp)
	lw	$s1, 24($fp)
	lw	$s0, 20($fp)
	lw	$fp, 52($fp)
	addiu	$sp, $sp, 112
	jr	$ra

	.data
_StringLabel_0:	.asciiz	"\n"
	.text
	.globl main
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
	li	$s1, 0
	sw	$s1, 0($s0)

_GeneratedLabel_2:
	la	$s0, 100($fp)
	li	$s1, 10
	lw	$s2, 0($s0)
	slt	$s3, $s2, $s1

	beqz	$s3 _GeneratedLabel_3
	la	$s0, 96($fp)
	la	$s1, 100($fp)
	lw	$s2, 0($s1)
	li	$s3, 4
	mul	$s4, $s3, $s2
	addu	$s5, $s0, $s4
	la	$s6, 100($fp)
	la	$s7, 100($fp)
	lw	$t0, 0($s6)
	lw	$t1, 0($s7)
	mul	$t2, $t0, $t1
	sw	$t2, 0($s5)
	la	$s0, 100($fp)
	la	$s1, 100($fp)
	li	$s2, 1
	lw	$s3, 0($s1)
	addu	$s4, $s3, $s2
	sw	$s4, 0($s0)

	b _GeneratedLabel_2

_GeneratedLabel_3:
	la	$s0, 100($fp)
	li	$s1, 0
	sw	$s1, 0($s0)

_GeneratedLabel_4:
	la	$s0, 100($fp)
	li	$s1, 10
	lw	$s2, 0($s0)
	slt	$s3, $s2, $s1

	beqz	$s3 _GeneratedLabel_5
	la	$s0, 96($fp)
	la	$s1, 100($fp)
	lw	$s2, 0($s1)
	li	$s3, 4
	mul	$s4, $s3, $s2
	addu	$s5, $s0, $s4
	lw	$s6, 0($s5)

	move	$a0, $s6


	li	$v0, 1
	syscall

	la	$s0, _StringLabel_0

	move	$a0, $s0


	li	$v0, 4
	syscall

	la	$s0, 100($fp)
	la	$s1, 100($fp)
	li	$s2, 1
	lw	$s3, 0($s1)
	addu	$s4, $s3, $s2
	sw	$s4, 0($s0)

	b _GeneratedLabel_4

_GeneratedLabel_5:
	li	$s0, 0
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
	addiu	$sp, $sp, 104
	jr	$ra





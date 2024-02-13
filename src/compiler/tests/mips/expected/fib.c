	.data
	.text
	.globl main
_Global_fib:
	addiu	$sp, $sp, -104
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

	la	$s0, 4($fp)
	lh	$s1, 0($s0)
	sll	$s2, $s1, 16
	sra	$s2, $s2, 16
	li	$s3, 2
	slt	$s4, $s2, $s3

	beqz	$s4 _GeneratedLabel_2
	li	$s5, 1

	b _GeneratedLabel_3

_GeneratedLabel_2:
	la	$s0, 4($fp)
	lh	$s1, 0($s0)
	sll	$s2, $s1, 16
	sra	$s2, $s2, 16
	li	$s3, 1
	sub	$s4, $s2, $s3
	move	$s6, $s4

	move	$a0, $s6

	sw	$t0, 60($fp)
	sw	$t1, 64($fp)
	sw	$t2, 68($fp)
	sw	$t3, 72($fp)
	sw	$t4, 76($fp)
	sw	$t5, 80($fp)
	sw	$t6, 84($fp)
	sw	$t7, 88($fp)
	sw	$t8, 92($fp)
	sw	$t9, 96($fp)
	jal	_Global_fib
	lw	$t9, 96($fp)
	lw	$t8, 92($fp)
	lw	$t7, 88($fp)
	lw	$t6, 84($fp)
	lw	$t5, 80($fp)
	lw	$t4, 76($fp)
	lw	$t3, 72($fp)
	lw	$t2, 68($fp)
	lw	$t1, 64($fp)
	lw	$t0, 60($fp)
	move	$s7, $v0
	la	$t0, 4($fp)
	lh	$t1, 0($t0)
	sll	$t2, $t1, 16
	sra	$t2, $t2, 16
	li	$t3, 2
	sub	$t4, $t2, $t3
	move	$t5, $t4

	move	$a0, $t5

	sw	$t0, 60($fp)
	sw	$t1, 64($fp)
	sw	$t2, 68($fp)
	sw	$t3, 72($fp)
	sw	$t4, 76($fp)
	sw	$t5, 80($fp)
	sw	$t6, 84($fp)
	sw	$t7, 88($fp)
	sw	$t8, 92($fp)
	sw	$t9, 96($fp)
	jal	_Global_fib
	lw	$t9, 96($fp)
	lw	$t8, 92($fp)
	lw	$t7, 88($fp)
	lw	$t6, 84($fp)
	lw	$t5, 80($fp)
	lw	$t4, 76($fp)
	lw	$t3, 72($fp)
	lw	$t2, 68($fp)
	lw	$t1, 64($fp)
	lw	$t0, 60($fp)
	move	$t6, $v0
	addu	$s5, $s7, $t6

_GeneratedLabel_3:
	move	$t7, $s5
	move	$v0, $t7

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
	addiu	$sp, $sp, 104
	jr	$ra
main:
	addiu	$sp, $sp, -104
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

	la	$s0, 100($fp)
	li	$s1, 5
	move	$s2, $s1

	move	$a0, $s2

	sw	$t0, 60($fp)
	sw	$t1, 64($fp)
	sw	$t2, 68($fp)
	sw	$t3, 72($fp)
	sw	$t4, 76($fp)
	sw	$t5, 80($fp)
	sw	$t6, 84($fp)
	sw	$t7, 88($fp)
	sw	$t8, 92($fp)
	sw	$t9, 96($fp)
	jal	_Global_fib
	lw	$t9, 96($fp)
	lw	$t8, 92($fp)
	lw	$t7, 88($fp)
	lw	$t6, 84($fp)
	lw	$t5, 80($fp)
	lw	$t4, 76($fp)
	lw	$t3, 72($fp)
	lw	$t2, 68($fp)
	lw	$t1, 64($fp)
	lw	$t0, 60($fp)
	move	$s3, $v0
	sll	$s4, $s3, 16
	sra	$s4, $s4, 16
	sw	$s4, 0($s0)
	la	$s0, 100($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	move	$s2, $v0
	li	$s0, 0
	move	$v0, $s0

	b _GeneratedLabel_4

_GeneratedLabel_4:

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
	addiu	$sp, $sp, 104
	jr	$ra

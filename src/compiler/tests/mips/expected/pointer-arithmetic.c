	.data
_StringLabel_0:	.asciiz	"Element at index "
_StringLabel_1:	.asciiz	": "
_StringLabel_2:	.asciiz	"\n"
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
	li	$s1, 0
	li	$s2, 4
	mul	$s3, $s2, $s1
	addu	$s4, $s0, $s3
	li	$s5, 1
	sw	$s5, 0($s4)
	la	$s0, 108($fp)
	li	$s1, 1
	li	$s2, 4
	mul	$s3, $s2, $s1
	addu	$s4, $s0, $s3
	li	$s5, 2
	sw	$s5, 0($s4)
	la	$s0, 108($fp)
	li	$s1, 2
	li	$s2, 4
	mul	$s3, $s2, $s1
	addu	$s4, $s0, $s3
	li	$s5, 3
	sw	$s5, 0($s4)
	la	$s0, 104($fp)
	la	$s1, 108($fp)
	sw	$s1, 0($s0)
	la	$s0, 100($fp)
	li	$s1, 0
	sw	$s1, 0($s0)

_GeneratedLabel_2:
	la	$s0, 100($fp)
	li	$s1, 3
	lw	$s2, 0($s0)
	slt	$s3, $s2, $s1

	beqz	$s3 _GeneratedLabel_3
	la	$s0, _StringLabel_0

	move	$a0, $s0


	li	$v0, 4
	syscall

	la	$s0, 100($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	la	$s0, _StringLabel_1

	move	$a0, $s0


	li	$v0, 4
	syscall

	la	$s0, 104($fp)
	la	$s1, 100($fp)
	lw	$s2, 0($s0)
	lw	$s3, 0($s1)
	li	$s4, 4
	mul	$s5, $s4, $s3
	addu	$s6, $s2, $s5
	lw	$s7, 0($s6)

	move	$a0, $s7


	li	$v0, 1
	syscall

	la	$s0, _StringLabel_2

	move	$a0, $s0


	li	$v0, 4
	syscall

	la	$s0, 100($fp)
	lw	$s1, 0($s0)
	li	$s2, 1
	addu	$s3, $s1, $s2
	sw	$s3, 0($s0)

	b _GeneratedLabel_2

_GeneratedLabel_3:
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

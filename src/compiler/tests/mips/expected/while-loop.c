	.data
_StringLabel_0:	.asciiz	"n: "
_StringLabel_1:	.asciiz	"\n"
	.text
	.globl main
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
	li	$s1, 0
	sw	$s1, 0($s0)

_GeneratedLabel_2:
	la	$s0, 100($fp)
	li	$s1, 5
	lw	$s2, 0($s0)
	slt	$s3, $s2, $s1

	beqz	$s3 _GeneratedLabel_3
	la	$s0, _StringLabel_0

	move	$a0, $s0


	li	$v0, 4
	syscall

	move	$s1, $v0
	la	$s0, 100($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	move	$s2, $v0
	la	$s0, _StringLabel_1

	move	$a0, $s0


	li	$v0, 4
	syscall

	move	$s1, $v0
	la	$s0, 100($fp)
	lw	$s1, 0($s0)
	li	$s2, 1
	addu	$s3, $s1, $s2
	sw	$s3, 0($s0)

	b _GeneratedLabel_2

_GeneratedLabel_3:
	la	$s0, _StringLabel_0

	move	$a0, $s0


	li	$v0, 4
	syscall

	move	$s1, $v0
	la	$s0, 100($fp)
	lw	$s1, 0($s0)

	move	$a0, $s1


	li	$v0, 1
	syscall

	move	$s2, $v0
	la	$s0, _StringLabel_1

	move	$a0, $s0


	li	$v0, 4
	syscall

	move	$s1, $v0
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
	addiu	$sp, $sp, 104
	jr	$ra

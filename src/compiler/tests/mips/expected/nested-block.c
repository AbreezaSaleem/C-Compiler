	.data
_StringLabel_0:	.asciiz	"Block 1: "
_StringLabel_1:	.asciiz	"\n"
_StringLabel_2:	.asciiz	"Block 2: "
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
	li	$s1, 100
	sw	$s1, 0($s0)
	la	$s0, 96($fp)
	li	$s1, 200
	sw	$s1, 0($s0)
	la	$s0, _StringLabel_0

	move	$a0, $s0


	li	$v0, 4
	syscall

	move	$s1, $v0
	la	$s0, 100($fp)
	la	$s1, 96($fp)
	lw	$s2, 0($s0)
	lw	$s3, 0($s1)
	addu	$s4, $s2, $s3

	move	$a0, $s4


	li	$v0, 1
	syscall

	move	$s5, $v0
	la	$s0, _StringLabel_1

	move	$a0, $s0


	li	$v0, 4
	syscall

	move	$s1, $v0
	la	$s0, 92($fp)
	li	$s1, 300
	sw	$s1, 0($s0)
	la	$s0, _StringLabel_2

	move	$a0, $s0


	li	$v0, 4
	syscall

	move	$s1, $v0
	la	$s0, 100($fp)
	la	$s1, 92($fp)
	lw	$s2, 0($s0)
	lw	$s3, 0($s1)
	addu	$s4, $s2, $s3

	move	$a0, $s4


	li	$v0, 1
	syscall

	move	$s5, $v0
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

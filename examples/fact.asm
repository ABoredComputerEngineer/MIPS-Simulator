# This is a program to calculate the factorial of a number
jmp Start
Fact:
addi $sp,$sp,-8
sw   $ra,0($sp)
sw   $a0,4($sp)
slti $t0,$a0,2
beq  $t0,$zero,Loop
addi $v0,$zero,1
jr   $ra
Loop:
addi $a0,$a0,-1
jal  Fact
lw   $ra,0($sp)
lw   $a0,4($sp)
addi $sp,$sp,8
mult $v0,$v0,$a0
jr   $ra

Start:
addi $a0,$zero,3
jal  Fact

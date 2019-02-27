# Strcpy
jmp  Start
Strcpy:
#Assume the address of the two strings are in the 
#argument registers a0 and a1
#This function copies contents of a1 into a0
lbu  $t0,0($a1)
sb   $t0,0($a0)
addi $a1,$a1,1
addi $a0,$a0,1
beq  $t0,$zero,Exit
jmp  Strcpy
Start:
# Load some values , Starting address is $gp
addi $s0,$zero,65
sb   $s0,0($gp)
addi $s0,$zero,66
sb   $s0,1($gp)
addi $s0,$zero,67
sb   $s0,2($gp)
addi $s0,$zero,68
sb   $s0,3($gp)
addi $s0,$zero,69
sb   $s0,4($gp)
addr $s0,$zero,$zero
sb   $s0,5($gp)
addi $a0,$gp,32
addr $a1,$zero,$gp
jmp  Strcpy
Exit:


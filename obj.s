.data
input_info:.asciiz "Enter an integer:"
ret_info:.asciiz"\n"
.globl main
.text
read:
    li $v0, 4
    la $a0, input_info
    syscall
    li $v0, 5
    syscall
    jr $ra
write:
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, ret_info
    syscall
    move $v0, $0
    jr $ra

fibo:
    li $t3, 1
    sw $t3, 16($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    beq $t1, $t2, label_2
    j label_4
label_4:
    li $t3, 2
    sw $t3, 16($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    beq $t1, $t2, label_2
    j label_3
label_2:
    li $t3, 1
    sw $t3, 16($sp)
    lw $v0, 16($sp)
    jr $ra
    j label_1
label_3:
    li $t3, 1
    sw $t3, 16($sp)
    lw $t1, 12($sp)
    lw $t2, 16($sp)
    sub $t3, $t1, $t2
    sw $t3, 20($sp)
    move $t0, $sp
    addi $sp, $sp, -44
    sw $ra, 0($sp)
    lw $t1, 20($t0)
    move $t3, $t1
    sw $t3, 12($sp)
    jal fibo
    lw $ra, 0($sp)
    addi $sp, $sp,44
    sw $v0, 24($sp)
    li $t3, 2
    sw $t3, 28($sp)
    lw $t1, 12($sp)
    lw $t2, 28($sp)
    sub $t3, $t1, $t2
    sw $t3, 32($sp)
    move $t0, $sp
    addi $sp, $sp, -44
    sw $ra, 0($sp)
    lw $t1, 32($t0)
    move $t3, $t1
    sw $t3, 12($sp)
    jal fibo
    lw $ra, 0($sp)
    addi $sp, $sp,44
    sw $v0, 36($sp)
    lw $t1, 24($sp)
    lw $t2, 36($sp)
    add $t3, $t1, $t2
    sw $t3, 40($sp)
    lw $v0, 40($sp)
    jr $ra
label_1:

main:
    addi $sp, $sp, -76
    li $t3, 1
    sw $t3, 16($sp)
    lw $t1, 16($sp)
    move $t3, $t1
    sw $t3, 12($sp)
    li $t3, 2
    sw $t3, 24($sp)
    lw $t1, 24($sp)
    move $t3, $t1
    sw $t3, 20($sp)
    li $t3, 1
    sw $t3, 56($sp)
    lw $t1, 56($sp)
    move $t3, $t1
    sw $t3, 52($sp)
    li $t3, 97
    sw $t3, 64($sp)
    lw $t1, 64($sp)
    move $t3, $t1
    sw $t3, 60($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal read
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    sw $v0, 68($sp)
    lw $t1, 68($sp)
    move $t3, $t1
    sw $t3, 40($sp)
    lw $a0, 40($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
label_10:
    li $t3, 4
    sw $t3, 68($sp)
    lw $t1, 52($sp)
    lw $t2, 68($sp)
    ble $t1, $t2, label_9
    j label_8
label_9:
    li $t3, 97
    sw $t3, 68($sp)
    lw $t1, 60($sp)
    lw $t2, 68($sp)
    beq $t1, $t2, label_12
    j label_13
label_12:
    li $t3, 1
    sw $t3, 68($sp)
    lw $t1, 52($sp)
    lw $t2, 68($sp)
    add $t3, $t1, $t2
    sw $t3, 52($sp)
    li $t3, 66
    sw $t3, 68($sp)
    lw $t1, 68($sp)
    move $t3, $t1
    sw $t3, 60($sp)
    j label_10
    j label_11
label_13:
    li $t3, 3
    sw $t3, 68($sp)
    lw $t1, 52($sp)
    lw $t2, 68($sp)
    beq $t1, $t2, label_16
    j label_11
label_16:
    j label_8
label_11:
    lw $t1, 52($sp)
    li $t2, 1
    add $t3, $t1, $t2
    sw $t3, 52($sp)
    lw $t1, 52($sp)
    lw $t2, 68($sp)
    div $t1, $t2
    mflo $t3
    sw $t3, 68($sp)
    j label_10
label_8:
    lw $a0, 52($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    li $t3, 0
    sw $t3, 68($sp)
    lw $v0, 68($sp)
    jr $ra
label_5:

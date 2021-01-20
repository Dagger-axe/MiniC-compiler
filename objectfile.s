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
    addi $sp, $sp, -72
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
    li $t3, 97
    sw $t3, 60($sp)
    lw $t1, 60($sp)
    move $t3, $t1
    sw $t3, 56($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal read
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    sw $v0, 64($sp)
    lw $t1, 64($sp)
    move $t3, $t1
    sw $t3, 52($sp)
    move $t0, $sp
    addi $sp, $sp, -44
    sw $ra, 0($sp)
    lw $t1, 52($t0)
    move $t3, $t1
    sw $t3, 12($sp)
    jal fibo
    lw $ra, 0($sp)
    addi $sp, $sp,44
    sw $v0, 64($sp)
    lw $a0, 64($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    li $t3, 1
    sw $t3, 64($sp)
    lw $t1, 64($sp)
    move $t3, $t1
    sw $t3, 52($sp)
label_11:
    li $t3, 4
    sw $t3, 64($sp)
    lw $t1, 52($sp)
    lw $t2, 64($sp)
    ble $t1, $t2, label_10
    j label_9
label_10:
    li $t3, 97
    sw $t3, 64($sp)
    lw $t1, 56($sp)
    lw $t2, 64($sp)
    beq $t1, $t2, label_13
    j label_14
label_13:
    li $t3, 1
    sw $t3, 64($sp)
    lw $t1, 52($sp)
    lw $t2, 64($sp)
    add $t3, $t1, $t2
    sw $t3, 52($sp)
    li $t3, 66
    sw $t3, 64($sp)
    lw $t1, 64($sp)
    move $t3, $t1
    sw $t3, 56($sp)
    j label_11
    j label_12
label_14:
    li $t3, 3
    sw $t3, 64($sp)
    lw $t1, 52($sp)
    lw $t2, 64($sp)
    beq $t1, $t2, label_17
    j label_12
label_17:
    j label_9
label_12:
    lw $t1, 52($sp)
    li $t2, 1
    add $t3, $t1, $t2
    sw $t3, 52($sp)
    lw $t1, 52($sp)
    lw $t2, 64($sp)
    div $t1, $t2
    mflo $t3
    sw $t3, 64($sp)
    j label_11
label_9:
    lw $a0, 52($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    li $t3, 0
    sw $t3, 64($sp)
    lw $t1, 64($sp)
    move $t3, $t1
    sw $t3, 52($sp)
    li $t3, 0
    sw $t3, 64($sp)
    lw $t1, 64($sp)
    move $t3, $t1
    sw $t3, 12($sp)
label_21:
    li $t3, 2
    sw $t3, 68($sp)
    lw $t1, 12($sp)
    lw $t2, 68($sp)
    blt $t1, $t2, label_22
    j label_20
label_23:
    lw $t1, 12($sp)
    li $t2, 1
    add $t3, $t1, $t2
    sw $t3, 12($sp)
    lw $t1, 12($sp)
    lw $t2, 72($sp)
    div $t1, $t2
    mflo $t3
    sw $t3, 72($sp)
    j label_21
label_22:
    li $t3, 0
    sw $t3, 76($sp)
    lw $t1, 76($sp)
    move $t3, $t1
    sw $t3, 20($sp)
label_25:
    li $t3, 3
    sw $t3, 80($sp)
    lw $t1, 20($sp)
    lw $t2, 80($sp)
    blt $t1, $t2, label_26
    j label_24
label_27:
    lw $t1, 20($sp)
    li $t2, 1
    add $t3, $t1, $t2
    sw $t3, 20($sp)
    lw $t1, 20($sp)
    lw $t2, 84($sp)
    div $t1, $t2
    mflo $t3
    sw $t3, 84($sp)
    j label_25
label_26:
    lw $t1, 12($sp)
    lw $t2, 20($sp)
    add $t3, $t1, $t2
    sw $t3, 88($sp)
    lw $t1, 88($sp)
    move $t3, $t1
    sw $t3, -88($sp)
    lw $t1, 52($sp)
    lw $t2, -88($sp)
    add $t3, $t1, $t2
    sw $t3, 52($sp)
    j label_27
label_24:
    lw $t1, 52($sp)
    li $t2, 1
    add $t3, $t1, $t2
    sw $t3, 52($sp)
    lw $t1, 52($sp)
    lw $t2, 76($sp)
    div $t1, $t2
    mflo $t3
    sw $t3, 76($sp)
    j label_23
label_20:
    lw $a0, 52($sp)
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    jal write
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    li $t3, 0
    sw $t3, 64($sp)
    lw $v0, 64($sp)
    jr $ra
label_5:

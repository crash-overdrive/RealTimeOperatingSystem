.globl softwareInterrupt
.type softwareInterrupt, %function
// takes in softwareInterrupt code
// returns to user Task with the right value in r0 if swi expected a correct value
softwareInterrupt:
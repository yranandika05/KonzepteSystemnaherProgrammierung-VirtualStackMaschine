_subFraction:
	asf	0
	pushl	-4
	getf	0
	pushl	-3
	getf	1
	mul
	pushl	-3
	getf	0
	pushl	-4
	getf	1
	mul
	sub
	pushl	-4
	getf	1
	pushl	-3
	getf	1
	mul
	call	_newFraction
	drop	2
	pushr
	popr
	jmp	__14
__14:
	rsf
	ret

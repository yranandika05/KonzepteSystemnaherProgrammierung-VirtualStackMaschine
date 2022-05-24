_newMatrix:
	asf	1
	pushc	2
	newa
	popl	0
	pushl	0
	pushc	0
	pushc	2
	newa
	putfa
	pushl	0
	pushc	1
	pushc	2
	newa
	putfa
	pushl	0
	pushc	0
	getfa
	pushc	0
	pushl	-6
	putfa
	pushl	0
	pushc	0
	getfa
	pushc	1
	pushl	-5
	putfa
	pushl	0
	pushc	1
	getfa
	pushc	0
	pushl	-4
	putfa
	pushl	0
	pushc	1
	getfa
	pushc	1
	pushl	-3
	putfa
	pushl	0
	popr
	jmp	__17
__17:
	rsf
	ret

_newFraction:
	asf	4
	pushl	-4
	pushc	0
	lt
	brf	__5
	pushc	0
	pushl	-4
	sub
	popl	0
	jmp	__6
__5:
	pushl	-4
	popl	0
__6:
	pushl	-3
	pushc	0
	lt
	brf	__7
	pushc	0
	pushl	-3
	sub
	popl	1
	jmp	__8
__7:
	pushl	-3
	popl	1
__8:
	pushl	0
	pushl	1
	call	_gcd
	drop	2
	pushr
	popl	2
	new	2
	popl	3
	pushl	-4
	pushc	0
	lt
	pushl	-3
	pushc	0
	lt
	ne
	brf	__9
	pushl	3
	pushc	0
	pushl	0
	sub
	pushl	2
	div
	putf	0
	jmp	__10
__9:
	pushl	3
	pushl	0
	pushl	2
	div
	putf	0
__10:
	pushl	3
	pushl	1
	pushl	2
	div
	putf	1
	pushl	3
	popr
	jmp	__4
__4:
	rsf
	ret

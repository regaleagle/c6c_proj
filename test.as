	push	103
	pop	sp
	geti
	pop	sb[0]
	gets
	pop	sb[1]
	push	sb[0]
	push	2
	add
	pop	in
	push	5
	pop	sb[in]
	push	sb[0]
	push	2
	add
	pop	in
	push	sb[in]
	pop	sb[102]
	push	"OTAY"
	puts
	push	sb[102]
	puti
L000:
	push	sb[0]
	push	0
	compgt
	j0	L001
	push	sb[0]
	push	2
	compeq
	j0	L002
	push	sb[0]
	push	1
	sub
	pop	sb[0]
	jmp	L000
L002:
	push	sb[0]
	push	9
	compgt
	j0	L003
	jmp	L001
L003:
	push	sb[1]
	puts
	push	sb[0]
	puti
	push	sb[102]
	puti
	push	"thing"
	puts_
	push	sb[0]
	push	1
	sub
	pop	sb[0]
	jmp	L000
L001:

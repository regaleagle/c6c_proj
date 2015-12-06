	push	102
	pop	sp
	geti
	pop	sb[0]
	push	101
	pop	in
L000:
	push	in
	push	1
	sub
	pop	in
	push	20
	pop	sb[in]
	push	in
	push	1
	compgt
	j1	L000
	push	sb[0]
	push	1
	add
	pop	in
	push	5
	pop	sb[in]
	push	sb[0]
	push	1
	add
	pop	in
	push	sb[in]
	pop	sb[101]
	push	sb[0]
	puti
	push	sb[101]
	puti
	push	0
	push	1
	add
	pop	in
	push	sb[in]
	puti

.text
.align 16

.globl md5_block_asm_host_order
.type md5_block_asm_host_order,@function
md5_block_asm_host_order:
	push	%rbp
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	# rdi = arg #1 (ctx, MD5_CTX pointer)
	# rsi = arg #2 (ptr, data pointer)
	# rdx = arg #3 (nbr, number of 16-word blocks to process)
	mov	%rdi,		%rbp	# rbp = ctx
	shl	$6,		%rdx	# rdx = nbr in bytes
	lea	(%rsi,%rdx),	%rdi	# rdi = end
	mov	0*4(%rbp),	%eax	# eax = ctx->A
	mov	1*4(%rbp),	%ebx	# ebx = ctx->B
	mov	2*4(%rbp),	%ecx	# ecx = ctx->C
	mov	3*4(%rbp),	%edx	# edx = ctx->D
	push	%rbp			# save ctx
	# end is 'rdi'
	# ptr is 'rsi'
	# A is 'eax'
	# B is 'ebx'
	# C is 'ecx'
	# D is 'edx'

	cmp	%rdi,		%rsi		# cmp end with ptr
	je	1f				# jmp if ptr == end

	# BEGIN of loop over 16-word blocks
2:	# save old values of A, B, C, D
	mov	%eax,		%r8d
	mov	%ebx,		%r9d
	mov	%ecx,		%r14d
	mov	%edx,		%r15d
 mov	0*4(%rsi),	%r10d		/* (NEXT STEP) X[0] */
 mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	xor	%ecx,		%r11d		/* y ^ ... */
	lea	0xd76aa478(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r11d		/* x & ... */
	xor	%edx,		%r11d		/* z ^ ... */
	mov	1*4(%rsi),%r10d		/* (NEXT STEP) X[1] */
	add	%r11d,		%eax		/* dst += ... */
	rol	$7,		%eax		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%ebx,		%eax		/* dst += x */
	xor	%ebx,		%r11d		/* y ^ ... */
	lea	0xe8c7b756(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r11d		/* x & ... */
	xor	%ecx,		%r11d		/* z ^ ... */
	mov	2*4(%rsi),%r10d		/* (NEXT STEP) X[2] */
	add	%r11d,		%edx		/* dst += ... */
	rol	$12,		%edx		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%eax,		%edx		/* dst += x */
	xor	%eax,		%r11d		/* y ^ ... */
	lea	0x242070db(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r11d		/* x & ... */
	xor	%ebx,		%r11d		/* z ^ ... */
	mov	3*4(%rsi),%r10d		/* (NEXT STEP) X[3] */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$17,		%ecx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%edx,		%ecx		/* dst += x */
	xor	%edx,		%r11d		/* y ^ ... */
	lea	0xc1bdceee(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r11d		/* x & ... */
	xor	%eax,		%r11d		/* z ^ ... */
	mov	4*4(%rsi),%r10d		/* (NEXT STEP) X[4] */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$22,		%ebx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%ecx,		%ebx		/* dst += x */
	xor	%ecx,		%r11d		/* y ^ ... */
	lea	0xf57c0faf(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r11d		/* x & ... */
	xor	%edx,		%r11d		/* z ^ ... */
	mov	5*4(%rsi),%r10d		/* (NEXT STEP) X[5] */
	add	%r11d,		%eax		/* dst += ... */
	rol	$7,		%eax		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%ebx,		%eax		/* dst += x */
	xor	%ebx,		%r11d		/* y ^ ... */
	lea	0x4787c62a(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r11d		/* x & ... */
	xor	%ecx,		%r11d		/* z ^ ... */
	mov	6*4(%rsi),%r10d		/* (NEXT STEP) X[6] */
	add	%r11d,		%edx		/* dst += ... */
	rol	$12,		%edx		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%eax,		%edx		/* dst += x */
	xor	%eax,		%r11d		/* y ^ ... */
	lea	0xa8304613(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r11d		/* x & ... */
	xor	%ebx,		%r11d		/* z ^ ... */
	mov	7*4(%rsi),%r10d		/* (NEXT STEP) X[7] */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$17,		%ecx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%edx,		%ecx		/* dst += x */
	xor	%edx,		%r11d		/* y ^ ... */
	lea	0xfd469501(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r11d		/* x & ... */
	xor	%eax,		%r11d		/* z ^ ... */
	mov	8*4(%rsi),%r10d		/* (NEXT STEP) X[8] */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$22,		%ebx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%ecx,		%ebx		/* dst += x */
	xor	%ecx,		%r11d		/* y ^ ... */
	lea	0x698098d8(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r11d		/* x & ... */
	xor	%edx,		%r11d		/* z ^ ... */
	mov	9*4(%rsi),%r10d		/* (NEXT STEP) X[9] */
	add	%r11d,		%eax		/* dst += ... */
	rol	$7,		%eax		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%ebx,		%eax		/* dst += x */
	xor	%ebx,		%r11d		/* y ^ ... */
	lea	0x8b44f7af(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r11d		/* x & ... */
	xor	%ecx,		%r11d		/* z ^ ... */
	mov	10*4(%rsi),%r10d		/* (NEXT STEP) X[10] */
	add	%r11d,		%edx		/* dst += ... */
	rol	$12,		%edx		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%eax,		%edx		/* dst += x */
	xor	%eax,		%r11d		/* y ^ ... */
	lea	0xffff5bb1(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r11d		/* x & ... */
	xor	%ebx,		%r11d		/* z ^ ... */
	mov	11*4(%rsi),%r10d		/* (NEXT STEP) X[11] */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$17,		%ecx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%edx,		%ecx		/* dst += x */
	xor	%edx,		%r11d		/* y ^ ... */
	lea	0x895cd7be(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r11d		/* x & ... */
	xor	%eax,		%r11d		/* z ^ ... */
	mov	12*4(%rsi),%r10d		/* (NEXT STEP) X[12] */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$22,		%ebx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%ecx,		%ebx		/* dst += x */
	xor	%ecx,		%r11d		/* y ^ ... */
	lea	0x6b901122(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r11d		/* x & ... */
	xor	%edx,		%r11d		/* z ^ ... */
	mov	13*4(%rsi),%r10d		/* (NEXT STEP) X[13] */
	add	%r11d,		%eax		/* dst += ... */
	rol	$7,		%eax		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%ebx,		%eax		/* dst += x */
	xor	%ebx,		%r11d		/* y ^ ... */
	lea	0xfd987193(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r11d		/* x & ... */
	xor	%ecx,		%r11d		/* z ^ ... */
	mov	14*4(%rsi),%r10d		/* (NEXT STEP) X[14] */
	add	%r11d,		%edx		/* dst += ... */
	rol	$12,		%edx		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%eax,		%edx		/* dst += x */
	xor	%eax,		%r11d		/* y ^ ... */
	lea	0xa679438e(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r11d		/* x & ... */
	xor	%ebx,		%r11d		/* z ^ ... */
	mov	15*4(%rsi),%r10d		/* (NEXT STEP) X[15] */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$17,		%ecx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%edx,		%ecx		/* dst += x */
	xor	%edx,		%r11d		/* y ^ ... */
	lea	0x49b40821(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r11d		/* x & ... */
	xor	%eax,		%r11d		/* z ^ ... */
	mov	0*4(%rsi),%r10d		/* (NEXT STEP) X[0] */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$22,		%ebx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%ecx,		%ebx		/* dst += x */
 mov	1*4(%rsi),	%r10d		/* (NEXT STEP) X[1] */
 mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
 mov	%edx,		%r12d		/* (NEXT STEP) z' = %edx */
	not	%r11d				/* not z */
	lea	0xf61e2562(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r12d		/* x & z */
	and	%ecx,		%r11d		/* y & (not z) */
	mov	6*4(%rsi),%r10d		/* (NEXT STEP) X[6] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%r12d,		%eax		/* dst += ... */
	mov	%ecx,		%r12d		/* (NEXT STEP) z' = %ecx */
	rol	$5,		%eax		/* dst <<< s */
	add	%ebx,		%eax		/* dst += x */
	not	%r11d				/* not z */
	lea	0xc040b340(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r12d		/* x & z */
	and	%ebx,		%r11d		/* y & (not z) */
	mov	11*4(%rsi),%r10d		/* (NEXT STEP) X[11] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%r12d,		%edx		/* dst += ... */
	mov	%ebx,		%r12d		/* (NEXT STEP) z' = %ebx */
	rol	$9,		%edx		/* dst <<< s */
	add	%eax,		%edx		/* dst += x */
	not	%r11d				/* not z */
	lea	0x265e5a51(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r12d		/* x & z */
	and	%eax,		%r11d		/* y & (not z) */
	mov	0*4(%rsi),%r10d		/* (NEXT STEP) X[0] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%r12d,		%ecx		/* dst += ... */
	mov	%eax,		%r12d		/* (NEXT STEP) z' = %eax */
	rol	$14,		%ecx		/* dst <<< s */
	add	%edx,		%ecx		/* dst += x */
	not	%r11d				/* not z */
	lea	0xe9b6c7aa(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r12d		/* x & z */
	and	%edx,		%r11d		/* y & (not z) */
	mov	5*4(%rsi),%r10d		/* (NEXT STEP) X[5] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%r12d,		%ebx		/* dst += ... */
	mov	%edx,		%r12d		/* (NEXT STEP) z' = %edx */
	rol	$20,		%ebx		/* dst <<< s */
	add	%ecx,		%ebx		/* dst += x */
	not	%r11d				/* not z */
	lea	0xd62f105d(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r12d		/* x & z */
	and	%ecx,		%r11d		/* y & (not z) */
	mov	10*4(%rsi),%r10d		/* (NEXT STEP) X[10] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%r12d,		%eax		/* dst += ... */
	mov	%ecx,		%r12d		/* (NEXT STEP) z' = %ecx */
	rol	$5,		%eax		/* dst <<< s */
	add	%ebx,		%eax		/* dst += x */
	not	%r11d				/* not z */
	lea	0x2441453(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r12d		/* x & z */
	and	%ebx,		%r11d		/* y & (not z) */
	mov	15*4(%rsi),%r10d		/* (NEXT STEP) X[15] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%r12d,		%edx		/* dst += ... */
	mov	%ebx,		%r12d		/* (NEXT STEP) z' = %ebx */
	rol	$9,		%edx		/* dst <<< s */
	add	%eax,		%edx		/* dst += x */
	not	%r11d				/* not z */
	lea	0xd8a1e681(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r12d		/* x & z */
	and	%eax,		%r11d		/* y & (not z) */
	mov	4*4(%rsi),%r10d		/* (NEXT STEP) X[4] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%r12d,		%ecx		/* dst += ... */
	mov	%eax,		%r12d		/* (NEXT STEP) z' = %eax */
	rol	$14,		%ecx		/* dst <<< s */
	add	%edx,		%ecx		/* dst += x */
	not	%r11d				/* not z */
	lea	0xe7d3fbc8(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r12d		/* x & z */
	and	%edx,		%r11d		/* y & (not z) */
	mov	9*4(%rsi),%r10d		/* (NEXT STEP) X[9] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%r12d,		%ebx		/* dst += ... */
	mov	%edx,		%r12d		/* (NEXT STEP) z' = %edx */
	rol	$20,		%ebx		/* dst <<< s */
	add	%ecx,		%ebx		/* dst += x */
	not	%r11d				/* not z */
	lea	0x21e1cde6(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r12d		/* x & z */
	and	%ecx,		%r11d		/* y & (not z) */
	mov	14*4(%rsi),%r10d		/* (NEXT STEP) X[14] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%r12d,		%eax		/* dst += ... */
	mov	%ecx,		%r12d		/* (NEXT STEP) z' = %ecx */
	rol	$5,		%eax		/* dst <<< s */
	add	%ebx,		%eax		/* dst += x */
	not	%r11d				/* not z */
	lea	0xc33707d6(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r12d		/* x & z */
	and	%ebx,		%r11d		/* y & (not z) */
	mov	3*4(%rsi),%r10d		/* (NEXT STEP) X[3] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%r12d,		%edx		/* dst += ... */
	mov	%ebx,		%r12d		/* (NEXT STEP) z' = %ebx */
	rol	$9,		%edx		/* dst <<< s */
	add	%eax,		%edx		/* dst += x */
	not	%r11d				/* not z */
	lea	0xf4d50d87(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r12d		/* x & z */
	and	%eax,		%r11d		/* y & (not z) */
	mov	8*4(%rsi),%r10d		/* (NEXT STEP) X[8] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%r12d,		%ecx		/* dst += ... */
	mov	%eax,		%r12d		/* (NEXT STEP) z' = %eax */
	rol	$14,		%ecx		/* dst <<< s */
	add	%edx,		%ecx		/* dst += x */
	not	%r11d				/* not z */
	lea	0x455a14ed(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r12d		/* x & z */
	and	%edx,		%r11d		/* y & (not z) */
	mov	13*4(%rsi),%r10d		/* (NEXT STEP) X[13] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%r12d,		%ebx		/* dst += ... */
	mov	%edx,		%r12d		/* (NEXT STEP) z' = %edx */
	rol	$20,		%ebx		/* dst <<< s */
	add	%ecx,		%ebx		/* dst += x */
	not	%r11d				/* not z */
	lea	0xa9e3e905(%eax,%r10d),%eax		/* Const + dst + ... */
	and	%ebx,		%r12d		/* x & z */
	and	%ecx,		%r11d		/* y & (not z) */
	mov	2*4(%rsi),%r10d		/* (NEXT STEP) X[2] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ecx,		%r11d		/* (NEXT STEP) z' = %ecx */
	add	%r12d,		%eax		/* dst += ... */
	mov	%ecx,		%r12d		/* (NEXT STEP) z' = %ecx */
	rol	$5,		%eax		/* dst <<< s */
	add	%ebx,		%eax		/* dst += x */
	not	%r11d				/* not z */
	lea	0xfcefa3f8(%edx,%r10d),%edx		/* Const + dst + ... */
	and	%eax,		%r12d		/* x & z */
	and	%ebx,		%r11d		/* y & (not z) */
	mov	7*4(%rsi),%r10d		/* (NEXT STEP) X[7] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%ebx,		%r11d		/* (NEXT STEP) z' = %ebx */
	add	%r12d,		%edx		/* dst += ... */
	mov	%ebx,		%r12d		/* (NEXT STEP) z' = %ebx */
	rol	$9,		%edx		/* dst <<< s */
	add	%eax,		%edx		/* dst += x */
	not	%r11d				/* not z */
	lea	0x676f02d9(%ecx,%r10d),%ecx		/* Const + dst + ... */
	and	%edx,		%r12d		/* x & z */
	and	%eax,		%r11d		/* y & (not z) */
	mov	12*4(%rsi),%r10d		/* (NEXT STEP) X[12] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%eax,		%r11d		/* (NEXT STEP) z' = %eax */
	add	%r12d,		%ecx		/* dst += ... */
	mov	%eax,		%r12d		/* (NEXT STEP) z' = %eax */
	rol	$14,		%ecx		/* dst <<< s */
	add	%edx,		%ecx		/* dst += x */
	not	%r11d				/* not z */
	lea	0x8d2a4c8a(%ebx,%r10d),%ebx		/* Const + dst + ... */
	and	%ecx,		%r12d		/* x & z */
	and	%edx,		%r11d		/* y & (not z) */
	mov	0*4(%rsi),%r10d		/* (NEXT STEP) X[0] */
	or	%r11d,		%r12d		/* (y & (not z)) | (x & z) */
	mov	%edx,		%r11d		/* (NEXT STEP) z' = %edx */
	add	%r12d,		%ebx		/* dst += ... */
	mov	%edx,		%r12d		/* (NEXT STEP) z' = %edx */
	rol	$20,		%ebx		/* dst <<< s */
	add	%ecx,		%ebx		/* dst += x */
 mov	5*4(%rsi),	%r10d		/* (NEXT STEP) X[5] */
 mov	%ecx,		%r11d		/* (NEXT STEP) y' = %ecx */
	lea	0xfffa3942(%eax,%r10d),%eax		/* Const + dst + ... */
	mov	8*4(%rsi),%r10d		/* (NEXT STEP) X[8] */
	xor	%edx,		%r11d		/* z ^ ... */
	xor	%ebx,		%r11d		/* x ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	rol	$4,		%eax		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) y' = %ebx */
	add	%ebx,		%eax		/* dst += x */
	lea	0x8771f681(%edx,%r10d),%edx		/* Const + dst + ... */
	mov	11*4(%rsi),%r10d		/* (NEXT STEP) X[11] */
	xor	%ecx,		%r11d		/* z ^ ... */
	xor	%eax,		%r11d		/* x ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	rol	$11,		%edx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) y' = %eax */
	add	%eax,		%edx		/* dst += x */
	lea	0x6d9d6122(%ecx,%r10d),%ecx		/* Const + dst + ... */
	mov	14*4(%rsi),%r10d		/* (NEXT STEP) X[14] */
	xor	%ebx,		%r11d		/* z ^ ... */
	xor	%edx,		%r11d		/* x ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$16,		%ecx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) y' = %edx */
	add	%edx,		%ecx		/* dst += x */
	lea	0xfde5380c(%ebx,%r10d),%ebx		/* Const + dst + ... */
	mov	1*4(%rsi),%r10d		/* (NEXT STEP) X[1] */
	xor	%eax,		%r11d		/* z ^ ... */
	xor	%ecx,		%r11d		/* x ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$23,		%ebx		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) y' = %ecx */
	add	%ecx,		%ebx		/* dst += x */
	lea	0xa4beea44(%eax,%r10d),%eax		/* Const + dst + ... */
	mov	4*4(%rsi),%r10d		/* (NEXT STEP) X[4] */
	xor	%edx,		%r11d		/* z ^ ... */
	xor	%ebx,		%r11d		/* x ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	rol	$4,		%eax		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) y' = %ebx */
	add	%ebx,		%eax		/* dst += x */
	lea	0x4bdecfa9(%edx,%r10d),%edx		/* Const + dst + ... */
	mov	7*4(%rsi),%r10d		/* (NEXT STEP) X[7] */
	xor	%ecx,		%r11d		/* z ^ ... */
	xor	%eax,		%r11d		/* x ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	rol	$11,		%edx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) y' = %eax */
	add	%eax,		%edx		/* dst += x */
	lea	0xf6bb4b60(%ecx,%r10d),%ecx		/* Const + dst + ... */
	mov	10*4(%rsi),%r10d		/* (NEXT STEP) X[10] */
	xor	%ebx,		%r11d		/* z ^ ... */
	xor	%edx,		%r11d		/* x ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$16,		%ecx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) y' = %edx */
	add	%edx,		%ecx		/* dst += x */
	lea	0xbebfbc70(%ebx,%r10d),%ebx		/* Const + dst + ... */
	mov	13*4(%rsi),%r10d		/* (NEXT STEP) X[13] */
	xor	%eax,		%r11d		/* z ^ ... */
	xor	%ecx,		%r11d		/* x ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$23,		%ebx		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) y' = %ecx */
	add	%ecx,		%ebx		/* dst += x */
	lea	0x289b7ec6(%eax,%r10d),%eax		/* Const + dst + ... */
	mov	0*4(%rsi),%r10d		/* (NEXT STEP) X[0] */
	xor	%edx,		%r11d		/* z ^ ... */
	xor	%ebx,		%r11d		/* x ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	rol	$4,		%eax		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) y' = %ebx */
	add	%ebx,		%eax		/* dst += x */
	lea	0xeaa127fa(%edx,%r10d),%edx		/* Const + dst + ... */
	mov	3*4(%rsi),%r10d		/* (NEXT STEP) X[3] */
	xor	%ecx,		%r11d		/* z ^ ... */
	xor	%eax,		%r11d		/* x ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	rol	$11,		%edx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) y' = %eax */
	add	%eax,		%edx		/* dst += x */
	lea	0xd4ef3085(%ecx,%r10d),%ecx		/* Const + dst + ... */
	mov	6*4(%rsi),%r10d		/* (NEXT STEP) X[6] */
	xor	%ebx,		%r11d		/* z ^ ... */
	xor	%edx,		%r11d		/* x ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$16,		%ecx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) y' = %edx */
	add	%edx,		%ecx		/* dst += x */
	lea	0x4881d05(%ebx,%r10d),%ebx		/* Const + dst + ... */
	mov	9*4(%rsi),%r10d		/* (NEXT STEP) X[9] */
	xor	%eax,		%r11d		/* z ^ ... */
	xor	%ecx,		%r11d		/* x ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$23,		%ebx		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) y' = %ecx */
	add	%ecx,		%ebx		/* dst += x */
	lea	0xd9d4d039(%eax,%r10d),%eax		/* Const + dst + ... */
	mov	12*4(%rsi),%r10d		/* (NEXT STEP) X[12] */
	xor	%edx,		%r11d		/* z ^ ... */
	xor	%ebx,		%r11d		/* x ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	rol	$4,		%eax		/* dst <<< s */
	mov	%ebx,		%r11d		/* (NEXT STEP) y' = %ebx */
	add	%ebx,		%eax		/* dst += x */
	lea	0xe6db99e5(%edx,%r10d),%edx		/* Const + dst + ... */
	mov	15*4(%rsi),%r10d		/* (NEXT STEP) X[15] */
	xor	%ecx,		%r11d		/* z ^ ... */
	xor	%eax,		%r11d		/* x ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	rol	$11,		%edx		/* dst <<< s */
	mov	%eax,		%r11d		/* (NEXT STEP) y' = %eax */
	add	%eax,		%edx		/* dst += x */
	lea	0x1fa27cf8(%ecx,%r10d),%ecx		/* Const + dst + ... */
	mov	2*4(%rsi),%r10d		/* (NEXT STEP) X[2] */
	xor	%ebx,		%r11d		/* z ^ ... */
	xor	%edx,		%r11d		/* x ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	rol	$16,		%ecx		/* dst <<< s */
	mov	%edx,		%r11d		/* (NEXT STEP) y' = %edx */
	add	%edx,		%ecx		/* dst += x */
	lea	0xc4ac5665(%ebx,%r10d),%ebx		/* Const + dst + ... */
	mov	0*4(%rsi),%r10d		/* (NEXT STEP) X[0] */
	xor	%eax,		%r11d		/* z ^ ... */
	xor	%ecx,		%r11d		/* x ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	rol	$23,		%ebx		/* dst <<< s */
	mov	%ecx,		%r11d		/* (NEXT STEP) y' = %ecx */
	add	%ecx,		%ebx		/* dst += x */
 mov	0*4(%rsi),	%r10d		/* (NEXT STEP) X[0] */
 mov	$0xffffffff,	%r11d
 xor	%edx,		%r11d		/* (NEXT STEP) not z' = not %edx*/
	lea	0xf4292244(%eax,%r10d),%eax		/* Const + dst + ... */
	or	%ebx,		%r11d		/* x | ... */
	xor	%ecx,		%r11d		/* y ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	mov	7*4(%rsi),%r10d		/* (NEXT STEP) X[7] */
	mov	$0xffffffff,	%r11d
	rol	$6,		%eax		/* dst <<< s */
	xor	%ecx,		%r11d		/* (NEXT STEP) not z' = not %ecx */
	add	%ebx,		%eax		/* dst += x */
	lea	0x432aff97(%edx,%r10d),%edx		/* Const + dst + ... */
	or	%eax,		%r11d		/* x | ... */
	xor	%ebx,		%r11d		/* y ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	mov	14*4(%rsi),%r10d		/* (NEXT STEP) X[14] */
	mov	$0xffffffff,	%r11d
	rol	$10,		%edx		/* dst <<< s */
	xor	%ebx,		%r11d		/* (NEXT STEP) not z' = not %ebx */
	add	%eax,		%edx		/* dst += x */
	lea	0xab9423a7(%ecx,%r10d),%ecx		/* Const + dst + ... */
	or	%edx,		%r11d		/* x | ... */
	xor	%eax,		%r11d		/* y ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	mov	5*4(%rsi),%r10d		/* (NEXT STEP) X[5] */
	mov	$0xffffffff,	%r11d
	rol	$15,		%ecx		/* dst <<< s */
	xor	%eax,		%r11d		/* (NEXT STEP) not z' = not %eax */
	add	%edx,		%ecx		/* dst += x */
	lea	0xfc93a039(%ebx,%r10d),%ebx		/* Const + dst + ... */
	or	%ecx,		%r11d		/* x | ... */
	xor	%edx,		%r11d		/* y ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	mov	12*4(%rsi),%r10d		/* (NEXT STEP) X[12] */
	mov	$0xffffffff,	%r11d
	rol	$21,		%ebx		/* dst <<< s */
	xor	%edx,		%r11d		/* (NEXT STEP) not z' = not %edx */
	add	%ecx,		%ebx		/* dst += x */
	lea	0x655b59c3(%eax,%r10d),%eax		/* Const + dst + ... */
	or	%ebx,		%r11d		/* x | ... */
	xor	%ecx,		%r11d		/* y ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	mov	3*4(%rsi),%r10d		/* (NEXT STEP) X[3] */
	mov	$0xffffffff,	%r11d
	rol	$6,		%eax		/* dst <<< s */
	xor	%ecx,		%r11d		/* (NEXT STEP) not z' = not %ecx */
	add	%ebx,		%eax		/* dst += x */
	lea	0x8f0ccc92(%edx,%r10d),%edx		/* Const + dst + ... */
	or	%eax,		%r11d		/* x | ... */
	xor	%ebx,		%r11d		/* y ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	mov	10*4(%rsi),%r10d		/* (NEXT STEP) X[10] */
	mov	$0xffffffff,	%r11d
	rol	$10,		%edx		/* dst <<< s */
	xor	%ebx,		%r11d		/* (NEXT STEP) not z' = not %ebx */
	add	%eax,		%edx		/* dst += x */
	lea	0xffeff47d(%ecx,%r10d),%ecx		/* Const + dst + ... */
	or	%edx,		%r11d		/* x | ... */
	xor	%eax,		%r11d		/* y ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	mov	1*4(%rsi),%r10d		/* (NEXT STEP) X[1] */
	mov	$0xffffffff,	%r11d
	rol	$15,		%ecx		/* dst <<< s */
	xor	%eax,		%r11d		/* (NEXT STEP) not z' = not %eax */
	add	%edx,		%ecx		/* dst += x */
	lea	0x85845dd1(%ebx,%r10d),%ebx		/* Const + dst + ... */
	or	%ecx,		%r11d		/* x | ... */
	xor	%edx,		%r11d		/* y ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	mov	8*4(%rsi),%r10d		/* (NEXT STEP) X[8] */
	mov	$0xffffffff,	%r11d
	rol	$21,		%ebx		/* dst <<< s */
	xor	%edx,		%r11d		/* (NEXT STEP) not z' = not %edx */
	add	%ecx,		%ebx		/* dst += x */
	lea	0x6fa87e4f(%eax,%r10d),%eax		/* Const + dst + ... */
	or	%ebx,		%r11d		/* x | ... */
	xor	%ecx,		%r11d		/* y ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	mov	15*4(%rsi),%r10d		/* (NEXT STEP) X[15] */
	mov	$0xffffffff,	%r11d
	rol	$6,		%eax		/* dst <<< s */
	xor	%ecx,		%r11d		/* (NEXT STEP) not z' = not %ecx */
	add	%ebx,		%eax		/* dst += x */
	lea	0xfe2ce6e0(%edx,%r10d),%edx		/* Const + dst + ... */
	or	%eax,		%r11d		/* x | ... */
	xor	%ebx,		%r11d		/* y ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	mov	6*4(%rsi),%r10d		/* (NEXT STEP) X[6] */
	mov	$0xffffffff,	%r11d
	rol	$10,		%edx		/* dst <<< s */
	xor	%ebx,		%r11d		/* (NEXT STEP) not z' = not %ebx */
	add	%eax,		%edx		/* dst += x */
	lea	0xa3014314(%ecx,%r10d),%ecx		/* Const + dst + ... */
	or	%edx,		%r11d		/* x | ... */
	xor	%eax,		%r11d		/* y ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	mov	13*4(%rsi),%r10d		/* (NEXT STEP) X[13] */
	mov	$0xffffffff,	%r11d
	rol	$15,		%ecx		/* dst <<< s */
	xor	%eax,		%r11d		/* (NEXT STEP) not z' = not %eax */
	add	%edx,		%ecx		/* dst += x */
	lea	0x4e0811a1(%ebx,%r10d),%ebx		/* Const + dst + ... */
	or	%ecx,		%r11d		/* x | ... */
	xor	%edx,		%r11d		/* y ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	mov	4*4(%rsi),%r10d		/* (NEXT STEP) X[4] */
	mov	$0xffffffff,	%r11d
	rol	$21,		%ebx		/* dst <<< s */
	xor	%edx,		%r11d		/* (NEXT STEP) not z' = not %edx */
	add	%ecx,		%ebx		/* dst += x */
	lea	0xf7537e82(%eax,%r10d),%eax		/* Const + dst + ... */
	or	%ebx,		%r11d		/* x | ... */
	xor	%ecx,		%r11d		/* y ^ ... */
	add	%r11d,		%eax		/* dst += ... */
	mov	11*4(%rsi),%r10d		/* (NEXT STEP) X[11] */
	mov	$0xffffffff,	%r11d
	rol	$6,		%eax		/* dst <<< s */
	xor	%ecx,		%r11d		/* (NEXT STEP) not z' = not %ecx */
	add	%ebx,		%eax		/* dst += x */
	lea	0xbd3af235(%edx,%r10d),%edx		/* Const + dst + ... */
	or	%eax,		%r11d		/* x | ... */
	xor	%ebx,		%r11d		/* y ^ ... */
	add	%r11d,		%edx		/* dst += ... */
	mov	2*4(%rsi),%r10d		/* (NEXT STEP) X[2] */
	mov	$0xffffffff,	%r11d
	rol	$10,		%edx		/* dst <<< s */
	xor	%ebx,		%r11d		/* (NEXT STEP) not z' = not %ebx */
	add	%eax,		%edx		/* dst += x */
	lea	0x2ad7d2bb(%ecx,%r10d),%ecx		/* Const + dst + ... */
	or	%edx,		%r11d		/* x | ... */
	xor	%eax,		%r11d		/* y ^ ... */
	add	%r11d,		%ecx		/* dst += ... */
	mov	9*4(%rsi),%r10d		/* (NEXT STEP) X[9] */
	mov	$0xffffffff,	%r11d
	rol	$15,		%ecx		/* dst <<< s */
	xor	%eax,		%r11d		/* (NEXT STEP) not z' = not %eax */
	add	%edx,		%ecx		/* dst += x */
	lea	0xeb86d391(%ebx,%r10d),%ebx		/* Const + dst + ... */
	or	%ecx,		%r11d		/* x | ... */
	xor	%edx,		%r11d		/* y ^ ... */
	add	%r11d,		%ebx		/* dst += ... */
	mov	0*4(%rsi),%r10d		/* (NEXT STEP) X[0] */
	mov	$0xffffffff,	%r11d
	rol	$21,		%ebx		/* dst <<< s */
	xor	%edx,		%r11d		/* (NEXT STEP) not z' = not %edx */
	add	%ecx,		%ebx		/* dst += x */
	# add old values of A, B, C, D
	add	%r8d,	%eax
	add	%r9d,	%ebx
	add	%r14d,	%ecx
	add	%r15d,	%edx

	# loop control
	add	$64,		%rsi		# ptr += 64
	cmp	%rdi,		%rsi		# cmp end with ptr
	jb	2b				# jmp if ptr < end
	# END of loop over 16-word blocks

1:	pop	%rbp				# restore ctx
	mov	%eax,		0*4(%rbp)	# ctx->A = A
	mov	%ebx,		1*4(%rbp)	# ctx->B = B
	mov	%ecx,		2*4(%rbp)	# ctx->C = C
	mov	%edx,		3*4(%rbp)	# ctx->D = D

	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbx
	pop	%rbp
	ret
.L_md5_block_asm_host_order_end:
.size md5_block_asm_host_order,.L_md5_block_asm_host_order_end-md5_block_asm_host_order

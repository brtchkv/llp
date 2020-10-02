global	_start

section	.data

section	.text


string_length:
	xor rax,rax
	.loop:
		cmp byte [rdi+rax], 0 ;проверка строки на наличие ненулевых символов
		je .end	
		inc rax ;инкрементирование, если ненулевой символ
		jmp .loop
	.end:		
		ret

print_string:
	call string_length 
	mov	rdx,rax ;задаем размер выводимой строки
	mov	rsi,rdi
	mov	rax,1 ;системный вывод
	mov	rdi,1 ;дескриптор stdout
	syscall
	xor	rax,rax
	ret
	
print_newline:
	mov rax,1 ;системный вызов
   	mov rdi,1 ;дескриптор вывода
   	mov rsi,10 ;записываем символ новой строки
   	mov rdx,1 ;размер выводимой строки
   	syscall
   	xor	rax,rax
	ret
	
string_copy:
	xor	r9,r9
	mov	rcx,rdx ;записываем строку в ячейку для циклов
	call	string_length 
	cmp	rax,rdx ;проверяем, что размер буфера не меньше размера строки
	jg	.reset
	.loop:
		mov	r9b,byte[rdi+rcx] ;записываем байт строки в байтовую строку
		mov	byte[rsi+rcx],	r9b
		dec	rcx ;декрементируем размер строки
		cmp	rcx,-1 
		je	.end
		jne	.loop
	.reset:
		mov	rax,0
		jmp	.end
	.end:
		ret

print_char:
	push	rdi
	mov	rsi,rsp
	mov	rax,1
	mov	rdx,1
	mov	rdi,1
	syscall
	pop	rdi
	xor	rax,rax
	ret

print_uint:
	push	rbp
	mov 	rbp,rsp
	mov 	rax,rdi
	mov 	rdi,10
	sub 	rsp,0x64
	dec 	rbp
	.loop:
		xor 	rdx,rdx
		div 	rdi 
		add 	rdx,'0' 
		dec 	rbp	
		mov 	byte[rbp],dl
		test 	rax,rax
		jnz 	.loop
	mov rdi,rbp
	call print_string
	add rsp,0x64
	pop rbp
	xor rax,rax
	ret

print_int:
	test	rdi,rdi
	jns	.print	;print number if positive, else -- move forward
	push	rdi	;saving a number to print a sign
	mov	rdi,'-' 
	call	print_char
	pop	rdi
	neg	rdi	;make it a positive to print a module
	.print:
		call	print_uint
	xor	rax,rax
	ret
	
read_char:
	xor	rax,rax
	push	rax
	mov	rdx,1
	mov	rsi,rsp
	mov	rdi,0
	syscall
	pop	rax
	ret

read_word:
	xor rax, rax
	push r10
	push r9
	xor r10, r10
	mov r9, rsi
	.loop:
		push rdi
		call read_char
		pop rdi
		cmp al, 9
		jz .loop
		cmp al, 32
		jz .loop
	.loop2:
		mov byte[rdi + r10], al
		cmp al, 0
		jz .end
		cmp al, 32
		jz .end
		cmp al, 9
		jz .end
		inc r10
		cmp r10, r9
		je .error
		push rdi
		call read_char
		pop rdi
		jmp .loop2
	.end:
		mov rax, 0
		mov rdx, r10
		mov byte [rdi + r10], 0
		mov rax, rdi
		pop r9
		pop r10
		ret
	.error:
		xor rax, rax
		pop r9
		pop r10
		ret
		

parse_uint:
	xor	rax,rax
	push	rbx
	call	string_length
	mov	rcx,rax
	xor	rax,rax
	xor	rdx,rdx
	.loop:
		xor	rbx,rbx
		mov	bl,byte[rdi+rdx]
		cmp	bl,'0'
		jb	.end
		cmp	bl,'9'
		ja	.end
		sub	bl,'0'
		imul	rax,10
		add	rax,rbx
		inc	rdx
		dec	rcx
		jnz	.loop
	.end:
		pop	rbx
		ret
parse_int:
	xor	rax,rax
	push	r9
	call	string_length
	mov	rcx,rax
	xor	rax,rax
	xor	rdx,rdx
	mov	rsi,rdi
	xor	r9b,r9b
	mov	r9b,byte[rsi+rdx]
	cmp	r9b,'-'
	je	.signed
	pop	r9
	jmp	parse_uint
	.signed:
		inc	rdi
		call	parse_uint
		neg	rax
		inc	rdx
		pop	r9
		ret

string_equals:
	xor rax, rax
	push rbx
	call string_length
	mov rcx, rax
	.loop:
		mov dl, byte[rdi+rcx]
		mov bl, byte[rsi+rcx]
		cmp dl, bl
		jnz .not_equal
		cmp bl, 0
		jz .equal
		dec rcx
		jnz .loop
	.equal:
		pop rbx
		mov rax, 1
		ret
	.not_equal:
		pop rbx
		mov rax, 0
		ret
	
	
	
	
	
	
	
	

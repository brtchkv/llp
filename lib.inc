section .text

string_length:
    mov rax, -1			; string length
	.loop:
		inc rax
		cmp byte[rdi+rax], 0
		jnz .loop
    ret

print_string:
    call string_length
	mov rsi, rdi		; pointer to string
	mov rdx, rax		; string length
	mov rax, 1			; sys_write
	mov rdi, 1			; stdout
	syscall
    ret

print_newline:
	mov rdi, 10

print_char:				;
    mov rax, 1			; sys_write
	push rdi			; pushing rdi(char) to memory for pointer
	mov rsi, rsp		; pointer to char
	mov rdi, 1			; stdout
	mov rdx, 1			; length=1 
	syscall
	pop rdi				; to fix rsp
    ret

print_int:
	cmp rdi, 0					; check if < 0
	jns print_uint
    xor rdi, -1					; make uint
	inc rdi						;
	push rdi
	mov rdi, '-'				; print '-'
	call print_char				;
	pop rdi

print_uint:
	mov rax, rdi
    mov r8, 10					; for div
	times 11 push word 0		; push 22 bytes (for 20 symb + '0x00')
	mov r9, rsp					; buffer for rsp
	add rsp, 21					; to last stack mem cell
	mov byte [rsp], 0			; end of the string
	.loop:
		xor rdx, rdx			; RDX:RAX when div
		dec rsp
		div r8
		add rdx, '0'			; RDX has remainder
		mov byte [rsp], dl		; byte of RDX
		test rax, rax
		jnz .loop
	mov rdi, rsp				; pointer to string
	call print_string
	mov rsp, r9					
	add rsp, 22					; fix rsp
    ret

string_equals:
	push rdi				; saving rdi and rsi
	push rsi				; 
	call string_length
	pop rsi
	pop rdi
	mov r8, rax				; r8 - length of 1 string
	mov rax, rdi
	mov rdi, rsi
	mov rsi, rax
	push rdi				; saving rdi, rsi and r8
	push rsi				;
	push r8					;
	call string_length
	pop r8
	pop rsi
	pop rdi
	mov rcx, rax			; rcx - length of 2 string
	xor rax, rax
	cmp r8, rcx				; if r8!=rcx |=> exit
	jnz .exit
	test rcx, rcx			; if r8=rcx=0 |=> return 1
	jz .nullstr
	.loop:
		dec rcx
		mov r9b, byte[rdi+rcx]
		cmp r9b, byte[rsi+rcx]
		jnz .exit			; if != |=> return 0
		test rcx, rcx
		jnz .loop
	.nullstr:
	inc rax
	.exit:
    ret

read_char:
    xor rax, rax
	push rax
	mov rdi, 0				; sys_read
	mov rsi, rsp			; pointer
	mov rdx, 1				; size
	syscall
	mov rax, [rsp]			
	add rsp, 8				; fix rsp
    ret 

section .data
word_buffer: times 256 db 0

section .text

read_word:
	xor rax, rax
	push r10			; save callee registers
	push r9 			
	xor r10, r10	
	mov r9, rsi			
	.loop:
		push rdi		
		call read_char		
		pop rdi
		cmp al, 9
		jz .loop
		cmp al, 32		; skip until first letter
		jz .loop
	.loop2:				; read until 'space' or lower
		mov byte[rdi + r10], al
		cmp al, 0		; check for null-pointer
		jz .end
		cmp al, 32		; spaces
		jz .end
		cmp al, 9		; tabs
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

; rdi points to a string
; returns rax: number, rdx : length
parse_uint:
	xor rdx, rdx			; RDX:RAX
	xor rax, rax			; RDX:RAX
	xor rcx, rcx			; Counter
	xor r11, r11			; Buffer
	mov r8, '0'
	mov r9, '9'
	mov r10, 10				; for mul
	.loop:
		mov r11b, byte[rdi+rcx]
		cmp r11b, r8b		; (char)-'0'
		js .break
		cmp r9b, r11b		; '9'-(char)
		js .break
		sub r11, r8			; char to uint
		mul r10				;
		add rax, r11		;
		inc rcx				; length+1
		test rax, rax		; if zero |=> break
		jnz .loop
	.break:
	mov rdx, rcx
    ret

; rdi points to a string
; returns rax: number, rdx : length
parse_int:
	push r12
	xor r12, r12			; buffer for '-' or...
	cmp byte[rdi], '-'
	jnz .plus				; if not '-' go parse_uint
	inc rdi					; rdi+1 for uint
	mov r12, -1
	.plus:
	call parse_uint			; same as uint if plus
	xor rax, r12
	sub rax, r12
	sub rdx, r12
	pop r12
	ret 

string_copy:
	.loop:
		xor rax, rax
		mov al, byte[rdi]
		mov byte[rsi], al
		inc rdi
		inc rsi
		test rax, rax
		jnz .loop
	ret

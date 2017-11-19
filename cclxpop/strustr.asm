; char *strustr(char * s1, char * s2)
; performs an non case sensitivtive earch for s2 in s1, returning a pointer
; to the match if found, or zero if not.
; Turbo C/ANSI Version, assumes Small model (64K code, separate 64K data)
;
; strustr.asm
;

; -------------------- Constants and Macros --------------------
; C function call stack layout -- SI, DI, BP, and SP must be preserved
old_bp      equ 0
ret_addr    equ 2
arg1        equ 4
arg2        equ 6
arg3        equ 8
arg4        equ 10
arg5        equ 12
arg6        equ 14
arg7        equ 16
arg8        equ 18

; -------------------- Data Segment --------------------

_DATA	segment	word public 'DATA'	; initialized data
_DATA	ends

_BSS	segment	word public 'BSS'	; uninitialized data
_BSS	ends

DGROUP	group	_DATA, _BSS
	assume	ds:DGROUP

; -------------------- Code Segment --------------------

_TEXT	segment	byte public 'CODE'
	assume	cs:_TEXT

	public  _strustr

; ------------------------------------------------------------
; string comparison function
_strustr proc near


	push	BP
	mov	BP,SP
	push	DI
	push	SI
	push	ES

	cld
	mov	AX,DS
	mov	ES,AX
	mov	SI,[BP] + arg1		; S1
	mov	CL,('a'-'A')		;Delta between upper and lower
	mov	CH,'z'
	mov	DI,[BP] + arg2		; S2

SetupBX:
	mov	BX,0
GetSrch:
	mov	ah,[DI]		;load first search char

Start:
	lodsb          		;get next data byte from source string
	or	al,al		;test for string end
	jz	nomatch		;end of source string
	sub	AL,AH		;Compare with current DI byte
	jz	curmatch	;OK - contine with search
	cmp	AL,CL		;check for 'a' to 'A' offset
	jnz	nomatchyet
	mov	al,[SI-1]	; check for letter
	cmp	al,ch
	jg	nomatchyet
	cmp	al,'a'
	jb	nomatchyet

curmatch:			;SI already inc, inc DI and test for end
	inc	BX
	mov	ah,[DI+BX]
	or	ah,ah
	jnz	Start

;match found - now figure out where
	mov	AX,SI
	sub	AX,BX		;start of destination string
	jmp	exit

Nomatchyet:			;not matched, not end of source string
	or	bx,bx
	jz	Start		;short circuit if no offset done
	sub	SI,BX		;reset to beginning of string
	jmp     SetupBX		;reset to beginning of search string

Nomatch:
	xor	ax,ax		;return value is zero

exit:
	pop	ES
	pop	SI
	pop	DI
	pop	BP
	ret

_strustr endp

_TEXT   ends

		end

; end of strustr.ASM

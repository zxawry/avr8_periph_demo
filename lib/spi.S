;---------------------------------------------------------------------------;
; USI control functions
;---------------------------------------------------------------------------;

.nolist
#include <avr/io.h>	// Include device specific definitions.
.list


;---------------------------------------------------------------------------;
; Initialize SPI
;
; void init_spi (void);

.global init_spi
.func init_spi
init_spi:
	in	r24, _SFR_IO_ADDR(DDRB)
	ori	r24, 0b00101100
	out	_SFR_IO_ADDR(DDRB), r24
	ldi	r24, 0b01010000
	out	_SFR_IO_ADDR(SPCR), r24
	ret
.endfunc


;---------------------------------------------------------------------------;
; Delay 100 microseconds
;
; void dly_100us (void);

.global dly_100us
.func dly_100us
dly_100us:
	;ldi	r24, lo8(F_CPU / 100000)	/* Loop counter */
	ldi	r24, lo8(111)	/* Loop counter */
	;ldi	r25, hi8(F_CPU / 100000)
	ldi	r25, hi8(111)
1:	sbiw	r30, 1		/* 10 clocks a loop */
	sbiw	r30, 1
	sbiw	r30, 1
	sbiw	r24, 1
	brne	1b
	ret
.endfunc


;---------------------------------------------------------------------------;
; Receive a byte (28 clks)
;
; BYTE rcv_spi (void);

.global rcv_spi
.func rcv_spi
rcv_spi:
	ldi	r24, 0xFF	; Send 0xFF to receive data
	; Goto next function
.endfunc


;---------------------------------------------------------------------------;
; Transmit a byte (27 clks)
;
; void xmit_spi (BYTE);

.global xmit_spi
.func xmit_spi
xmit_spi:
	out	_SFR_IO_ADDR(SPDR), r24		; Put a byte to be transmit

1:	in	r25, _SFR_IO_ADDR(SPSR)
	sbrs	r25, SPIF
	rjmp	1b

	in	r24, _SFR_IO_ADDR(SPDR)		; Get received byte
	ret
.endfunc



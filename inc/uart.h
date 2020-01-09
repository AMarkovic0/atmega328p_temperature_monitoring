/*
 *************************************************************************
Basic UART communication library for ATMEGA328p
Defined baud rate at 9600 for 1.6MHz crystal oscillator

Source file: uart.c

Compiler: avr-gcc
Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Made by: Aleksa Markovic
Licence: MIT
 *************************************************************************
 */

#ifndef _UART_H_
#define _UART_H_

#include<stdio.h>
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

// Ring buffer size
#define UART_RBUF_SIZE 32

//UART0 registers
#define UART0_CON_STAT_REGA	UCSR0A
#define UART0_CON_STAT_REGB	UCSR0B
#define UART0_CON_STAT_REGC	UCSR0C

// UART status macros
#define UART_RCV_COMPLETE	0x80
#define UART_TRANS_COMPLETE	0x40
#define UART_DATREG_EMPTY	0x20
#define UART_FRAME_ERROR	0x10
#define UART_DATA_OVERR		0x08
#define UART_PARITY_ERR		0x04
#define UART_DOUBLE_TR		0x02
#define UART_MULTIPROCESSOR_COM 0x01

//UART control macros
#define UART_RX_COMPLETE_INT_EN	0x80
#define UART_TX_COMPLETE_INT_EN	0x40
#define UART_DREG_EMPTY_INT_EN	0x20
#define UART_REC_EN		0x10
#define UART_TRANS_EN		0x08
#define UART_RECV_DATA8BIT	0x04
#define UART_TRANS_DATA8BIT	0x02
#define UART_PARITY_EN		0x01

// UART control enummeration types:
typedef enum uart_config_t
{
	ENABLE ,
	DISABLE ,
	ASYNC ,
	SYNC ,
	MASTER_SPI ,
	EVEN ,
	ODD ,
	STOP1 ,
	STOP2 ,
	RISING_EDGE ,
	FALLING_EDGE ,
}uart_config_t;

typedef struct
{
	uart_config_t RX_COMPLETE_INT;
	uart_config_t TX_COMPLETE_INT;
	uart_config_t DATA_REG_EMPTY_INT;
	uart_config_t RECEIVE;
	uart_config_t TRANSMIT;
	uart_config_t OPERATION_MODE;
	uart_config_t PARITY_MODE;
	uart_config_t STOP_BIT;
	uart_config_t CLK_POLARITY;
	uart_config_t DOUBLE_SPEED;
	uint32_t BAUD;
}_UART_CONFIG;

typedef struct
{
	uint8_t buffer[ UART_RBUF_SIZE ];
	volatile uint8_t* head;
	volatile uint8_t* tail;
}_RING_BUFFER;

void UART0_init( _UART_CONFIG uart );
void UART0_set_operation_mode( _UART_CONFIG uart );
void UART0_set_parity_mode( _UART_CONFIG uart );
void UART0_set_stop_bit_mode( _UART_CONFIG uart );
void UART0_set_baud( _UART_CONFIG uart );
uint8_t UART0_receive_byte( uint8_t* r_data );
uint8_t UART0_send_byte( uint8_t w_data );
uint8_t UART0_read( uint8_t* r_data , uint8_t size );

extern volatile _RING_BUFFER ring_buf;
extern volatile uint8_t RCV_FLAG;

#endif //_UART_H_

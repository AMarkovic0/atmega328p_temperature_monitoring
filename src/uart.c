#include "inc/uart.h"

volatile _RING_BUFFER ring_buf = { { 0 } , NULL , NULL };
volatile uint8_t RCV_FLAG = 0;

void UART0_init( _UART_CONFIG uart )
{
        if( ENABLE == uart.RX_COMPLETE_INT )
        {
                UART0_CON_STAT_REGB |= UART_RX_COMPLETE_INT_EN;
        }
        else if( DISABLE == uart.RX_COMPLETE_INT )
        {
                UART0_CON_STAT_REGB &= ~( UART_RX_COMPLETE_INT_EN );
        }
        if( ENABLE == uart.TX_COMPLETE_INT )
        {
                UART0_CON_STAT_REGB |= UART_TX_COMPLETE_INT_EN;
        }
        else if( DISABLE == uart.TX_COMPLETE_INT )
        {
                UART0_CON_STAT_REGB &= ~( UART_TX_COMPLETE_INT_EN );
        }
        if( ENABLE == uart.DATA_REG_EMPTY_INT )
        {
                UART0_CON_STAT_REGB |= UART_DREG_EMPTY_INT_EN;
        }
        else if( DISABLE == uart.DATA_REG_EMPTY_INT )
        {
                UART0_CON_STAT_REGB &= ~( UART_DREG_EMPTY_INT_EN );
        }
        if( ENABLE == uart.RECEIVE )
        {
                UART0_CON_STAT_REGB |= UART_REC_EN;
        }
        else if( DISABLE == uart.RECEIVE )
        {
                UART0_CON_STAT_REGB &= ~( UART_REC_EN );
        }
        if( ENABLE == uart.TRANSMIT )
        {
                UART0_CON_STAT_REGB |= UART_TRANS_EN;
        }
        else if( DISABLE == uart.TRANSMIT )
        {
                UART0_CON_STAT_REGB &= ~( UART_TRANS_EN );
        }
	if( ENABLE == uart.DOUBLE_SPEED )
	{
		UART0_CON_STAT_REGA |= UART_DOUBLE_TR;
	}
	else
	{
		UART0_CON_STAT_REGA &= ~( UART_DOUBLE_TR );
	}
        UART0_set_operation_mode( uart );
        UART0_set_parity_mode( uart );
        UART0_set_stop_bit_mode( uart );
        UART0_set_baud( uart );

        ring_buf.head = ring_buf.buffer;
        ring_buf.tail = ring_buf.buffer;

        return;
}

void UART0_set_operation_mode( _UART_CONFIG uart )
{
        if( ASYNC == uart.OPERATION_MODE )
        {
                UART0_CON_STAT_REGC &= ~( 1 << 7 );
                UART0_CON_STAT_REGC &= ~( 1 << 6 );
        }
        else if( SYNC == uart.OPERATION_MODE )
        {
                UART0_CON_STAT_REGC &= ~( 1 << 7 );
                UART0_CON_STAT_REGC |= ( 1 << 6 );
        }
        else if( MASTER_SPI == uart.OPERATION_MODE )
        {
                UART0_CON_STAT_REGC |= ( 1 << 7 );
                UART0_CON_STAT_REGC |= ( 1 << 6 );
        }

        return;
}

void UART0_set_parity_mode( _UART_CONFIG uart )
{
        if( EVEN == uart.PARITY_MODE )
        {
                UART0_CON_STAT_REGC |= ( 1 << 5 );
                UART0_CON_STAT_REGC &= ~( 1 << 4 );

        }
        else if( ODD == uart.PARITY_MODE )
        {
                UART0_CON_STAT_REGC |= ( 1 << 5 );
                UART0_CON_STAT_REGC |= ( 1 << 4 );
        }
        else if( DISABLE == uart.PARITY_MODE )
        {
                UART0_CON_STAT_REGC &= ~( 1 << 5 );
                UART0_CON_STAT_REGC &= ~( 1 << 4 );
        }

        return;
}

void UART0_set_stop_bit_mode( _UART_CONFIG uart )
{
        if( STOP1 == uart.STOP_BIT )
        {
                UART0_CON_STAT_REGC &= ~( 1 << 3 );
        }
        else if( STOP2 == uart.STOP_BIT )
        {
                UART0_CON_STAT_REGC |= ( 1 << 3 );
        }

        return;
}

void UART0_set_baud( _UART_CONFIG uart )
{
        UART0_CON_STAT_REGB |= UART_TRANS_EN;
	uint32_t ubrr = 0;
	if( 0 == ( UART0_CON_STAT_REGA & UART_DOUBLE_TR ) )
	{
        	ubrr = ( ( ( F_CPU / ( uart.BAUD * 16UL ) ) ) - 1 );
	}
	else
	{
		ubrr = ( ( ( F_CPU / ( uart.BAUD * 8UL ) ) ) - 1 );
	}
	UBRR0H = ( ubrr >> 8 );
        UBRR0L = ubrr;
        UART0_CON_STAT_REGC = 0x06;

        return;
}

uint8_t UART0_receive_byte( uint8_t* r_data )
{

        while( !( UART0_CON_STAT_REGA & UART_DATREG_EMPTY ) );
        *r_data = UDR0;

        return 1;
}

uint8_t UART0_send_byte( uint8_t w_data )
{
        while( !( UART0_CON_STAT_REGA & UART_DATREG_EMPTY ) );
        UDR0 = w_data;

        return 1;
}

uint8_t UART0_read( uint8_t* r_data , uint8_t size )
{
        if( RCV_FLAG )
        {
                for( int j = 0 ; j < size ; j++ )
                {
                        *( r_data+j ) = *( ring_buf.head );
                        *( ring_buf.head ) = 0;

                        if( ring_buf.head == ring_buf.tail )
                                break;

                        if( ring_buf.head == &( ring_buf.buffer[ UART_RBUF_SIZE-1 ] ) )
                        {
                                ring_buf.head = ring_buf.buffer;
                        }
                        else
                        {
                                ring_buf.head++;
                        }
                }
                RCV_FLAG = 0;
        }
        return 1;
}


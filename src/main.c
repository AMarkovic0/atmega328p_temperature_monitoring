#define F_CPU 16000000UL

#include<string.h>
#include<util/delay.h>

#include"inc/uart.h"

#define TEMP_SENS_ADC_PORT 	8
#define TS_GAIN			1.22f
#define TS_OFFSET		324.31f
#define FILTER_ARR_SIZE		10
#define SIZE_OF_FLOAT		4

#define CONTROL_LED_DDR		DDRB
#define CONTROL_LED_PORT	PORTB
#define CONTROL_LED_PIN		PB5

#define CMD_SIZE 		6
#define TOGGLE_LED_CMD 		"AT+LED"
#define READ_TEMPERATURE_CMD 	"AT+TER"
#define SLE_CMD			"AT+SLE"

typedef union ts_value
{
	float ts_val;
	uint8_t ts_bytes[ SIZE_OF_FLOAT ];
}ts_val_u;

void TS_init( ts_val_u* value );
uint8_t ADC_start_conversion( void );
uint16_t ADC_read( void );
void read_temperature( ts_val_u* value );

int main( void )
{
	uint8_t data[ 10 ] = { 0 }; 	
	ts_val_u temperature;

	_UART_CONFIG uart0 =
       	{
       		ENABLE,
        	DISABLE,
        	DISABLE,
        	ENABLE,
        	ENABLE,
        	SYNC,
        	DISABLE,
        	STOP1,
        	RISING_EDGE,
		DISABLE,
        	9600
        };

	CONTROL_LED_DDR |= ( 1 << CONTROL_LED_PIN );
	UART0_init( uart0 );
	TS_init( &temperature );
	sei();

	while( 1 )
	{
		UART0_read( data , CMD_SIZE );
		if( 0 == strcmp( data , TOGGLE_LED_CMD ) )
		{
			CONTROL_LED_PORT ^= ( 1 << CONTROL_LED_PIN );
			memset( data , 0 , 10 );
			continue;
		}
		if( 0 == strcmp( data , READ_TEMPERATURE_CMD ) )
		{	
			read_temperature( &temperature );
			UART0_send_byte( temperature.ts_bytes[ 0 ] );
			UART0_send_byte( temperature.ts_bytes[ 1 ] );
			UART0_send_byte( temperature.ts_bytes[ 2 ] );
			UART0_send_byte( temperature.ts_bytes[ 3 ] );
			memset( data , 0 , 10 );
			continue;
		}
		if( 0 == strcmp( data , SLE_CMD ) )
		{
			continue;
		}
		_delay_ms( 100 );
	}

	return 0;
}

ISR( USART_RX_vect )
{
        static volatile uint8_t r_data = 0;

        UART0_receive_byte( &r_data );
        *( ring_buf.tail ) = r_data;

        if( ring_buf.tail == &( ring_buf.buffer[ UART_RBUF_SIZE-1 ] ) )
        {
                ring_buf.tail = ring_buf.buffer;
        }
        else
        {
                ring_buf.tail++;
        }
        if( ring_buf.tail == ring_buf.head )
                ring_buf.head++;

        RCV_FLAG++;

        return;
}

void TS_init( ts_val_u* value )
{
	ADCSRA |= ((1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0));     
	ADMUX |= (1 << REFS0) | (1 << REFS1) ;
	ADCSRB &= ~((1 << ADTS2)|(1 << ADTS1)|(1 << ADTS0)); 
	ADCSRA |= (1 << ADEN);                             
	ADCSRA |= (1 << ADSC);
	_delay_ms( 100 );                                     
	ADMUX = (ADMUX & 0xE0) | (TEMP_SENS_ADC_PORT & 0x1F);

	(*value).ts_val = 0.0f;

	return;
}

uint8_t ADC_start_conversion( void )
{
	ADCSRA |= (1 << ADSC);
	for( uint16_t j = 0 ; ( j < 65000 ) && ( ADCSRA & (1 << ADSC) ) ; j++ )
	{
		CONTROL_LED_PORT ^= ( 1 << CONTROL_LED_PIN );
		_delay_ms( 1 );
	}

	if( ( ADCSRA & (1 << ADSC)) )
	{
		UART0_send_byte( 'E' );
		return 0;
	}
	else
	{	
		return 1;
	}
}

uint16_t ADC_read( void )
{
	if( ADC_start_conversion( ) )
		return ADC;
	else
		return 0;
	
}

void read_temperature( ts_val_u* value )
{
	(*value).ts_val = 0.0;
	for( uint8_t j = 0 ; j < FILTER_ARR_SIZE ; j++ )
		(*value).ts_val += ( ADC_read( ) - TS_OFFSET ) / TS_GAIN;
	(*value).ts_val = (*value).ts_val / (float)FILTER_ARR_SIZE;
	return;
}

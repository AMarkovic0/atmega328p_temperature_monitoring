#define F_CPU 16000000UL

#include <avr/io.h>
#include<util/delay.h>
#include<stdint.h>

#define UBRR 103  //For baud rate 9600, crystal oscillator 1.6 Mhz

void initTimer(void);
void initADC(void);
void conversationADC(void);

void readPC(void);
void writePC(void);
void PCsend(char data);
char PCrecieve(void);

char dataS = 'R';
char dataR = '0';

int i = 0;

int main(void)
{
    initTimer();
    initADC();
    readPC();

    while(1)
    {
        if('0' == dataR)
          dataR = PCrecieve();

        if(dataR == 65)
        {
          _delay_ms(100);

          writePC();
          PCsend(ADC);
          dataR = '0';
          readPC();
        }
    }

    return 0;
}

void initTimer(void)
{
    TCCR1B |= 1 << CS12 | 1 << WGM12;
    TIMSK1 |= 1 << OCIE1A;
    OCR1A = 62500;
}

void initADC(void)
{
    ADCSRA |= ((1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0));     //Prescaler at 128 so we have an 125Khz clock source
    ADMUX |= (1 << REFS0);
    ADMUX &= ~(1 << REFS1);                                 //Avcc(+5v) as voltage reference
    ADCSRB &= ~((1 << ADTS2)|(1 << ADTS1)|(1 << ADTS0));    //ADC in free-running mode
    ADCSRA |= (1 << ADATE);                                 //Signal source, in this case is the free-running
    ADCSRA |= (1 << ADEN);                                  //Power up the ADC
    ADCSRA |= (1 << ADSC);
    _delay_ms(1);                                           //Start converting
}

void readPC(void)
{
    //unsigned int ubrr = BAUD_RATE_9600_BPS;

  UBRR0H = (UBRR >> 8);   // Shift the 16bit value ubrr 8 times to the right and transfer the upper 8 bits to UBBR0H register.
  UBRR0L = (UBRR);        // Copy the 16 bit value ubrr to the 8 bit UBBR0L register,Upper 8 bits are truncated while lower 8 bits are copied
  UCSR0B = (1 << RXEN0);    // Enable receiver
  UCSR0C = 0x06;          // Set frame format: 8data, 1stop bit
}

void writePC(void)
{
    //unsigned int ubrr = BAUD_RATE_9600_BPS;

  UBRR0H = (UBRR >> 8);   // Shift the 16bit value ubrr 8 times to the right and transfer the upper 8 bits to UBBR0H register.
  UBRR0L = (UBRR);        // Copy the 16 bit value ubrr to the 8 bit UBBR0L register,Upper 8 bits are truncated while lower 8 bits are copied
  UCSR0C = 0x06;          // Set frame format: 8data, 1stop bit
  UCSR0B = (1 << TXEN0);    // Enable  transmitter
}

void conversationADC(void)
{
    ADCSRA |= (1 << ADSC);
}

void PCsend(char data)
{
    while (!( UCSR0A & (1<<UDRE0)));    // Wait for empty transmit buffer
    UDR0 = data;                     // Put data into buffer, sends the data
    _delay_ms(100);
    while (!( UCSR0A & (1<<UDRE0)));
    UDR0 = '\n';
    while (!( UCSR0A & (1<<UDRE0)));
    UDR0 = '\r';
    _delay_ms(100);
}

char PCrecieve(void)
{
    while (!(UCSR0A & (1 << RXC0)));

    return UDR0;
}

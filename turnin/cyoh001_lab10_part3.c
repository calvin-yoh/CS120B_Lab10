/*	Author:
 *  	Partner(s) Name:
 *   	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn()
{
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}
void TimerOff()
{
	TCCR1B = 0x00;
}
void TimerISR()
{
	TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect)
{
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0)
	{
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) TCCR3B &= 0x08;
		else TCCR3B |= 0x03;
		if (frequency < 0.954) OCR3A = 0xFFFF;
		else if (frequency > 31250) OCR3A = 0x0000;
		else OCR3A = (short)(8000000 / (128 * frequency)) - 1;
		TCNT3 = 0;
		current_frequency = frequency;
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States { Start, WAIT, PLAY, DOWN, PAUSE, WAIT_2 }state;
double notes[22] = { 830.61, 622.25, 659.25, 739.99, 987.77, 880.00, 830.61, 554.37, 659.25, 739.99, 987.77, 880.00, 830.61, 493.88, 622.25, 659.25, 739.99, 830.61, 0, 830.61, 739.99, 554.37};
char time[22] = { 12, 12, 8, 12, 12, 8, 12, 12, 8, 12, 12, 8, 12, 12, 8, 12, 12, 8, 1, 12, 12, 23 };
unsigned char i = 0x00;
unsigned char k = 0x00;
unsigned char r = 0x00;

void Tick()
{
	switch (state) //transitions
	{
	case Start:
	{
		state = WAIT;
		break;
	}
	case WAIT:
	{
		if ((~PINA & 0x01) == 0x01)
		{
			state = PLAY; break;
		}
		else
		{
			state = WAIT; break;
		}
	}

	case PLAY:
	{
		if (i <= time[k])
		{
			state = PLAY; break;
		}
		else if (i > time[k])
		{
			++r;
			state = DOWN;
			break;
		}
	}
	case DOWN:
	{
		if (r < 24)
		{
			++k;
			state = PLAY;
			break;
		}
		else if (r >= 20)
		{
			state = PAUSE; break;
		}
	}

	case PAUSE:
	{
		if ((~PINA & 0x01) == 0x01)
		{
			state = WAIT_2; break;
		}
		else
		{
			state = PAUSE; break;
		}
	}
	case WAIT_2:
	{
		if ((~PINA & 0x01) == 0x01)
		{
			state = WAIT_2;
			break;
		}
		else
		{
			state = WAIT;
			break;
		}
	}
	default:
		break;
	}
	switch (state) //state actions
	{
	case Start:
		break;

	case WAIT:
	{
		i = 0;
		k = 0;
		r = 0;
		break;
	}

	case PLAY:
	{
		set_PWM(notes[k]);
		++i;
		break;
	}

	case DOWN:
	{
		set_PWM(0); break;
	}

	case PAUSE:
		break;
	default:
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	PWM_on();

	TimerSet(62);
	TimerOn();
	state = Start;
	while (1)
	{
		Tick();
		while (!TimerFlag);
		TimerFlag = 0;
	}
}



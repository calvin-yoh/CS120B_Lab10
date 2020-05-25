/*	Author: cyoh001
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>

enum States { Start, Up, Down, Begin, Wait, OnOff } state;
double notes[8] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
double temp = 0;
unsigned char min = 0x00;
unsigned char max = 0x07;
unsigned char current = 0x00;
unsigned char currentMode = 0x00;
unsigned char offHolder = 0x00;
unsigned char tempA = 0x00;

void set_PWM(double frequency)
{
	static double current_frequency;
	if (frequency != current_frequency)
	{
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

void Tick()
{
	tempA = ~PINA & 0x07;
	switch (state) // transitions
	{
	case Start:
	{
		state = Begin;
		break;
	}
	case Begin:
	{
		if (tempA == 0x01)
		{
			state = Up;
			break;
		}
		else if (tempA == 0x02)
		{
			state = Down;
			break;
		}
		else if (tempA == 0x04)
		{
			state = OnOff;
			break;
		}
		else
		{
			state = Begin;
			break;
		}
	}
	case Up:
	{
		temp = notes[current];
		state = Wait;
		break;
	}
	case Down:
	{
		temp = notes[current];
		state = Wait;
		break;
	}
	case Wait:
	{
		if (tempA == 0x00)
		{
			state = Begin;
			break;
		}
		else
		{
			state = Wait;
			break;
		}
	}
	case OnOff:
	{
		state = Wait;
		break;
	}
	default:
		break;
	}
	switch (state) // state actions
	{
	case Up:
	{
		if ((current + 1) > max)
		{
			current = max;
			break;
		}
		else
		{
			current++;
			break;
		}
	}
	case Down:
	{
		if ((current - 1) < min)
		{
			current = min;
			break;
		}
		else
		{
			current--;
			break;
		}
	}
	case Wait:
	{
		temp = notes[current];
		set_PWM(temp);
		break;
	}
	case OnOff:
	{
		if (currentMode == 0x01)
		{
			PWM_off();
			offHolder = current;
			set_PWM(0);
			currentMode = 0;
			break;
		}
		else
		{
			PWM_on();
			current = offHolder;
			currentMode = 1;
			break;
		}
	}
	default:
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	state = Start;
	PWM_on();
	currentMode = 1;

	while (1)
	{
		Tick();
	}
}
/*	Author: lab
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_threeLedTrackercurr = 0;
void TimerOn()
{
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_threeLedTrackercurr = _avr_timer_M;
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
	_avr_timer_threeLedTrackercurr--;
	if (_avr_timer_threeLedTrackercurr == 0)
	{
		TimerISR();
		_avr_timer_threeLedTrackercurr = _avr_timer_M;
	}
}
void TimerSet(unsigned long M)
{
	_avr_timer_M = M;
	_avr_timer_threeLedTrackercurr = _avr_timer_M;
}

enum States_One { ThreeLedStart, ThreeLedFirst, ThreeLedSecond, ThreeLedThird } threeLedStates;
enum States_Two { BlinkingStart, BlinkingOn, BlinkingOff } blinkingStates;
enum States_Three { CombinedStart, Display } combinedStates;
enum States_Four { SpeakerStart, SpeakerOn, SpeakerOff } speakerStates;
unsigned char blinkingLED = 0x00;
unsigned char threeLEDs = 0x00;
unsigned char speakerTemp = 0x00;
unsigned char tempB = 0x00;
unsigned long blinkingTracker = 0;
unsigned long threeLedTracker = 0;
unsigned long speakerTracker = 0;
const unsigned long period = 1;

void ThreeLEDsSM()
{
	switch (threeLedStates) // Transitions
	{
	case ThreeLedStart:
	{
		threeLedStates = ThreeLedFirst;
		break;
	}
	case ThreeLedFirst:
	{
		threeLedStates = ThreeLedSecond;
		break;
	}
	case ThreeLedSecond:
	{
		threeLedStates = ThreeLedThird;
		break;
	}
	case ThreeLedThird:
	{
		threeLedStates = ThreeLedFirst;
		break;
	}
	default:
		break;
	}

	switch (threeLedStates) // State Actions
	{
	case ThreeLedStart:
		break;
	case ThreeLedFirst:
	{
		threeLEDs = 0x01;
		break;
	}
	case ThreeLedSecond:
	{
		threeLEDs = 0x02;
		break;
	}
	case ThreeLedThird:
	{
		threeLEDs = 0x04;
		break;
	}
	default:
		break;
	}
}
void BlinkingLEDSM()
{
	switch (blinkingStates) // Transitions
	{
	case BlinkingStart:
	{
		blinkingStates = BlinkingOn;
		break;
	}
	case BlinkingOn:
	{
		blinkingStates = BlinkingOff;
		break;
	}
	case BlinkingOff:
	{
		blinkingStates = BlinkingOn;
		break;
	}
	default:
		break;
	}
	switch (blinkingStates) // State Actions
	{
	case BlinkingStart:
		break;
	case BlinkingOn:
	{
		blinkingLED = 0x08;
		break;
	}
	case BlinkingOff:
	{
		blinkingLED = 0x00;
	}
	default:
		break;
	}
}

void SpeakerSM()
{
	switch (speakerStates) // Transitions
	{
	case SpeakerStart:
	{
		speakerStates = SpeakerOn;
		break;
	}
	case SpeakerOn:
	{
		speakerStates = SpeakerOff;
		break;
	}
	case SpeakerOff:
	{
		speakerStates = SpeakerOn;
		break;
	}
	default:
		break;
	}

	switch (speakerStates) // State Actions
	{
	case SpeakerOn:
	{
		speakerTemp = 0x10;
		break;
	}
	case SpeakerOff:
	{
		speakerTemp = 0x00;
		break;
	}
	default:
		break;
	}
}

void CombineLEDsSM()
{
	switch (combinedStates) // Transitions
	{
	case CombinedStart:
	{
		combinedStates = Display;
		break;
	}
	default:
		break;
	}
	switch (combinedStates) // State Actions
	{
	case Display:
	{
		tempB = speakerTemp | blinkingLED | threeLEDs ;
		PORTB = tempB;
		break;
	}
	default:
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	threeLedStates = ThreeLedStart;
	blinkingStates = BlinkingStart;
	combinedStates = CombinedStart;
	speakerStates = SpeakerStart;
	threeLedTracker = 0;
	blinkingTracker = 0;
	speakerTracker = 0;

	TimerSet(1);
	TimerOn();
	while (1)
	{
		if (threeLedTracker >= 300)
		{
			ThreeLEDsSM();
			threeLedTracker = 0;
		}
		if (blinkingTracker >= 1000)
		{
			BlinkingLEDSM();
			blinkingTracker = 0;
		}
		if (speakerTracker >= 1000)
		{
			SpeakerSM();
			speakerTracker = 0;
		}
		else
		{
			speakerTemp = 0x00;
		}
		CombineLEDsSM();
		while (!TimerFlag);
		TimerFlag = 0;
		blinkingTracker += period;
		threeLedTracker += period;
		speakerTracker += period;
	}
}
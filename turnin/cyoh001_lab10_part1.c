/*Author: cyoh001
* Partner(s) Name :
	*Lab Section :
*Assignment : Lab #  Exercise #
* Exercise Description : [optional - include for your own benefit]
*
*I acknowledge all content contained herein, excluding template or example
* code, is my own original work.
*/
#include <avr/io.h>
#include <avr/interrupt.h>

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
unsigned char blinkingLED = 0x00;
unsigned char threeLEDs = 0x00;
unsigned char tempB = 0x00;
unsigned short blinkingTracker = 0x00;
unsigned short threeLedTracker = 0x00;

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
		if (threeLedTracker < 1000)
		{
			threeLedStates = ThreeLedFirst;
			++threeLedTracker;
		}
		else
		{
			threeLedStates = ThreeLedSecond;
			threeLedTracker = 0;
		}
		break;
	}
	case ThreeLedSecond:
	{
		if (threeLedTracker < 1000)
		{
			threeLedStates = ThreeLedSecond;
			++threeLedTracker;
		}
		else
		{
			threeLedStates = ThreeLedThird;
			threeLedTracker = 0;
		}
		break;
	}
	case ThreeLedThird:
	{
		if (threeLedTracker < 1000)
		{
			threeLedStates = ThreeLedThird;
			++threeLedTracker;
		}
		else
		{
			threeLedStates = ThreeLedFirst;
			threeLedTracker = 0;
		}
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
		if (blinkingTracker < 1000)
		{
			blinkingStates = BlinkingOn;
			++blinkingTracker;
		}
		else
		{
			blinkingStates = BlinkingOff;
			blinkingTracker = 0;
		}
		break;
	}
	case BlinkingOff:
	{
		if (blinkingTracker < 1000)
		{
			blinkingStates = BlinkingOff;
			++blinkingTracker;
		}
		else
		{
			blinkingStates = BlinkingOn;
			blinkingTracker = 0;
		}
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
		tempB = blinkingLED | threeLEDs;
		PORTB = tempB;
		break;
	}
	default:
		break;
	}
}

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
	threeLedStates = Start;
	blinkingStates = BlinkingStart;
	combinedStates = CombinedStart;
	threeLedTracker = 0;
	blinkingTracker = 0;

	TimerSet(1);
	TimerOn();
	while (1)
	{
		ThreeLEDsSM();
		BlinkingLEDSM();
		CombineLEDsSM();
		while (!TimerFlag);
		TimerFlag = 0;
	}
}
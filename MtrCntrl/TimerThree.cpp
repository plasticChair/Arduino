/*
 *  Interrupt and PWM utilities for 16 bit Timer3 on ATmega1280
 *
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified Oct 2009 by Dan Clemens to work with timer3 of the ATMega1280 or Arduino Mega
 *  Modified March 2010 by Albert Sanz to make compatible with Arduino Mega Timer3
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "TimerThree.h"
 // preinstatiate

ISR(TIMER3_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  cntrlTimer.isrCallback();
}

void TimerThree::initialize(long microseconds)
{
  TCCR3A = 0;                 // clear control register A 
  TCCR3B = _BV(WGM33);        // set mode as phase and frequency correct pwm, stop the timer
  setPeriod(microseconds);
}

void TimerThree::setPeriod(long microseconds)
{
  long cycles = (F_CPU * microseconds) / 2000000;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS30);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS31);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS31) | _BV(CS30);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS32);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS32) | _BV(CS30);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS32) | _BV(CS30);  // request was out of bounds, set as maximum
  ICR3 = pwmPeriod = cycles;                                                     // ICR1 is TOP in p & f correct pwm mode
  TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
  TCCR3B |= clockSelectBits;                                                     // reset clock select register
}

void TimerThree::setPwmDuty(char pin, int duty) // expects duty cycle to be 12 bit 4096 (50%=2048, 100%=4096)
{
  unsigned long dutyCycle = pwmPeriod;
  dutyCycle *= duty;
  dutyCycle >>= 12;
  if(pin == 5) {OCR3A = dutyCycle;}
  if(pin == 3) {OCR3B = dutyCycle;}
  if(pin == 2) {OCR3C = dutyCycle;}
}

void TimerThree::pwm(char pin, int duty, long microseconds)  // expects duty cycle to be 12 bit 4096 (50%=2048, 100%=4096)
{
  if(microseconds > 0) setPeriod(microseconds);
  
	// sets data direction register for pwm output pin
	// activates the output pin
  if(pin == 5) { DDRL |= _BV(PORTL3); TCCR3A |= _BV(COM3A1); }
  if(pin == 3) { DDRL |= _BV(PORTL4); TCCR3A |= _BV(COM3B1); }
  if(pin == 2) { DDRL |= _BV(PORTL5); TCCR3A |= _BV(COM3C1); }
  setPwmDuty(pin, duty);
  start();
}

void TimerThree::disablePwm(char pin)
{
  if(pin == 5) TCCR3A &= ~_BV(COM3A1);   // clear the bit that enables pwm on PL3
  if(pin == 3) TCCR3A &= ~_BV(COM3B1);   // clear the bit that enables pwm on PL4
  if(pin == 2) TCCR3A &= ~_BV(COM3C1);   // clear the bit that enables pwm on PL5
}

void TimerThree::attachInterrupt(void (*isr)(), long microseconds)
{
  if(microseconds > 0) setPeriod(microseconds);
  isrCallback = isr;                                       // register the user's callback with the real ISR
  TIMSK3 = _BV(TOIE3);                                     // sets the timer overflow interrupt enable bit
  sei();                                                   // ensures that interrupts are globally enabled
  start();
}

void TimerThree::detachInterrupt()
{
  TIMSK3 &= ~_BV(TOIE3);                                   // clears the timer overflow interrupt enable bit 
}

void TimerThree::start()
{
  TCCR3B |= clockSelectBits;
}

void TimerThree::stop()
{
  TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));          // clears all clock selects bits
}

void TimerThree::restart()
{
  TCNT3 = 0;
}

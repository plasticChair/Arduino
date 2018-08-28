/*
 *  Interrupt and PWM utilities for 16 bit Timer4 on ATmega1280
 *
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega428 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified Oct 2009 by Dan Clemens to work with timer4 of the ATMega1280 or Arduino Mega
 *  Modified March 2010 by Albert Sanz to make compatible with Arduino Mega Timer4
 *
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 4 of the License, or
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

#include "TimerFour.h"
TimerFour Timer4;

ISR(TIMER4_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  Timer4.isrCallback();
}

void TimerFour::initialize(long microseconds)
{
  TCCR4A = 0;                 // clear control register A 
  TCCR4B = _BV(WGM43);        // set mode as phase and frequency correct pwm, stop the timer
  setPeriod(microseconds);
}

void TimerFour::setPeriod(long microseconds)
{
  long cycles = (F_CPU * microseconds) / 2000000;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS40);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS41);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS41) | _BV(CS40);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS42);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS42) | _BV(CS40);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS42) | _BV(CS40);  // request was out of bounds, set as maximum
  ICR4 = pwmPeriod = cycles;                                                     // ICR1 is TOP in p & f correct pwm mode
  TCCR4B &= ~(_BV(CS40) | _BV(CS41) | _BV(CS42));
  TCCR4B |= clockSelectBits;                                                     // reset clock select register
}

void TimerFour::setPwmDuty(char pin, int duty) // expects duty cycle to be 12 bit 4096 (50%=2048, 100%=4096)
{
  unsigned long dutyCycle = pwmPeriod;
  dutyCycle *= duty;
  dutyCycle >>= 12;
  if(pin == 46) {OCR4A = dutyCycle;}
  if(pin == 45) {OCR4B = dutyCycle;}
  if(pin == 44) {OCR4C = dutyCycle;}
}

void TimerFour::pwm(char pin, int duty, long microseconds)  // expects duty cycle to be 12 bit 4096 (50%=2048, 100%=4096)
{
  if(microseconds > 0) setPeriod(microseconds);
  
  // sets data direction register for pwm output pin
  // activates the output pin
  if(pin == 46) { DDRL |= _BV(PORTL3); TCCR4A |= _BV(COM4A1); }
  if(pin == 45) { DDRL |= _BV(PORTL4); TCCR4A |= _BV(COM4B1); }
  if(pin == 44) { DDRL |= _BV(PORTL5); TCCR4A |= _BV(COM4C1); }
  setPwmDuty(pin, duty);
  start();
}

void TimerFour::disablePwm(char pin)
{
  if(pin == 46) TCCR4A &= ~_BV(COM4A1);   // clear the bit that enables pwm on PL4
  if(pin == 45) TCCR4A &= ~_BV(COM4B1);   // clear the bit that enables pwm on PL4
  if(pin == 44) TCCR4A &= ~_BV(COM4C1);   // clear the bit that enables pwm on PL5
}

void TimerFour::attachInterrupt(void (*isr)(), long microseconds)
{
  if(microseconds > 0) setPeriod(microseconds);
  isrCallback = isr;                                       // register the user's callback with the real ISR
  TIMSK4 = _BV(TOIE4);                                     // sets the timer overflow interrupt enable bit
  sei();                                                   // ensures that interrupts are globally enabled
  start();
}

void TimerFour::detachInterrupt()
{
  TIMSK4 &= ~_BV(TOIE4);                                   // clears the timer overflow interrupt enable bit 
}

void TimerFour::start()
{
  TCCR4B |= clockSelectBits;
}

void TimerFour::stop()
{
  TCCR4B &= ~(_BV(CS40) | _BV(CS41) | _BV(CS42));          // clears all clock selects bits
}

void TimerFour::restart()
{
  TCNT4 = 0;
}

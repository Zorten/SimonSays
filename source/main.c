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
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "../header/timer.h"
#include "../header/scheduler.h"

//--------------------------------------
// LED Matrix Demo SynchSM
// Period: 100 ms
//--------------------------------------
enum Demo_States {shift};
int Demo_Tick(int state) {

	// Local Variables
	static unsigned char pattern = 0x80;	// LED pattern - 0: LED off; 1: LED on
	static unsigned char row = 0xFE;  	// Row(s) displaying pattern. 
							// 0: display pattern on row
							// 1: do NOT display pattern on row
	// Transitions
	switch (state) {
		case shift:	
			break;
		default:	
			state = shift;
			break;
	}	
	// Actions
	switch (state) {
		case shift:	
			if (row == 0xEF && pattern == 0x01) { // Reset demo 
				pattern = 0x80;		    
				row = 0xFE;
			} else if (pattern == 0x01) { // Move LED to start of next row
				pattern = 0x80;
				row = (row << 1) | 0x01;
			} else { // Shift LED one spot to the right on current row
				pattern >>= 1;
			}
			break;
		default:
			break;
	}
	PORTC = pattern;	// Pattern to display
	PORTD = row;		// Row(s) displaying pattern	
	return state;
}


int main(void) {
	/* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	/* Insert your solution below */

	task tasks[1];
	const unsigned short numTasks = 1;
	
	const unsigned long tasksPeriodGCD = 100;
	const unsigned long periodDemo = 100;
	
	unsigned char t = 0; 
	tasks[t].state = shift;
	tasks[t].period = periodDemo;
	tasks[t].elapsedTime = tasks[t].period;
	tasks[t].TickFct = &Demo_Tick;
	++t;

	//Set the timer and turn it on
	TimerSet(tasksPeriodGCD);
	TimerOn();
	
	unsigned short i; //scheduler for loop iterator
	
	while (1) {
		for (i = 0; i < numTasks; i++){
			if (tasks[i].elapsedTime >= tasks[i].period){
				tasks[i].state = tasks[i].TickFct(tasks[i].state);
				tasks[i].elapsedTime = 0;
			}
			tasks[i].elapsedTime += tasksPeriodGCD;
		}
		while(!TimerFlag){};
		TimerFlag = 0;
	}
	
	return 1;
}









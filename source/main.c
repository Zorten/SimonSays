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
enum TopLeft_States {init, hold1, hold2, hold3, hold4, off1};
int TickFct_TopLeft(int state) {

	// Local Variables
	static unsigned char column = 0x00;	// LED column - 0: LED off; 1: LED on
	static unsigned char row = 0x00;  	// Row(s) displaying column. 
							// 0: display column on row
							// 1: do NOT display column on row
	// Transitions
	switch (state) {
		case init:
			state = hold1;
			break;

		case hold1:	
			state = hold2;
			break;

		case hold2:	
			state = hold3;
			break;

		case hold3:	
			state = hold4;
			break;

		case hold4:	
			state = off1;
			break;

		case off1:
			state = hold1;
			break;

		default:	
			state = init;
			break;
	}	
	// Actions
	switch (state) {
		case init:
			break;
		case hold1:
			column = 0xE0;	
			row = 0x03;
			break;

		case hold2:
			column = 0x07;	
			row = 0x03;
			break;

		case hold3:
			column = 0xE0;	
			row = 0x18;
			break;

		case hold4:
			column = 0x07;	
			row = 0x18;
			break;	

		case off1:
			column = 0x00;
			row = 0x00;	

		default:
			break;
	}

	PORTC = column;	// column to display
	PORTD = ~row;		// Row(s) displaying column	
	return state;
}

/*

enum TopRight_States {hold2, off2};
int TickFct_TopRight(int state) {

	// Local Variables
	static unsigned char column = 0x00;	// LED column - 0: LED off; 1: LED on
	static unsigned char row = 0x00;  	// Row(s) displaying column. 
							// 0: display column on row
							// 1: do NOT display column on row
	// Transitions
	switch (state) {
		case hold2:	
			state = off2;
			break;

		case off2:
			state = hold2;
			break;

		default:	
			state = hold2;
			break;
	}	
	// Actions
	switch (state) {
		case hold2:
			column = 0x07;	
			row = 0x03;
			break;

		case off2:
			column = 0x00;
			row = 0x00;

		default:
			break;
	}
	PORTC = column;	// column to display
	PORTD = ~row;		// Row(s) displaying column	
	return state;
}

enum BottomLeft_States {hold3, off3};
int TickFct_BottomLeft(int state) {

	// Local Variables
	static unsigned char column = 0x00;	// LED column - 0: LED off; 1: LED on
	static unsigned char row = 0x00;  	// Row(s) displaying column. 
							// 0: display column on row
							// 1: do NOT display column on row
	// Transitions
	switch (state) {
		case hold3:	
			state = off3;
			break;

		case off3:
			state = hold3;
			break;

		default:	
			state = hold3;
			break;
	}	
	// Actions
	switch (state) {
		case hold3:
			column = 0xE0;	
			row = 0x18;
			break;

		case off3:
			column = 0x00;
			row = 0x00;

		default:
			break;
	}
	PORTC = column;	// column to display
	PORTD = ~row;		// Row(s) displaying column	
	return state;
}

enum BottomRight_States {hold4, off4};
int TickFct_BottomRight(int state) {

	// Local Variables
	static unsigned char column = 0x00;	// LED column - 0: LED off; 1: LED on
	static unsigned char row = 0x00;  	// Row(s) displaying column. 
							// 0: display column on row
							// 1: do NOT display column on row
	// Transitions
	switch (state) {
		case hold4:	
			state = off4;
			break;

		case off4:
			state = hold4;
			break;

		default:	
			state = hold4;
			break;
	}	
	// Actions
	switch (state) {
		case hold4:
			column = 0x07;	
			row = 0x18;
			break;

		case off4:
			column = 0x00;
			row = 0x00;

		default:
			break;
	}
	PORTC = column;	// column to display
	PORTD = ~row;		// Row(s) displaying column	
	return state;
}

*/


int main(void) {
	/* Insert DDR and PORT initializations */
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	/* Insert your solution below */

	task tasks[1];
	const unsigned short numTasks = 1;
	
	const unsigned long tasksPeriodGCD = 500;
	const unsigned long periodTopLeft = 1000;

/*
	const unsigned long periodTopRight = 500;
	const unsigned long periodBottomLeft = 500;
	const unsigned long periodBottomRight = 500;

*/
	
	unsigned char t = 0;
 
	tasks[t].state = init;
	tasks[t].period = periodTopLeft;
	tasks[t].elapsedTime = tasks[t].period;
	tasks[t].TickFct = &TickFct_TopLeft;
	++t;

/*

	tasks[t].state = hold2;
	tasks[t].period = periodTopRight;
	tasks[t].elapsedTime = tasks[t].period;
	tasks[t].TickFct = &TickFct_TopRight;
	++t;

	tasks[t].state = hold3;
	tasks[t].period = periodBottomLeft;
	tasks[t].elapsedTime = tasks[t].period;
	tasks[t].TickFct = &TickFct_BottomLeft;
	++t;

	tasks[t].state = hold4;
	tasks[t].period = periodBottomRight;
	tasks[t].elapsedTime = tasks[t].period;
	tasks[t].TickFct = &TickFct_BottomRight;
	++t;

*/
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









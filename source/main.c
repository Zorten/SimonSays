#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "../header/timer.h"
#include "../header/scheduler.h"
#include "stdbool.h"

bool start = false; //variable to determine whether game has started or not
enum Menu_States {waitOn, waitOff, input} Menu_state;
void TickFct_Menu(){ //Function will flash all 4 quarters as long as there's no input. Aka the game hasn't begun.
    char buttons = ~PINA;
    switch (Menu_state){
        case waitOn:
            if (buttons == 0x00){
                Menu_state = waitOff;
            }
            else{
                Menu_state = input;
            }
            break;
        
	case waitOff:
            if (buttons == 0x00){
                Menu_state = waitOn;
            }
            else{
                Menu_state = input;
            }
            break;

        case input:
            break;

        default:
            break;
    }

    switch (Menu_state){
        case waitOn:
            PORTC = 0xE7;
            PORTD = ~(0x1B);
            break;

	case waitOff:
		PORTC = 0x00;
		PORTD = ~(0x00);
		break;

        case input:
            PORTC = 0x00;
            PORTD = ~(0x00);
            start = true;
            break;

        default:
            break;
    }
}

char Simon[3] = {1, 4, 3};
static unsigned char round = 0;
enum Game_States {sequence};
int TickFct_Game(int state){

	//static unsigned char column = 0x00; 
	//static unsigned char row = 0x00; 
	unsigned char quarter = 0;
	unsigned long loop = 0;
	
	switch (state) {
		case sequence:
			for (unsigned char i = 0; i < round; ++i){
				quarter = Simon[i];
				switch (quarter){
					case 1:
						while (loop < 100000){
							PORTC = 0xE0;	
							PORTD = ~(0x03);
							++loop;
						}
						loop = 0;

						while (loop < 50000){
							PORTC = 0x00;	
							PORTD = ~(0x00);
							++loop;
						}
						loop = 0;
						break;

					case 2:
						while (loop < 100000){
							PORTC = 0x07;	
							PORTD = ~(0x03);
							++loop;
						}
						loop = 0;

						while (loop < 50000){
							PORTC = 0x00;	
							PORTD = ~(0x00);
							++loop;
						}
						loop = 0;
						break;	
					case 3:
						while (loop < 100000){
							PORTC = 0xE0;	
							PORTD = ~(0x18);
							++loop;
						}
						loop = 0;

						while (loop < 50000){
							PORTC = 0x00;	
							PORTD = ~(0x00);
							++loop;
						}
						loop = 0;
						break;

					case 4:
						while (loop < 100000){
							PORTC = 0x07;	
							PORTD = ~(0x18);
							++loop;
						}
						loop = 0;

						while (loop < 50000){
							PORTC = 0x00;	
							PORTD = ~(0x00);
							++loop;
						}
						loop = 0;
						break;
				}
				//PORTC = column;
				//PORTD = ~row;
			}
	}

	return state; 
}

enum Input_states {init, wait, check};
int TickFct_Input(int state){
	char buttons = ~PINA;
	unsigned char press = 0;
	switch (state){
		case init:
			PORTC = 0x00;
			PORTD = ~(0x00);
			state = wait;
			break;

		case wait:
			if (buttons == 0x00){
				state = wait; 			
			}
			else if (buttons == 0x01){
				state = check;
				press = 1;
			}
			else if (buttons == 0x02){
				state = check;
				press = 2;
			}
			else if (buttons == 0x04){
				state = check;
				press = 3;
			}
			else if (buttons == 0x08){
				state = check;
				press = 4;
			}
			break;

		case check:
			
			break;
	}

	switch (state){
		case init:
			break;

		case wait:
			break;

		case check:
			break;
	}
}



int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
    	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

    //Setting up task scheduler
	task tasks[1];
	const unsigned short numTasks = 1;
	const unsigned long tasksPeriodGCD = 100;
	const unsigned long periodGame = 1000;

/*
	const unsigned long periodTopRight = 500;
	const unsigned long periodBottomLeft = 500;
	const unsigned long periodBottomRight = 500;
*/
	
	unsigned char t = 0;
 
	tasks[t].state = sequence;
	tasks[t].period = periodGame;
	tasks[t].elapsedTime = tasks[t].period;
	tasks[t].TickFct = &TickFct_Game;
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
	
	
	Menu_state = waitOn; //initial state for menu 
    	unsigned short i; //scheduler for-loop iterator
	while (1) {
        	if (!start){ //menu function independent as it should only run when the game isn't being played
            		TickFct_Menu();
        	}
        	else{
			round = 1;
			for (i = 0; i < numTasks; i++){
				if (tasks[i].elapsedTime >= tasks[i].period){
				    tasks[i].state = tasks[i].TickFct(tasks[i].state);
				    tasks[i].elapsedTime = 0;
				}
				tasks[i].elapsedTime += tasksPeriodGCD;
		    	}
        	}
	
		while(!TimerFlag){};
		TimerFlag = 0;
	}
	
	return 1;
}



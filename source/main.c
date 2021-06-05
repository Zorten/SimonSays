#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "../header/timer.h"
#include "../header/scheduler.h"
#include "stdbool.h"

static bool start = false; //variable to determine whether game has started or not
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

const unsigned char levels = 3;
char Simon[3] = {1, 4, 3};
static unsigned char round = 0;
enum Game_States {sequence} Game_state;
void TickFct_Game(){ 
	unsigned char quarter = 0;
	unsigned long loop = 0;
	
	switch (Game_state) {
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
			}
	} 
}

static unsigned char correctPresses = 0;
unsigned char pressCount = 0;
bool inputDone = false;
enum Input_states {init, wait, check, done} Input_state;
void TickFct_Input(){
	unsigned long loop = 0;
	char buttons = ~PINA;
	unsigned char press = 0;
	switch (Input_state){
		case init:
			PORTC = 0x00;
			PORTD = ~(0x00);
			Input_state = wait;
			break;

		case wait:
			PORTC = 0x00;
			PORTD = ~(0x00);
			if (buttons == 0x00){
				Input_state = wait; 			
			}
			else if (buttons == 0x01){
				Input_state = check;
				press = 1;
			}
			else if (buttons == 0x02){
				Input_state = check;
				press = 2;
			}
			else if (buttons == 0x04){
				Input_state = check;
				press = 3;
			}
			else if (buttons == 0x08){
				Input_state = check;
				press = 4;
			}
			break;

		case check:
			if (pressCount < round){
				Input_state = wait;
			}
			else{
				Input_state = done;
			}
			break;

		case done:
			Input_state = wait;
			break;
	}

	switch (Input_state){
		case init:
			break;

		case wait:
			break;

		case check:
			++pressCount;
			if (press == 1){
				while (loop < 50000){
					PORTC = 0xE0;	
					PORTD = ~(0x03);
					++loop;
				}
				loop = 0;
			}
			else if (press == 2){
				while (loop < 50000){
					PORTC = 0x07;	
					PORTD = ~(0x03);
					++loop;
				}
				loop = 0;
			}
			else if (press == 3){
				while (loop < 50000){
					PORTC = 0xE0;	
					PORTD = ~(0x18);
					++loop;
				}
				loop = 0;
			}
			else if (press == 4){
				while (loop < 50000){
					PORTC = 0x07;	
					PORTD = ~(0x18);
					++loop;
				}
				loop = 0;
			}


			if (press == Simon[pressCount - 1]){
				++correctPresses;
			}
			break;

		case done: 
			inputDone = true;
			break;
	}
}



int main(void) {
	/* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
    	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;


	unsigned long Menu_elapsedTime = 100;
    	//unsigned long Game_elapsedTime = 1000;
    	//unsigned long Input_elapsedTime = 100;


	const unsigned char timerPeriod = 50;


	//Set the timer and turn it on
	TimerSet(timerPeriod);
	TimerOn();

	Menu_state = waitOn; //initial state for menu
	Game_state = sequence;
	Input_state = init;
		

	unsigned long j = 0;
	bool gameOver = false; 
	while (1) {
        	if (!start){ //menu function independent as it should only run when the game isn't being played
			if (Menu_elapsedTime >= 100){
            			TickFct_Menu();
				Menu_elapsedTime = 0;
			}
        	}
        	else{
				round = 1;
				while (!gameOver){
					correctPresses = 0;
					TickFct_Game();
					while (!inputDone){
						if (j >= 1000){
							TickFct_Input();
							j = 0;	
						}
						++j;
					}
					inputDone = false; 

					if (correctPresses == round){
						if (round == levels){
							gameOver = true;
							start = false;
						}
						else{
							++round;
						}
					}
					else{
						gameOver = true;
						start = false;
					}
				}
        	}

		
		while(!TimerFlag){};
		TimerFlag = 0;
		Menu_elapsedTime += timerPeriod;

	}
	
	return 1;
}


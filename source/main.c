/*	Author: Zergio Ruvalcaba
 *	Lab Section: 23
 *	Assignment: Lab #11
 *
 *	Project Description: This is the Simon Says game. An LED Matrix is used and divided into
 *	4 quarters. On the first round a quarter will light up, and you must then press the button mapped to that quarter.
 *	Each round from then on the number of quarters that light up increases, starting with the ones that lit in the 
 *	previous round, creating an increasingly difficult sequence. If you input the wrong sequence, the game ends and restarts. 
 *	You win if you get through the entire sequence. 
 *
 *	I acknowledge all content contained herein, excluding template, provided code, or example
 *	code, is my own original work.
 *	
 *	Demo Link <>
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "../header/timer.h"
#include "../header/scheduler.h"
#include "stdbool.h"
#include "stdlib.h"
#include "../header/pwm.h"
#include "time.h"

//=======================Global Variables=====================================================
static bool start = false; //Determine whether game has started or not
static bool inputDone = false; //Determine whether user input is finished
bool gameOver = false; //Determine whether game is over or not
static unsigned char correctPresses = 0; //Keeps track if the user is pressing correct sequence
static unsigned char pressCount = 0; //Kepps track of how many buttons have been pressed
static unsigned char round = 0; //Keeps track of current round
const unsigned char levels = 5; //Total number of rounds
char Simon[5]; //Array that holds the sequence. Numbers 1-4 are mapped to the 4 quarters
			        //1 = Top Left; 2 = Top Right; 3 = Bottom Left; 4 = Bottom Right
//=============================================================================================


//=======================Enum declarations=====================================================
enum Menu_States {waitOn, waitOff, input} Menu_state;
enum Game_States {sequence} Game_state;
enum Input_States {init, wait, check, done} Input_state;
enum GameWon_States {celebration} GameWon_state;
//=============================================================================================


//=======================SM Functions==========================================================

//Function will flash all 4 quarters as long as there's no input. 
//Aka the game hasn't begun. Once input is detected, start variable is set to true
void TickFct_Menu(){ 
    char buttons = ~PINA; //capture button presses

    switch (Menu_state){ //transitions
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
			Menu_state = waitOn; //After game is started and finishes, it'll come back here, so set back to initial state
            break;

        default:
            break;
    }

    switch (Menu_state){ //actions
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


//Function will display sequence according to current round.
//This must be done in one tick of the function, so there is only one state.
void TickFct_Game(){ 
	unsigned int quarter = 0; //Used to check which quarter should light up according to the sequence
	unsigned long loop = 0; //Used to light LED for about a second since this is done in one tick
	
	switch (Game_state) {
		case sequence:
			for (unsigned char i = 0; i < round; ++i){ //For loop runs depending on the current round
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


//Function will wait for user input. When there's an input, the corresponding quarter will light up
//and the input will be compared with the expected sequence, if input is correct, correctPresses is
//increased so that it can be checked against the current round. If input is wrong, game will end.
void TickFct_Input(){
	unsigned long loop = 0; //Used to light up LED after input is detected
	char buttons = ~PINA; //Used to capture button presses
	unsigned char press = 0; //Used to map a button press to respective quarter

	switch (Input_state){
		case init:
			PORTC = 0x00;
			PORTD = ~(0x00);
			Input_state = wait;
			break;

		case wait:
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
			//check to see if correct input sequence is over
			if (pressCount < round){ 
				Input_state = wait;
			}
			else{
				Input_state = done;
			}
			break;

		case done:
			Input_state = wait; //When called again, it will return here so set it back to wait
			break;
	}

	switch (Input_state){
		case init:
			break;

		case wait:
			PORTC = 0x00;
			PORTD = ~(0x00);
			break;

		case check:
			//First light up for about a second and then turn off, to give time between input and game transitions
			++pressCount;
			if (press == 1){
				while (loop < 50000){
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
			}
			else if (press == 2){
				while (loop < 50000){
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
			}
			else if (press == 3){
				while (loop < 50000){
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
			}
			else if (press == 4){
				while (loop < 50000){
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
			}

			//Check if the button pressed is the correct one according to the game sequence
			if (press == Simon[pressCount - 1]){
				++correctPresses;
			}
			else{
				gameOver = true;
				start = false;
			}
			break;

		case done: 
			//End input
			inputDone = true;
			break;
	}
}

void TickFct_GameWon(){
	unsigned long loop = 0;

	switch (GameWon_state){
		case celebration:
			while (loop < 100000){
				PORTC = 0xFF;	
				PORTD = 0x00;
				++loop;
			}
			loop = 0;

			while (loop < 100000){
				PORTC = 0x00;	
				PORTD = ~(0x00);
				++loop;
			}
			loop = 0;

			while (loop < 100000){
				PORTC = 0xFF;	
				PORTD = 0x00;
				++loop;
			}
			loop = 0;

			while (loop < 100000){
				PORTC = 0x00;	
				PORTD = ~(0x00);
				++loop;
			}
			loop = 0;
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

	//Initial states for SMs
	Menu_state = waitOn; 
	Game_state = sequence; 
	Input_state = init;
	GameWon_state = celebration;
		
	unsigned long seed = 0;
	unsigned long j = 0; //used to tick input function 
	while (1) {
        	if (!start){  //menu function as long as there's no input
			if (Menu_elapsedTime >= 100){
            			TickFct_Menu();
				Menu_elapsedTime = 0;
				++seed;
			}
        	}
        	else{
			//Populate game array with random values
			srand(seed); // Unique seed
			for (unsigned char i = 0; i < levels; ++i){
				Simon[i] = ((rand() % 4) + 1);
			}
	
			gameOver = false; //reset variables
			round = 1;
			while (!gameOver){
				TickFct_Game(); //Display sequence
				while (!inputDone){
					if (j >= 1000){
						TickFct_Input(); //Get user input
						if (gameOver){
							break;
						}
						j = 0;	
					}
					++j;
				}
				inputDone = false; //reset variable

				if (correctPresses == round){ //check if the sequence inputted was correct
					if (round == levels){
						gameOver = true;
						start = false;
						TickFct_GameWon();
					}
					else{
						++round;
					}
				}
				else{
					gameOver = true;
					start = false;
				}
				correctPresses = 0;
				pressCount = 0;
			}
        	}

		
		while(!TimerFlag){};
		TimerFlag = 0;
		Menu_elapsedTime += timerPeriod;

	}
	return 1;
}

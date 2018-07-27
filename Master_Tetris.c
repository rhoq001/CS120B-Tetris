/*
 * GccApplication1.c
 *
 * Created: 3/2/2016 6:14:25 PM
 *  Author: student
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "C:/Users/Raqtan/Downloads/header files/timer.h"
#include "C:/Users/Raqtan/Downloads/USARTtestfile.h"
const unsigned char one = 0x01;
unsigned char temps[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char i = 0;
unsigned char checkShift = 0x00;
unsigned short waitTime = 0;
unsigned char waitTimeShift = 0;
unsigned char move = 0x00;
unsigned char randomGenerated = 0x00;
unsigned char shifted = 0x00;
unsigned char lineNum = 0x00;
unsigned char startGame = 0x00;
unsigned char receivedNum = 0x00;
unsigned char controlTimer = 0x00;

void sendData(unsigned char x){
	unsigned char hasSent = 0;
	while(!hasSent){
		if(USART_IsSendReady(1)){
			USART_Send(x,1);
			hasSent = 1;
		}
	}

}

unsigned char recieveData(){
	unsigned char temp;
	while(1){
		if (USART_HasReceived(0)){
			temp = USART_Receive(0);
			USART_Flush(0);
			return temp;
		}
	}
}

void transmit_data(unsigned char data) //controls red columns
{
	for(int a = 4; a >= 0; --a)
	{
		PORTD = 0x80;
		PORTD |= ((data << a) & 0x10);
		PORTD |= 0x40;
	}
	for(int a = 1; a <= 3; ++a)
	{
		PORTD = 0x80;
		PORTD |= ((data >> a) & 0x10);
		PORTD |= 0x40;
	}
	PORTD |= 0x20;
	PORTD = 0x00;
}


void transmit_data2(unsigned char data) //controls red columns
{
	for(int a = 4; a >= 0; --a)
	{
		PORTB = 0x80;
		PORTB |= ((data << a) & 0x10);
		PORTB |= 0x40;
	}
	for(int a = 1; a <= 3; ++a)
	{
		PORTB = 0x80;
		PORTB |= ((data >> a) & 0x10);
		PORTB |= 0x40;
	}
	PORTB |= 0x20;
	PORTB = 0x00;
}

// void transmit_data2(unsigned char data) //controls red columns
// {
// 	for(int a = 6; a >= 0; --a)
// 	{
// 		PORTB = 0x10;
// 		PORTB |= ((data << a) & 0x02);
// 		PORTB |= 0x08;
// 	}
// 	for(int a = 1; a <= 1; ++a)
// 	{
// 		PORTB = 0x10;
// 		PORTB |= ((data >> a) & 0x02);
// 		PORTB |= 0x08;
// 	}
// 	PORTB |= 0x04;
// 	PORTB = 0x00;
// }

void InitADC()
{
	ADMUX=(1<<REFS0);   //set seventh bit of ADMUX to 1 so reference voltage is 5v
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //rescalar div factor =128
}
void convert(){
	ADCSRA |=(1<<ADSC);//start ADC conversion
	while ( !(ADCSRA & (1<<ADIF)));//wait till ADC conversion
	
}

typedef struct shape{
	unsigned char center_x;
	unsigned char center_y;
	unsigned char bottom_x;
	unsigned char bottom_y;
	unsigned char coordinate[4][2];
} shape;

shape currentShape;
shape L;
shape J;
shape I;
shape C1;
shape C2;
shape S;
shape T;

void setShape(shape x){
	currentShape.bottom_x = x.bottom_x;
	currentShape.bottom_y = x.bottom_y;
	currentShape.center_x = x.center_x;
	currentShape.center_y = x.center_y;
	for(unsigned char a = 0; a < 4; a++){
		currentShape.coordinate[a][0] = x.coordinate[a][0];
		currentShape.coordinate[a][1] = x.coordinate[a][1];
	}
	return;
}

int isOn(unsigned char x, unsigned char y){
	if((temps[y] & (one << (7 - x))) >> (7 - x)){
		return 1;
	}
	else return 0;
}

void turnOnLED(unsigned char x, unsigned char y){
	if(x < 0){
		return;
	}
	if(x > 7){
		return;
	}
	if(y < 0){
		return;
	}
	if(y > 7){
		return;
	}
	temps[y] |= (one << (7 - x));
	return;
}

void turnOffLED(unsigned char x, unsigned char y){
	if(x < 0){
		return;
	}
	if(x > 7){
		return;
	}
	if(y < 0){
		return;
	}
	if(y > 7){
		return;
	}
	temps[y] &= ~(one << (7 - x));
	return;
}

void ShapesInit(){
	L.bottom_x = 0;
	L.bottom_y = 1;
	L.coordinate[0][0] = L.bottom_x;
	L.coordinate[0][1] = L.bottom_y;
	L.coordinate[1][0] = L.bottom_x;
	L.coordinate[1][1] = L.bottom_y - 1;
	L.coordinate[2][0] = L.bottom_x + 1;
	L.coordinate[2][1] = L.bottom_y - 1;
	L.coordinate[3][0] = L.bottom_x + 2;
	L.coordinate[3][1] = L.bottom_y - 1;
	L.center_x = L.coordinate[1][0];
	L.center_y = L.coordinate[1][1];
	
	J.bottom_x = 2;
	J.bottom_y = 1;
	J.coordinate[0][0] = J.bottom_x;
	J.coordinate[0][1] = J.bottom_y;
	J.coordinate[1][0] = J.bottom_x;
	J.coordinate[1][1] = J.bottom_y - 1;
	J.coordinate[2][0] = J.bottom_x - 1;
	J.coordinate[2][1] = J.bottom_y - 1;
	J.coordinate[3][0] = J.bottom_x - 2;
	J.coordinate[3][1] = J.bottom_y - 1;
	J.center_x = J.coordinate[1][0];
	J.center_y = J.coordinate[1][1];
	
	I.bottom_x = 0;
	I.bottom_y = 0;
	I.coordinate[0][0] = I.bottom_x;
	I.coordinate[0][1] = I.bottom_y;
	I.coordinate[1][0] = I.bottom_x + 1;
	I.coordinate[1][1] = I.bottom_y;
	I.coordinate[2][0] = I.bottom_x + 2;
	I.coordinate[2][1] = I.bottom_y ;
	I.coordinate[3][0] = I.bottom_x + 3;
	I.coordinate[3][1] = I.bottom_y;
	I.center_x = I.coordinate[1][0];
	I.center_y = I.coordinate[1][1];
	
	C1.bottom_x = 2;
	C1.bottom_y = 1;
	C1.coordinate[0][0] = C1.bottom_x;
	C1.coordinate[0][1] = C1.bottom_y;
	C1.coordinate[1][0] = C1.bottom_x - 1;
	C1.coordinate[1][1] = C1.bottom_y;
	C1.coordinate[2][0] = C1.bottom_x - 1;
	C1.coordinate[2][1] = C1.bottom_y - 1;
	C1.coordinate[3][0] = C1.bottom_x - 2;
	C1.coordinate[3][1] = C1.bottom_y - 1;
	C1.center_x = C1.coordinate[1][0];
	C1.center_y = C1.coordinate[1][1];
	
	C2.bottom_x = 0;
	C2.bottom_y = 1;
	C2.coordinate[0][0] = C2.bottom_x;
	C2.coordinate[0][1] = C2.bottom_y;
	C2.coordinate[1][0] = C2.bottom_x + 1;
	C2.coordinate[1][1] = C2.bottom_y;
	C2.coordinate[2][0] = C2.bottom_x + 1;
	C2.coordinate[2][1] = C2.bottom_y - 1;
	C2.coordinate[3][0] = C2.bottom_x + 2;
	C2.coordinate[3][1] = C2.bottom_y - 1;
	C2.center_x = C2.coordinate[1][0];
	C2.center_y = C2.coordinate[1][1];
	
	S.bottom_x = 0;
	S.bottom_y = 1;
	S.coordinate[0][0] = S.bottom_x;
	S.coordinate[0][1] = S.bottom_y;
	S.coordinate[1][0] = S.bottom_x;
	S.coordinate[1][1] = S.bottom_y - 1;
	S.coordinate[2][0] = S.bottom_x + 1;
	S.coordinate[2][1] = S.bottom_y - 1;
	S.coordinate[3][0] = S.bottom_x + 1;
	S.coordinate[3][1] = S.bottom_y;
	S.center_x = S.coordinate[1][0];
	S.center_y = S.coordinate[1][1];
	
	T.bottom_x = 1;
	T.bottom_y = 1;
	T.coordinate[0][0] = T.bottom_x;
	T.coordinate[0][1] = T.bottom_y;
	T.coordinate[1][0] = T.bottom_x;
	T.coordinate[1][1] = T.bottom_y - 1;
	T.coordinate[2][0] = T.bottom_x + 1;
	T.coordinate[2][1] = T.bottom_y - 1;
	T.coordinate[3][0] = T.bottom_x - 1;
	T.coordinate[3][1] = T.bottom_y - 1;
	T.center_x = T.coordinate[1][0];
	T.center_y = T.coordinate[1][1];
	
	
}

void shiftRight(){
	for(unsigned char a = 0; a < 4; a++){
		currentShape.coordinate[a][0]++;
	}
	currentShape.center_x++;
}

void shiftLeft(){
	for(unsigned char a = 0; a < 4; a++){
		currentShape.coordinate[a][0]--;
	}
	currentShape.center_x--;
}

void shiftDownFunct(){
	for(unsigned char a = 0; a < 4; a++){
		currentShape.coordinate[a][1]++;
	}
	currentShape.center_y++;
}

void shiftUp(){
	for(unsigned char a = 0; a < 4; a++){
		currentShape.coordinate[a][1]--;
	}
	currentShape.center_y--;
}

enum displayMatrixstates {Init1, display} displayState;
void displayMatrix(){
	unsigned char multiplex[] = {1, 2, 4, 8, 16, 32, 64, 128};
	switch(displayState){
		case Init1:
		displayState = display;
		break;
		
		case display:
		displayState = display;
		break;
		
		default:
		displayState = Init1;
		break;
	}
	
	switch(displayState){
		case Init1:
		break;
		
		case display:
		transmit_data(multiplex[i]);
		PORTC = (~(temps[i]));
		if(i > 6){
			i = 0;
		}
		else{
			i++;
		}
		break;
		
		default:
		break;
	}
	return;
}

enum shiftStatesDown {Init2, shiftDown} downState;
void ShiftDownTick(){
	switch(downState){
		case Init2:
		downState = shiftDown;
		break;
		
		case shiftDown:
		downState = shiftDown;
		break;
		
		default:
		downState = Init2;
		break;
	}
	
	switch(downState){
		case Init2:
		break;
		
		case shiftDown:
		if((checkShift & 0x02)){
			for(unsigned char a = 0; a < 4; a++){
				turnOffLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
				currentShape.coordinate[a][1]++;
			}
			currentShape.center_y++;
			for(unsigned char a = 0; a < 4; a++){
				turnOnLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
			}
		}
		break;
		
		default:
		break;
		
	}
	return;
}

enum checkShiftStates {Init3, checkShiftState} checkState;

void checkShiftTick(){
	unsigned char checkShiftRight = 0x00;
	unsigned char checkShiftDown = 0x00;
	unsigned char checkShiftLeft = 0x00;
	unsigned char tempShift = 0x00;
	switch(checkState){
		case Init3:
		checkState = checkShiftState;
		break;
		
		case checkShiftState:
		checkState = checkShiftState;
		break;
		
		default:
		checkState = Init3;
		break;
	}
	switch(checkState){
		case Init3:
		break;
		
		case checkShiftState:
		for(unsigned char a = 0; a < 4; a++){
			if(currentShape.coordinate[a][0] > 6){
				tempShift = 0x00;
			}
			else{
				if(isOn(currentShape.coordinate[a][0] + 1, currentShape.coordinate[a][1])){
					tempShift = 0x00;
					for(unsigned char b = 0; b < 4; b++){
						if(((currentShape.coordinate[a][0] + 1) == currentShape.coordinate[b][0]) && ((currentShape.coordinate[a][1]) == currentShape.coordinate[b][1])){
							tempShift = 0x01;
						}
					}
				}
				else{
					tempShift = 0x01;
				}
			}
			checkShiftRight = (checkShiftRight << 1) | tempShift;
		}
		if(checkShiftRight == 0x0F){
			checkShiftRight = 0x01;
		}
		else{
			checkShiftRight = 0x00;
		}
		
		tempShift = 0x00;
		
		for(unsigned char a = 0; a < 4; a++){
			if(currentShape.coordinate[a][1] > 6){
				tempShift = 0x00;
			}
			else{
				if(isOn(currentShape.coordinate[a][0], currentShape.coordinate[a][1] + 1)){
					tempShift = 0x00;
					for(unsigned char b = 0; b < 4; b++){
						if(((currentShape.coordinate[a][0]) == currentShape.coordinate[b][0]) && ((currentShape.coordinate[a][1] + 1) == currentShape.coordinate[b][1])){
							tempShift = 0x01;
						}
					}
				}
				else{
					tempShift = 0x01;
				}
			}
			checkShiftDown = (checkShiftDown << 1) | tempShift;
		}
		if(checkShiftDown == 0x0F){
			checkShiftDown = 0x01;
		}
		else{
			checkShiftDown = 0x00;
		}
		
		tempShift = 0x00;
		
		for(unsigned char a = 0; a < 4; a++){
			if(currentShape.coordinate[a][0] < 1){
				tempShift = 0x00;
			}
			else{
				if(isOn(currentShape.coordinate[a][0] - 1, currentShape.coordinate[a][1])){
					tempShift = 0x00;
					for(unsigned char b = 0; b < 4; b++){
						if(((currentShape.coordinate[a][0] - 1) == currentShape.coordinate[b][0]) && ((currentShape.coordinate[a][1]) == currentShape.coordinate[b][1])){
							tempShift = 0x01;
						}
					}
				}
				else{
					tempShift = 0x01;
				}
			}
			checkShiftLeft = (checkShiftLeft << 1) | tempShift;
		}
		if(checkShiftLeft == 0x0F){
			checkShiftLeft = 0x01;
		}
		else{
			checkShiftLeft = 0x00;
		}
		checkShift = (checkShiftLeft << 2) | (checkShiftDown << 1) | checkShiftRight;
		break;
		
		default:
		break;
	}
}

enum Random_GenerateStates {Init4, RandomGenerate, RandomGenerateWait, Moving_Down, WaitOnBottom} Random_GenerateState;
void Random_GenerateTick(){
	unsigned char randNum = 0;
	switch(Random_GenerateState){
		
		case Init4:
		Random_GenerateState = RandomGenerate;
		break;
		
		case RandomGenerate:
		Random_GenerateState = Moving_Down;
		break;
		
		case RandomGenerateWait:
		Random_GenerateState = RandomGenerate;
		break;
		
		case Moving_Down:
		if(checkShift & 0x02){
			Random_GenerateState = Moving_Down;
		}
		else if(!(checkShift & 0x02)){
			Random_GenerateState = WaitOnBottom;
		}
		break;
		
		case WaitOnBottom:
		if(waitTime < 1000 && !(checkShift & 0x02)){
			Random_GenerateState = WaitOnBottom;
		}
		else if(waitTime >= 1000 && !(checkShift & 0x02)){
			Random_GenerateState = RandomGenerateWait;
		}
		else if(checkShift & 0x02){
			Random_GenerateState = Moving_Down;
			waitTime = 0;
		}
		break;
		
		default:
		Random_GenerateState = Init4;
		break;
	}
	switch(Random_GenerateState){
		case Init4:
		break;
		
		case RandomGenerateWait:
		randomGenerated = 0x01;
		break;
		
		case RandomGenerate:
		randNum = rand() % 7;
		if(randNum == 0){
			setShape(L);
		}
		else if(randNum == 1){
			setShape(J);
		}
		else if(randNum == 2){
			setShape(I);
		}
		else if(randNum == 3){
			setShape(C1);
		}
		else if(randNum == 4){
			setShape(C2);
		}
		else if(randNum == 5){
			setShape(S);
		}
		else if(randNum == 6){
			setShape(T);
		}
		for(unsigned char a = 0; a < 4; a++){
			turnOnLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
		}
		break;
		
		case Moving_Down:
		randomGenerated = 0x00;
		waitTime = 0;
		break;
		
		case WaitOnBottom:
		waitTime++;
		break;
		
		default:
		break;
		
	}
	return;
	
}

enum shiftLRStates {Init5, WaitToShift, LeftShift, RightShift, LeftShiftWait, RightShiftWait} shiftLRState;

void shiftLRTick(){
	switch(shiftLRState){
		case Init5:
		shiftLRState = WaitToShift;
		break;
		
		case WaitToShift:
		if((move & 0x02) && (checkShift & 0x04)){
			shiftLRState = LeftShift;
		}
		else if((move & 0x01) && (checkShift & 0x01)){
			shiftLRState = RightShift;
		}
		else{
			shiftLRState = WaitToShift;
		}
		break;
		
		case LeftShift:
		if((move & 0x02) && (checkShift & 0x04)){
			shiftLRState = LeftShiftWait;
		}
		else{
			shiftLRState = WaitToShift;
		}
		break;
		
		case LeftShiftWait:
		if((move & 0x02) && (checkShift & 0x04) && (waitTimeShift < 10)){
			shiftLRState = LeftShiftWait;
		}
		else if((move & 0x02) && (checkShift & 0x04) && (waitTimeShift >= 10)){
			shiftLRState = LeftShift;
			waitTimeShift = 0;
		}
		else{
			shiftLRState = WaitToShift;
		}
		break;
		
		case RightShift:
		if((move & 0x01) && (checkShift & 0x01)){
			shiftLRState = RightShiftWait;
		}
		else{
			shiftLRState = WaitToShift;
		}
		break;
		
		case RightShiftWait:
		if((move & 0x01) && (checkShift & 0x01) && (waitTimeShift < 10)){
			shiftLRState = RightShiftWait;
		}
		else if((move & 0x01) && (checkShift & 0x01) && (waitTimeShift >= 10)){
			shiftLRState = RightShift;
			waitTimeShift = 0;
		}
		else{
			shiftLRState = WaitToShift;
		}
		break;
		
		default:
		shiftLRState = Init5;
		break;
	}
	switch(shiftLRState){
		case Init5:
		break;
		
		case WaitToShift:
		break;
		
		case LeftShift:
		for(unsigned char a = 0; a < 4; a++){
			turnOffLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
			currentShape.coordinate[a][0]--;
		}
		currentShape.center_x--;
		for(unsigned char a = 0; a < 4; a++){
			turnOnLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
		}
		break;
		
		case LeftShiftWait:
		waitTimeShift++;
		break;
		
		case RightShift:
		for(unsigned char a = 0; a < 4; a++){
			turnOffLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
			currentShape.coordinate[a][0]++;
		}
		currentShape.center_x++;
		for(unsigned char a = 0; a < 4; a++){
			turnOnLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
		}
		break;
		
		case RightShiftWait:
		waitTimeShift++;
		break;
		
		default:
		break;
	}
}

enum rotateStates {Init6, waitRotate, Rotate} rotateState;
void rotateTick(){
	unsigned char distanceFromCenter = 0;
	unsigned char holdCoordinatex = 0;
	unsigned char holdCoordinatey = 0;
	unsigned char tempCoord[4][2];
	unsigned char tempcenterx = 0;
	unsigned char tempcentery = 0;
	switch(rotateState){
		case Init6:
		rotateState = Rotate;
		break;
		
		case waitRotate:
		if((~PINA & 0x04)){
			rotateState = Rotate;
			for(unsigned char a = 0; a < 4; a++){
				turnOffLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
				tempCoord[a][0] = currentShape.coordinate[a][0];
				tempCoord[a][1] = currentShape.coordinate[a][1];
			}
			tempcenterx = currentShape.center_x;
			tempcentery = currentShape.center_y;
			for(unsigned char a = 0; a < 4; a++){
				holdCoordinatex = currentShape.coordinate[a][0];
				holdCoordinatey = currentShape.coordinate[a][1];
				
					
				if(holdCoordinatex > currentShape.center_x){
					distanceFromCenter = holdCoordinatex - currentShape.center_x;
					currentShape.coordinate[a][0] -= distanceFromCenter;
					currentShape.coordinate[a][1] += distanceFromCenter;
					while(!(currentShape.coordinate[a][1] <= 7 && currentShape.coordinate[a][1] >= 0) || isOn(currentShape.coordinate[a][0], currentShape.coordinate[a][1])){
						shiftUp();
						holdCoordinatey--;
					}
				}
				
				
				if(holdCoordinatex < currentShape.center_x){
					distanceFromCenter = currentShape.center_x - holdCoordinatex;
					currentShape.coordinate[a][0] += distanceFromCenter;
					currentShape.coordinate[a][1] -= distanceFromCenter;
					while(!(currentShape.coordinate[a][1] <= 7 && currentShape.coordinate[a][1] >= 0) || isOn(currentShape.coordinate[a][0], currentShape.coordinate[a][1])){
						shiftDownFunct();
						holdCoordinatey++;
					}
				}
				
				
				if(holdCoordinatey > currentShape.center_y){
					distanceFromCenter = holdCoordinatey - currentShape.center_y;
					currentShape.coordinate[a][0] -= distanceFromCenter;
					currentShape.coordinate[a][1] -= distanceFromCenter;
					while(!(currentShape.coordinate[a][0] <= 7 && currentShape.coordinate[a][0] >= 0) || isOn(currentShape.coordinate[a][0], currentShape.coordinate[a][1])){
						shiftRight();
					}
				}
				
				
				if(holdCoordinatey < currentShape.center_y){
					distanceFromCenter = currentShape.center_y - holdCoordinatey;
					currentShape.coordinate[a][0] += distanceFromCenter;
					currentShape.coordinate[a][1] += distanceFromCenter;
					while(!(currentShape.coordinate[a][0] <= 7 && currentShape.coordinate[a][0] >= 0) || isOn(currentShape.coordinate[a][0], currentShape.coordinate[a][1])){
						shiftLeft();
					}
				}
			}
			
			holdCoordinatex = 0;
			
			for(unsigned char a = 0; a < 4; a++){
				if(isOn(currentShape.coordinate[a][0], currentShape.coordinate[a][1]) || !(currentShape.coordinate[a][1] <= 7 && currentShape.coordinate[a][1] >= 0) || !(currentShape.coordinate[a][0] <= 7 && currentShape.coordinate[a][0] >= 0)){
					holdCoordinatex = 1;
				}
			}
			
			if(holdCoordinatex){
				for(unsigned char a = 0; a < 4; a++){
					currentShape.coordinate[a][0] = tempCoord[a][0];
					currentShape.coordinate[a][1] = tempCoord[a][1];
				}
				currentShape.center_x = tempcenterx;
				currentShape.center_y = tempcentery;
			}
			
			for(unsigned char a = 0; a < 4; a++){
				turnOnLED(currentShape.coordinate[a][0], currentShape.coordinate[a][1]);
			}
		}
		else{
			rotateState = waitRotate;
		}
		break;
		
		case Rotate:
		if(!(~PINA & 0x04)){
			rotateState = waitRotate;
		}
		else{
			rotateState = Rotate;
		}
		break;
		
		default:
		rotateState = Init6;
		break;
	}
}

enum states {start, wait, x_axis, y_axis} state;
unsigned short movx;
void tick()
{
	switch(state){
		case start:
		state = wait;
		break;
		
		case wait:
		if (ADMUX == 0x40)
		{
			
			state = x_axis;
		}
		else if (ADMUX ==0x41)
		{
			state = y_axis;
		}
		
		else{state = wait;}
		break;
		
		case x_axis:
		state = wait;
		break;
		
		case y_axis:
		state = wait;
		break;
		
		default:
		state = start;
		break;
		
		
	}
	switch(state)
	{
		case start:
		break;
		
		case wait:
		break;
		
		case x_axis:
		convert();
		movx = ADC;
		if(receivedNum & 0x02){
			move = 0x00;
		}
		else if(!(receivedNum & 0x02)){
			if (movx > 712)
			{
				move = 0x01;
			}
			
			else if (movx < 312)
			{
				move = 0x02;
			}
			else{
				move = 0x00;
			}
		}
		ADMUX = 0x41;// initialize value of ADMUX to switch over to next pin
		break;
		
		case y_axis:
		convert();
		movx = ADC;
		if(receivedNum & 0x02){
			move = 0x00;
		}
		else if(!(receivedNum & 0x02)){
			if (movx > 812)
			{
				move = 0x04;
			}
			else{
				move = 0x00;
			}
		}
		ADMUX = 0x40;
		break;
		
		default:
		break;
		
	}
}

enum ridLinesStates {Init7, waitRidLines} ridLinesState;

void ridLinesTick(){
	unsigned char shift = 0;
	unsigned char ridLinesTrue = 0;
	unsigned char numLinescleared = 0;
	switch(ridLinesState){
		case Init7:
		ridLinesState = waitRidLines;
		break;
		
		case waitRidLines:
		do {
			shift = 0;
			ridLinesTrue = 0;
			for(unsigned char a = 0; a < 8; a++){
				if(temps[a] == 0xFF && randomGenerated){
					ridLinesTrue = 0x01;
					ridLinesTrue = ridLinesTrue << a;
					numLinescleared++;
				}
			}
			while(!(ridLinesTrue == 0x01) && !(ridLinesTrue == 0x00)){
				ridLinesTrue = ridLinesTrue >> 1;
				shift++;
			}
			if(ridLinesTrue){
				temps[shift] = 0;
			}
			while(shift > 0){
				temps[shift] = temps[shift - 1];
				shift--;
			}
			if(ridLinesTrue){
				temps[0] = 0;
				lineNum++;
				if(numLinescleared == 2){
					sendData(2);
				}
				if(lineNum == 7){
					sendData(4);
				}
				if((lineNum % 3) == 0){
					for(unsigned a = 0; a < 8; a++){
						temps[a] = 0;
					}
				}
				if(lineNum > 9){
					lineNum = 0;
				}
			}
		} while(ridLinesTrue);
		numLinescleared = 0;
		break;
		
		default:
		ridLinesState = Init7;
		break;
	}
	
	switch(ridLinesState){
		case Init7:
		break;
		
		case waitRidLines:
		break;
		
		default:
		break;
	}
}

enum SEGStates {Init8, SEGdisplay} SEGState;

void SEGTick(){
	switch(SEGState){
		case Init8:
		SEGState = SEGdisplay;
		break;
		
		case SEGdisplay:
		SEGState = SEGdisplay;
		break;
		
		default:
		SEGState = Init8;
		break;
	}
	
	switch(SEGState){
		case Init8:
		break;
		
		case SEGdisplay:
		if(lineNum == 0){
			transmit_data2(0xC0);
		}
		else if(lineNum == 1){
			transmit_data2(0xF9);
		}
		else if(lineNum == 2){
			transmit_data2(0xA4);
		}
		else if(lineNum == 3){
			transmit_data2(0xB0);
		}
		else if(lineNum == 4){
			transmit_data2(0x99);
		}
		else if(lineNum == 5){
			transmit_data2(0x92);
		}
		else if(lineNum == 6){
			transmit_data2(0x82);
		}
		else if(lineNum == 7){
			transmit_data2(0xF8);
		}
		else if(lineNum == 8){
			transmit_data2(0x80);
		}
		else if(lineNum == 9){
			transmit_data2(0x98);
		}
		else{
			transmit_data2(0xBF);
		}
		
		default:
		break;
	}
}
enum RecieveStates {Init9, Receive} RecieveState;
void RecieveTick(){
	switch(RecieveState){
		case Init9:
		RecieveState = Receive;
		break;
		
		case Receive:
		RecieveState = Receive;
		break;
		
		default:
		RecieveState = Init9;
		break;
	}
	
	switch(RecieveState){
		case Init9:
		break;
		
		case Receive:
		if(USART_HasReceived(0)){
			receivedNum |= recieveData();
		}
		break;
		
		default:
		break;
	}
}

enum controlStates {Init10, controlWait, disable} controlState;
void controlTick(){
	switch(controlState){
		case Init10:
		controlState = controlWait;
		break;
		
		case controlWait:
		if(receivedNum & 0x02){
			controlState = disable;
			controlTimer = 0;
		}
		break;
		
		case disable:
		if(controlTimer >= 60){
			controlState = controlWait;
			receivedNum &= 0xFD;
		}
		break;
		
		default:
		controlState = Init10;
		break;
	}
	
	switch(controlState){
		case Init10:
		break;
		
		case controlWait:
		break;
		
		case disable:
		controlTimer++;
		break;
		
		default:
		break;
	}
}

enum randLEDStates {Init11, randLEDWait, randlED} randLEDState;
void randLEDTick(){
	unsigned char turnedOn = 0;
	unsigned char randx = 0;
	unsigned char randy = 0;
	switch(randLEDState){
		case Init11:
		randLEDState = randLEDWait;
		break;
		
		case randLEDWait:
		if(receivedNum & 0x04){
			randLEDState = randlED;
		}
		break;
		
		case randlED:
		randLEDState = randLEDWait;
		receivedNum &= 0xFB;
		break;
		
		default:
		randLEDState = Init11;
		break;
	}
	
	switch(randLEDState){
		case Init11:
		break;
		
		case randLEDWait:
		break;
		
		case randlED:
 		for(unsigned char a = 0; a < 3; a++){
			 turnedOn = 0;
 			while(!turnedOn){
 				randx = (rand() % 4) + 4;
 				randy = rand() % 8;
				if(!isOn(randx, randy)){
					turnOnLED(randx, randy);
					turnedOn = 1;
 				}
 			}
		}
		break;
		
		default:
		break;
	}
}


int main(void)
{
	DDRA = 0x00;
	PORTA = 0xFF;
	DDRC = 0xFF;
	PORTC = 0xFF;
	DDRB = 0xFF;
	PORTB = 0x00;
	DDRD = 0xFF;
	PORTD = 0xFF;
	
	
	srand(10);
	
	unsigned long shiftDownTime = 0;
	unsigned char shiftLRTime = 0;
	unsigned char rotateTime = 0;
	unsigned char SEGTime = 0;
	unsigned char controlTickTime = 0;
	unsigned char randLEDTime = 0;
	
	InitADC();
	
	TimerOn();

	TimerSet(1);
	
	ShapesInit();
	
	initUSART(0);
	
	initUSART(1);
	
	
    while(1)
    {
		
		if(~PINA & 0x08){
			startGame = 1;
			sendData(1);
		}
		
		if(startGame){
			Random_GenerateTick();
			
			checkShiftTick();
			
			if(shiftDownTime >= 1000){
				ShiftDownTick();
				shiftDownTime = 0;
				
			}
			
			tick();
			
			if(shiftLRTime >= 50){
				shiftLRTick();
				shiftLRTime = 0;
			}
			
			
			ridLinesTick();
			
			if(rotateTime >= 50){
				rotateTick();
				rotateTime = 0;
			}
			
			if(SEGTime >= 200){
				SEGTick();
				SEGTime = 0;
			}
			
			if(randLEDTime >= 100){
				randLEDTick();
				randLEDTime = 0;
			}
			
			displayMatrix();
			
			RecieveTick();
			
			if(controlTickTime >= 100){
				controlTick();
				controlTickTime = 0;
			}
			
			while(!TimerFlag){};
			
			TimerFlag = 0;
			
			shiftDownTime ++;
			if(move & 0x04){
				shiftDownTime += 10;
			}
			shiftLRTime ++;
			rotateTime ++;
			SEGTime++;
			controlTickTime++;
			randLEDTime++;jk
		}
        //TODO:: Please write your application code 
    }
}


// link to cs120b video https://www.youtube.com/watch?v=5r3Lc5_FlvA
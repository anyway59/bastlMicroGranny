/* 
	
mg2HW.cpp library
this library is to be used with standuino MicroGranny 2.0 hardware

documentation of the library you find here:
mg2HW library

Created by Vaclav Pelousek 2013 www.pelousek.eu
for Standuino wwww.standuino.eu

*/


#include "Arduino.h"
#include <avr/pgmspace.h>

#define SHIFTREGISTER_SER  C,5
#define SHIFTREGISTER_RCK  B,1
#define SHIFTREGISTER_SRCK B,0
  
 #define BUT_PIN_BAM D,6
 #define BUT_PIN_BAM_2 D,7
  
#include <shiftRegisterFast.h>
//#include <portManipulations.h>
#include "mg2HW.h"

const unsigned char rowPin[NUMBER_OF_ROWS]={DIGIT_1_PIN,DIGIT_2_PIN,DIGIT_3_PIN,DIGIT_4_PIN,LED_PIN,LED_2_PIN};
//const unsigned char row[5]={ ROW_1_PIN,ROW_2_PIN,ROW_3_PIN,ROW_4_PIN,ROW_5_PIN };
const unsigned char rgbPin[3]={LED_R_PIN,LED_G_PIN,LED_B_PIN};

const char knobPin[NUMBER_OF_KNOBS] PROGMEM ={KNOB_PIN_1,KNOB_PIN_2,KNOB_PIN_3,KNOB_PIN_4};


/*
Segments
 -  A
 F / / B
 -  G
 E / / C
 -  D
 */
 
const unsigned char segments[8]={
  SEG_A,SEG_B,SEG_C,SEG_D,SEG_E,SEG_F,SEG_G,SEG_DOT};
  
  
#define BLACK_BITS 0
#define RED_BITS 1
#define GREEN_BITS 2
#define BLUE_BITS 4
#define YELLOW_BITS 3
#define MAGENTA_BITS 5
#define CIAN_BITS 6
#define WHITE_BITS 7


const char colorBit[NUMBER_OF_COLORS] PROGMEM = {  

BLACK_BITS, RED_BITS,GREEN_BITS,BLUE_BITS,YELLOW_BITS,MAGENTA_BITS,CIAN_BITS,WHITE_BITS

};
enum BitOrder {LSB_FIRST,MSB_FIRST};
const char typo[40] PROGMEM ={
  B00111111, //0
  B00000110, //1
  B01011011, //2
  B01001111, //3
  B01100110, //4
  B01101101, //5
  B01111101, //6
  B00000111, //7
  B01111111, //8
  B01101111, //9
  B01110111, //A 10
  B01111100, //b
  B01011000, //c
  B01011110, //d
  B01111001, //e
  B01110001, //f
  B00111101, //g
  B01110100, //h
  B00000100, //i
  B00011110, //j 
  B01110000, //k 20
  B00111000, //l
  B01010101, //m
  B01010100, //n
  B01011100, //o 
  B01110011, //p 25
  B01100111, //q
  B01010000, //r
  B01101101, //s //tu memit 
  B01111000, //t 
  B00011100, //u 30
  B00001100, //v 31
  B01101010, //w
  B01001001, //x 
  B01110010, //y 
  B01011011, //z tu menit 35
  B00000000, // void 36
  B01000000, //37
  B01001001, //38
  B01010010, //39 slash
};



mg2HW::mg2HW(){

}

//############# ROUTINE FUNCTIONS #############

void mg2HW::initialize(){ 
	pinMode(DATA_PIN,OUTPUT);
	pinMode(CLOCK_PIN,OUTPUT);
	pinMode(LATCH_PIN,OUTPUT);
	pinMode(BUTTON_PIN,INPUT_PULLUP);
	pinMode(BUTTON_2_PIN,INPUT_PULLUP);
	//pinMode(A2,OUTPUT);

}

void mg2HW::update(){
	updateKnobs();
	updateButtons();
	updateDisplay(); 
}



//############# KNOB RELATED FUNCTIONS #############
/*
void mg2HW::setFreezeType(unsigned char _TYPE){

//	if(_TYPE==DEFAULT) unfreezeExternaly=false;
//	else if(_TYPE==UNFREEZE_EXTERNALY) unfreezeExternaly=true;

}
*/
//update values and hashes of knobs

 
  
  
void mg2HW::updateKnobs(){

	//knobChangedHash = 0;
  for (uint8_t i = 0; i < NUMBER_OF_KNOBS; i++) {
  	lastKnobValues[i]=knobValues[i];
  	
    int newValue = analogRead(pgm_read_word_near(knobPin + i));
     
    int distance = abs(newValue - knobValues[i]); 
    
   /*
    if(!unfreezeExternaly){
    	if (bitRead(knobFreezedHash, i) == true) {
    	  if (distance > KNOB_FREEZE_DISTANCE) {
     	   bitWrite(knobFreezedHash, i, false);
     	   bitWrite(knobChangedHash, i, true);
    	  }
  	  } 
    }
    
   	if (abs(newValue - knobValues[i]) > KNOB_TOLERANCE) {
      bitWrite(knobChangedHash, i, true);
    }
    */
   // lastKnobValues[i]=knobValues[i];
    knobValues[i] = newValue;
    //delay(10); ///
    updateDisplay();
  }
  
}


//returns the freezing state of knob	
boolean mg2HW::knobFreezed(unsigned char _KNOB){ 
	return bitRead(knobFreezedHash,_KNOB);
}
/*
boolean mg2HW::knobMoved(unsigned char _KNOB){ 
	return bitRead(knobChangedHash,_KNOB);
}
*/
// freeze all knobs
void mg2HW::freezeAllKnobs(){ 
	for(uint8_t i=0;i<NUMBER_OF_KNOBS;i++){
		bitWrite(knobFreezedHash,i,true);
	}
}

// unfreeze all knobs
void mg2HW::unfreezeAllKnobs(){ 
	for(uint8_t i=0;i<NUMBER_OF_KNOBS;i++){
		bitWrite(knobFreezedHash,i,false);
	}
}


// freeze one knob
void mg2HW::freezeKnob(unsigned char _KNOB){ 
	bitWrite(knobFreezedHash,_KNOB,true);
}

// unfreeze one knob
void mg2HW::unfreezeKnob(unsigned char _KNOB){ 
		bitWrite(knobFreezedHash,_KNOB,false);
}


// get knob value
int mg2HW::knobValue(unsigned char _KNOB){ 

	return knobValues[_KNOB];

}

// get last knob value
int mg2HW::lastKnobValue(unsigned char _KNOB){ 

	return lastKnobValues[_KNOB];

}

void mg2HW::setLastKnobValue(unsigned char _KNOB,int _val){ 

	lastKnobValues[_KNOB]=_val;

}

//############# LED RELATED FUNCTIONS #############

// write the values from the hash to the pins
//bitRead(ledStateHash,i)


// set state of led

void mg2HW::setLed(unsigned char _LED,boolean _STATE){ 

	if(_LED<8) bitWrite(displayBuffer[LED_ROW],_LED,_STATE);
	else bitWrite(displayBuffer[LED_2_ROW],_LED-8,_STATE);

}

void mg2HW::setColor(unsigned char _COLOR){ 

	unsigned char _bits=pgm_read_word_near(colorBit + _COLOR)	;
	
	for(uint8_t i=0;i<3;i++){
		setLed(rgbPin[i],bitRead(_bits,i));
	}

}

void mg2HW::displayText(char *text){
  for(uint8_t i=0;i<NUMBER_OF_DIGITS;i++) displayChar(text[i],i);
}

void mg2HW::displayChar(char whichChar,unsigned char _digit) {
  if(whichChar==32)
  { 
    lightNumber(VOID,_digit);
  }
  else if(whichChar<65){
    lightNumber(whichChar-48,_digit);
  }
  else if(whichChar<97){
    lightNumber(whichChar-65+10,_digit);
  }
  else {
    lightNumber(whichChar-97+10,_digit);
  }
  
}


void mg2HW::lightNumber(int numberToDisplay, unsigned char _digit) {

  for(uint8_t i=0;i<7;i++){
    bitWrite(displayBuffer[_digit],segments[i],bitRead(pgm_read_word_near(typo+numberToDisplay),i)); 
  }

}

void mg2HW::displayNumber(int _number){
 
  lightNumber(_number/100,1);
  lightNumber((_number%100)/10,2);
  lightNumber(_number%10,3);

}

void mg2HW::setDot(unsigned char _dot, boolean _state){
	bitWrite(displayBuffer[_dot],SEG_DOT,_state);
}

//############# BUTTON RELATED FUNCTIONS #############
/*
void mg2HW::updateButtons(){ 
updateButtons();
}
*/
void mg2HW::updateDisplay(){
//inline void updateDisplay(){
  if(++_row >= NUMBER_OF_ROWS) _row=0;
  
	
  
  	//digitalWrite(LATCH_PIN,LOW); 
  	shiftRegFast::write_8bit(1<<rowPin[_row]);
  	
  	shiftRegFast::write_8bit(~displayBuffer[_row]);
  	shiftRegFast::enableOutput();
   // digitalWrite(LATCH_PIN,HIGH);
  

}
/*
void updateDisplay(){
  if(++mg2HW::_row >= NUMBER_OF_ROWS) _row=0;
  
	
  
  	digitalWrite(LATCH_PIN,LOW); 
  	shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,1<<rowPin[mg2HW::_row]);
    shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,~mg2HW::displayBuffer[mg2HW::_row]);//analogRead(4)>>_row);//);
    
    digitalWrite(LATCH_PIN,HIGH);
  

}
*/



// updates all button related hashes
void mg2HW::updateButtons(){  

//	justReleasedHash=0;
//    justPressedHash=0;
    
	for(uint8_t i=0;i<8;i++){ // first read the buttons and update button states
	
		unsigned char whichButton=1<<i;;
		
		
	   // digitalWrite(LATCH_PIN,LOW); 
	    shiftRegFast::write_8bit(0);
	    shiftRegFast::write_8bit(~whichButton);
	    shiftRegFast::enableOutput();
	   // shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,0);
	    //shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,~whichButton);
	    
	    
	   // digitalWrite(LATCH_PIN,HIGH);
    	delayMicroseconds(5); //delete?
		
		//pinMode(pgm_read_word_near(buttonPins + i), INPUT_PULLUP);
		
		
		//bitWrite(buttonStateHash,i,!digitalRead(BUTTON_PIN));//bit_read_in(BUT_PIN));
		bitWrite(buttonStateHash,i,!bit_read_in(BUT_PIN_BAM));//bit_read_in(BUT_PIN)); 

		//bitWrite(buttonStateHash,i+8,!digitalRead(BUTTON_2_PIN));//bit_read_in(BUT_PIN_2));
		bitWrite(buttonStateHash,i+8,!bit_read_in(BUT_PIN_BAM_2));
		
		// and now update all the other hashes
	 	bitWrite(justPressedHash,i,false); 
		bitWrite(justReleasedHash,i,false);
    	if(bitRead(buttonStateHash,i)==true && bitRead(lastButtonStateHash,i)==false)  bitWrite(justPressedHash,i,true);
    	if(bitRead(buttonStateHash,i)==false && bitRead(lastButtonStateHash,i)==true)  bitWrite(justReleasedHash,i,true);
		bitWrite(lastButtonStateHash,i,bitRead(buttonStateHash,i));  
		
		// and now update all the other hashes
	 	bitWrite(justPressedHash,i+8,false); 
		bitWrite(justReleasedHash,i+8,false);
    	if(bitRead(buttonStateHash,i+8)==true && bitRead(lastButtonStateHash,i+8)==false)  bitWrite(justPressedHash,i+8,true);
    	if(bitRead(buttonStateHash,i+8)==false && bitRead(lastButtonStateHash,i+8)==true)  bitWrite(justReleasedHash,i+8,true);
		bitWrite(lastButtonStateHash,i+8,bitRead(buttonStateHash,i+8));  
		
	}
 
}

void mg2HW::dimForRecord(unsigned char _BUTTON){ 
	unsigned char whichButton=1<<_BUTTON;
	bitWrite(whichButton,REC,1);
	//digitalWrite(LATCH_PIN,LOW); 
    //shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,1<<rowPin[LED_ROW]);	    
    //shiftOut(DATA_PIN,CLOCK_PIN,LSBFIRST,~whichButton);
    shiftRegFast::write_8bit(1<<rowPin[LED_ROW]);
    shiftRegFast::write_8bit(~whichButton);
    shiftRegFast::enableOutput();
   // digitalWrite(LATCH_PIN,HIGH);
}

//returns current state of a button
boolean mg2HW::buttonState(unsigned char _BUTTON){ 
	return bitRead(buttonStateHash,_BUTTON);
}

//returns true if the button was just pressed
boolean mg2HW::justPressed(unsigned char _BUTTON){

	return bitRead(justPressedHash,_BUTTON);

}

//returns true if the button was just released
boolean mg2HW::justReleased(unsigned char _BUTTON){
	return bitRead(justReleasedHash,_BUTTON);
}



//flips the software switch
void mg2HW::flipSwitch(unsigned char _SWITCH){ 
	bitWrite(switchStateHash,_SWITCH,!bitRead(switchStateHash,_SWITCH));
}

// sets switch state
void mg2HW::setSwitch(unsigned char _SWITCH, boolean _STATE){ 
	bitWrite(switchStateHash,_SWITCH,_STATE);
}

//returns switch state
boolean mg2HW::switchState(unsigned char _SWITCH){ 
	return bitRead(switchStateHash,_SWITCH);
}

//resetsSwitches
void mg2HW::resetSwitches(){
	for(uint8_t i=0;i<NUMBER_OF_BUTTONS;i++){
		bitWrite(switchStateHash,i,false);
	}
}

//use switch states as bits of one number - sound
unsigned char mg2HW::soundFromSwitches(){
	unsigned char val=0;
	for(uint8_t i=0;i<4;i++){
		bitWrite(val,i,bitRead(switchStateHash,i));
	}
	return val;

}

//use button states as bits of one number - sound
unsigned char mg2HW::soundFromButtons(){
	unsigned char val=0;
	for(uint8_t i=0;i<4;i++){
		bitWrite(val,i,bitRead(buttonStateHash,i));
	}
	return val;
}



/*
#define CLEAR_TIMES 8
#define CLEAR_DELAY 50
boolean mg2HW::factoryClear(){
	
	pinMode(FACTORY_CLEAR_PIN,INPUT_PULLUP);
	pinMode(FACTORY_CLEAR_SIGNAL_PIN,OUTPUT);
	if(!digitalRead(FACTORY_CLEAR_PIN)){	 
 		for(int i=ZERO;i<CLEAR_TIMES;i++){
		  digitalWrite(FACTORY_CLEAR_SIGNAL_PIN,HIGH);
		  delay(CLEAR_DELAY);
		 digitalWrite(FACTORY_CLEAR_SIGNAL_PIN,LOW);
		  delay(CLEAR_DELAY);
  		}
  	return true;
	}
  else return false;
	 
}

void mg2HW::factoryCleared(){
	for(int i=ZERO;i<CLEAR_TIMES;i++){
		  	digitalWrite(FACTORY_CLEAR_SIGNAL_PIN,HIGH);
		  	delay(CLEAR_DELAY);
	  		digitalWrite(FACTORY_CLEAR_SIGNAL_PIN,LOW);
	  		delay(CLEAR_DELAY);
  		}
}
*/
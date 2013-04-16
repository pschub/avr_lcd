/*
    LCD Driver for ATMEGA 329p 
    Copyright (C) 2013 Patrick Schubert 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Uses PORTD6 for R/nW pin
 *      PORTC[0:3] for data pins
 *      PORTC[4] for RS pin
 *      PORTC[5] for EN pin
 *
 * Uses some delays for lcd startup timing. Needs instruction cycle
 * of 4MHz
 *
 * Patrick Schubert
 * Autumn 2012
 * 
*/

#include <avr/io.h>
#include "lcdDriver.h"
#include <util/delay.h>


#define HI_NYBBLE 0xF0
#define LO_NYBBLE 0x0F

//input registers of the lcd:
#define INSTR_REG 0  //instruction register
#define DATA_REG 1   //data register

#define STARTUP_DELAY 150//30000 //delay for 15ms on lcd startup
#define CLEAR_DELAY 40//3000 //delay for clearing the screen

//instruction codes
#define CLEAR 0x1 //clears display of data
#define DISPLAY_ON 0x0F //0x0F for blinking cursor, 0x0C for none
#define DISPLAY_OFF 0x8 //turns off lcd screen
#define SHIFT_RIGHT 0x14 //shifts cursor right
#define SHIFT_LEFT 0x10 //shifts cursor left
#define FUNC_SET 0x28 //sets lcd to 4-bit mode, 2-line display, with 5x8 font

//internal functions:
static void enablePulse(void);
static void write(unsigned char command, unsigned char regSelect);
static void lcdWait(void);


/*
 * Initialize lcd. This MUST be run before any other commands!
 */
void lcdInit(){
    RW_DDR |= P_RW;
    RS_DDR |= P_RS;
    EN_DDR |= P_EN;
    DATA_DDR |= (0xF << DATA_OFFSET); 

    RS_PORT &= ~P_RS;
    RW_PORT &= ~P_RW;

    _delay_ms(15);                      //15ms delay for lcd warmup

    DATA_BUS = (DATA_BUS & HI_NYBBLE)  |  (0x3); //func set 1 then wait 4.1ms
    enablePulse();

    _delay_us(4100);
    DATA_BUS = (DATA_BUS & HI_NYBBLE)  |  (0x3); //func set 2 then wait 100us
    enablePulse();
    
    DATA_BUS = (DATA_BUS & HI_NYBBLE)  |  (0x3); //func set 3 then wait ??
    enablePulse();
    _delay_us(100);

    DATA_BUS = (DATA_BUS & HI_NYBBLE)  |  (0x2); //func set for 4-bit mode
    enablePulse();
    _delay_us(100);

    write(FUNC_SET, INSTR_REG);        //func set 4-bit, 2 line mode
    _delay_us(100);

    lcdSetCursor(0,0);
    lcdClear();

    write(DISPLAY_ON, INSTR_REG);      //turn on the screen
    lcdWait();
    return;
}

/*
 * Takes a ptr to a null-terminated char array (aka a string!)
 * and prints to the lcd.
 */
void lcdWrite(const char* stringPtr){
    while(*stringPtr){  
        write(*stringPtr++, DATA_REG);
        lcdWait();
    }
    return;
}

/*
 * Moves the cursor to (x,y) in the display. Upper left is (0,0)
 */
void lcdSetCursor(unsigned char col, unsigned char row){
    unsigned char command;
    command = (0x01<<7)|(row<<6);
    command = command|(col);
    write(command, INSTR_REG);
    lcdWait();
    return;
}

/*
 * Returns the cursor to the home position on the lcd screen
 * Note: Interestingly, giving the 'return' command to the lcd
 * takes a lot longer (up to 1.5ms) to compute than telling it to set the cursor
 * to (0,0). Weird.
 */
void lcdReturn(void){
    lcdSetCursor(0,0);
    return;
}

/*
 * Clears the lcd screen
 */
void lcdClear(void){
    write(CLEAR, INSTR_REG);
    lcdWait();
    return;
}

/*
 * Turns display on/off
 */
void lcdToggler(unsigned char on){
    if (on){
        write(DISPLAY_ON, INSTR_REG);
    }
    else {
        write(DISPLAY_OFF, INSTR_REG);
    }
    return;
}

/*
 * Writes one byte to the display
 */
void lcdWriteByte(char character){
    write(character, 1);
    lcdWait();
    return;
}

/*
 * lcdWriteNum takes an int and prints the last five digits plus
 * a negative sign if necessary. Digit positions to the left of
 * the ten-thousands place are omitted. (good for up to 16-bit numbers!)
 */
void lcdWriteNum(int number){
    unsigned char i=0;
    unsigned char j;
    unsigned char negative = (number<0); //test for negative
    unsigned char numString[7] = "      ";
    unsigned char outString[7];
    j = negative;
    if (negative){
        number = number*-1;  //get rid of sign for math purposes
    }
     //pick apart number digit by digit
    numString[0] = (number%100000)/10000 + '0';  //and store in numString
    numString[1] = (number%10000)/1000 + '0';
    numString[2] = (number%1000)/100 + '0';
    numString[3] = (number%100)/10 + '0';
    numString[4] = (number%10)   + '0';

    while(numString[i]=='0'){ //find where first digit starts
        i++;                  //ie, get rid of leading zeros
    }
    if (i==5){ //if all were zeros, we still want to print at least one.
        i = 4;
    }

    while(numString[i]!= ' '){
        outString[j] = numString[i];
        j++;i++;
    }
    outString[j] = '\0'; //add terminating null character
    if (negative){
        outString[0] = '-'; //add negative sign if necessary
    }

    lcdWrite(outString); //print to lcd
    return;
}

/*
 *   STATIC FUNCTIONS
 */

/*
 * Writes a command or a character to the lcd.
 * If a character, regSelect = 1
 * If an instruction, regSelect = 0
 */
static void write(unsigned char command, unsigned char regSelect){
    if (regSelect)
        RS_PORT |= P_RS;
    else
        RS_PORT &= ~P_RS;

    DATA_BUS = (DATA_BUS & HI_NYBBLE)  |  (command >>  4); //xmt upper nibble
    enablePulse();

    DATA_BUS = (DATA_BUS & HI_NYBBLE) | (command & LO_NYBBLE); //xmt lower nibble
    enablePulse();
    return;
}

/* Sends a pulse to the lcd, signaling that there's
 * new data to be loaded. The lcd loads the data on the falling edge
 */
static void enablePulse(){
    asm volatile ("nop");
    EN_PORT |= P_EN;
    asm volatile ("nop");
    EN_PORT &= ~P_EN;
    return;
}

/*
 * Reads busy flag in lcd and waits until cleared
 */
static void lcdWait(void){
    unsigned char busy = 1;
    DATA_BUS |= 0x0F; //set pullups
    DATA_DDR &= 0xF0; //set input
    RW_PORT |= P_RW; //set READ
    RS_PORT &= ~(P_RS ); //set instruction
    while (busy){
        EN_PORT |= P_EN;
        asm volatile ("nop");
        busy = DATA_PINS & BUSY_FLAG; //check busy flag
        EN_PORT &= ~P_EN;
        enablePulse();   //cycle out lower byte
    }
    RW_PORT &= ~P_RW;
    DATA_DDR |= (0x0F);
    return;
}


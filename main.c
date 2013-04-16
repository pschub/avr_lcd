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
lcd demo program
*/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include "lcdDriver.h"


//function prototypes
void setupUC(void);


int main(void)
{
    setupUC();
    lcdInit();

    lcdWrite("catapus");
    lcdWriteNum(123);
    lcdWriteByte('@');
    for(;;);

    return 0;
}

//initializes uC. 
void setupUC(void){
  //debug pin D0 (pin 2)
  DDRD |= (1<<1)|(1<<0);    //set as output
  PORTD |= (1<<1)|(1<<0);        //set hi
  
}



/*
lcdDemo
Patrick Schubert
Started March 29, 2013

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



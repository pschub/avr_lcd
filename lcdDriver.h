#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#ifndef F_CPU
  #define F_CPU 8000000
#endif

/*
 * P_RW is on PORTD
 * P_EN and P_RS are on PORTC
 * DATA BUS is on the lower 4 bits of PORTC
 * Note that this 4bit connects to the *upper* 4 bits of lcd
*/

#define RW_PORT PORTB
#define RW_DDR DDRB
#define P_RW (1<<1)

#define EN_PORT PORTB
#define EN_DDR DDRB
#define P_EN (1<<2)

#define RS_PORT PORTB
#define RS_DDR DDRB
#define P_RS (1<<0)

#define DATA_BUS PORTC
#define DATA_PINS PINC
#define DATA_DDR DDRC
#define DATA_OFFSET 0
#define BUSY_FLAG (1<<3)


void lcdInit(void); //lcdInit must be run before any other lcd functions are called!
void lcdReturn(void); //returns the cursor to 0,0
void lcdClear(void); //clears the screen of data
void lcdWrite(const char* stringPtr); //writes a string to the screen
void lcdWriteByte(char character);
void lcdWriteNum(int number); //writes a 4 digit number to screen
void lcdSetCursor(unsigned char row, unsigned char col); //moves the cursor to (x,y) on the screen
void lcdToggler(unsigned char on); //turns display on/off

#endif


// Title        : LCDinterface
// Software     : WINAVR GCC
// Version      : 1.0
// Date Created : 17.03.2013
// Last Update  : 30.03.2013
// Target       : ATTiny2313
// Hardware requirements: LCD display 2*16, ATTinyboard2313
// Description  : 4-bit LCD Interface

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

// used pins on port B
#define LCD_DB4 0    // PORTB.0 
#define LCD_DB5 1    // PORTB.1
#define LCD_DB6 2    // PORTB.2 
#define LCD_DB7 3    // PORTB.3
#define LCD_E 1     // PORTB.6 Enable 
#define LCD_RS 0     // PORTB.7 Register Select


//LCD commands
#define LCD_CLR         0x01    // clear display
#define LCD_HOME        0x02    // return home

#define LCD_INC         0x06    // Increment, display freeze
#define LCD_MOV         0x10    // Cursor move, not shift

#define LCD_OFF         0x08    // lcd off
#define LCD_ON          0x0C    // lcd on             
#define LCD_BLINK_ON    0x0D    // blink on              
#define LCD_CURSOR_ON   0x0E    // cursor on               
#define LCD_ALL_ON      0x0F    // cursor on /  blink on
#define LCD_LINE1       0x80    // cursor Pos on line 1 (or with column)
#define LCD_LINE2       0xC0    // cursor Pos on line 2 (or with column)

unsigned char chr,data,pos;

// writes a char to the LCD
void LCD_char(unsigned char data)

{
PORTD = (data & 0xf0) >> 2; 
PORTB |= 1<<LCD_RS;
PORTB |= 1<<LCD_E;
_delay_ms(2);
PORTB &= ~(1<<LCD_E);
_delay_ms(2);

PORTD = (data & 0x0f) << 2; //low nibble
PORTB |= 1<<LCD_RS;
PORTB |= 1<<LCD_E;
_delay_ms(2);
PORTB &= ~(1<<LCD_E);
_delay_ms(2);
}


// writes a instruction to the LCD
void LCD_inst(unsigned char inst)
{   
    PORTD = (inst & 0xf0) >> 2;
    PORTB &= ~(1<<LCD_RS); // set RS to instructions
    PORTB |= 1<<LCD_E;
    _delay_ms(2);
    PORTB &= ~(1<<LCD_E);
    _delay_ms(2);
    
    PORTD = (inst & 0x0f) << 2; //send low nibble
    PORTB |= 1<<LCD_E;
    _delay_ms(2);
    PORTB &= ~(1<<LCD_E);
    _delay_ms(2);   
}

// clear display
void LCDclr(void)
{
LCD_inst (LCD_CLR);
}

// return home
void LCDhome(void)
{
LCD_inst (LCD_HOME);
}

// LCD off
void LCDoff(void)
{
LCD_inst (LCD_OFF);
}

// LCD on
void LCDon(void)
{
LCD_inst (LCD_ON);
}

// cursor on
void LCDcursor(void)
{
LCD_inst (LCD_CURSOR_ON);
}

// blink on
void LCDblink(void)
{
LCD_inst (LCD_BLINK_ON);
}

// cursor all on
void LCDall(void)
{
LCD_inst (LCD_ALL_ON);
}

//go to first line
void LCDline1 (void)

{
LCD_inst (0b10000000);

}

//go to second line
void LCDline2 (void)

{
LCD_inst (0b11000000);

}


// goto position x,y
void LCDgoto (char x,char y)
{

if (y == 0)
{
pos = 0b00000000 + x;
}

else if (y == 1)
{
pos = 0b01000000 + x;
}

LCD_inst (0b10000000 | pos);

}

void LCDtext(char *data)
{
    while (*data)
    {
        LCD_char(*data);
        data++;
    }
}

void LCD_init(void)
{
    DDRB = 0xFF;  // portb as output
    DDRD = 0xff;
    _delay_ms(40);

    PORTD = 1<<2;
    PORTB |= 1<<LCD_E;
    _delay_ms(1);
    PORTB &= ~(1<<LCD_E);
    _delay_ms(1);

    PORTD = 1<<2;
    PORTB |= 1<<LCD_E;
    _delay_ms(1);
    PORTB &= ~(1<<LCD_E);
    _delay_ms(1);

    PORTD = 1<<2;
    PORTB |= 1<<LCD_E;
    _delay_ms(1);
    PORTB &= ~(1<<LCD_E);
    _delay_ms(1);

    

    //set 4-bit mode and 2-line
    LCD_inst (0b00101000);

    //turn on display and cursor
    LCD_inst (0b00001100);

    //clr display
    LCD_inst (LCD_CLR);

}


int main( void )
{
    LCD_init();
    //LCDtext (">>AVR LCD DEMO<<");
    //LCDgoto (2,1);
    //LCDtext("Hello World!");
    //LCDall();
    //LCDhome();
}






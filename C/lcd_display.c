/*
 * 	startup.c
 *
 *
 * Description:
 *  Controls an ASCII LCD display via GPIO registers.
 *  Demonstrates low-level data and control line handling, timing, and initialization
 *  of a standard HD44780-compatible LCD.
 *  Hardware: MD407 (STM32F4-based board)
 */
__attribute__((naked)) __attribute__((section (".start_section")) )
void startup ( void )
{
__asm__ volatile(" LDR R0,=0x2001C000\n");		/* set stack */
__asm__ volatile(" MOV SP,R0\n");
__asm__ volatile(" BL main\n");					/* call main */
__asm__ volatile(".L1: B .L1\n");				/* never return */
}

#define SYSTICK 0xE000E010
#define GPIO_E 0x40021000

#define STK_CTRL ((volatile unsigned int *) (SYSTICK))
#define STK_LOAD ((volatile unsigned int *) (SYSTICK + 0x4))
#define STK_VAL ((volatile unsigned int *) (SYSTICK + 0x8))

#define GPIO_OTYPER ((volatile unsigned short *) (GPIO_E+0x4))
#define GPIO_MODER ((volatile unsigned int *) (GPIO_E))
#define GPIO_ODR_LOW ((volatile unsigned char *) (GPIO_E+0x14))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (GPIO_E+0x15))

#define GPIO_IDR_LOW ((volatile unsigned char *) (GPIO_E+0x10))
#define GPIO_IDR_HIGH ((volatile unsigned char *) (GPIO_E+0x11))

#define B_E 0x40
#define B_SELECT 4
#define B_RW 2
#define B_RS 1

void init_app(void);
void delay_250ns(void);
void delay_mikro(unsigned int us);
void delay_milli(unsigned int ms);

void ascii_ctrl_bit_set(char x);
void ascii_ctrl_bit_clear( char x );

void ascii_write_controller(unsigned char byte);
unsigned char ascii_read_controller(void);
void ascii_write_cmd(unsigned char command);
void ascii_write_data(unsigned char data);
unsigned char ascii_read_status(void);
unsigned char ascii_read_data(void);

void ascii_init(void);

void ascii_gotoxy(int x, int y);
void ascii_write_char(unsigned char c);

void main(void)
{
	char *s;
	char test1[] = "Tommy ";
	char test2[] = "is the best";
	
	init_app();
	ascii_init();
	ascii_gotoxy(1,1);
	s = test1;
	while(*s)
		ascii_write_char(*s++);
	ascii_gotoxy(1,2);	
	s = test2;
	while(*s)
		ascii_write_char(*s++);
	return 0;

}

void init_app(void)
{	
	*GPIO_MODER = 0x55555555;
	*GPIO_OTYPER = 0x00000000;
}

void delay_250ns(void){
//SystemCoreClock = 168000000;
*STK_CTRL = 0;
*STK_LOAD = ( (168/4) -1 );
*STK_VAL = 0;
*STK_CTRL = 5;
while( (*STK_CTRL & 0x10000 )== 0 );
*STK_CTRL = 0;

}

void delay_mikro(unsigned int us)
{
	#ifdef SIMULATOR
		us = us / 1000;
		us++;
	#endif
	
	while( us > 0 )
	{
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
		us--;
	}
}

void delay_milli(unsigned int ms)
{
	#ifdef SIMULATOR
		ms = ms / 1000;
		ms++;
	#endif
	
	while( ms > 0 )
	{
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
		
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
		
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();
		
		delay_250ns();
		delay_250ns();
		delay_250ns();
		delay_250ns();

		ms--;
	}
}
	
	void ascii_ctrl_bit_set(char x)
	{
		char c;
		c = *GPIO_ODR_LOW;
		*GPIO_ODR_LOW = B_SELECT | x | c;
	}
	
	void ascii_ctrl_bit_clear( char x )
	{
		char c;
		c = *GPIO_ODR_LOW;
		c = c & ~x;
		*GPIO_ODR_LOW = B_SELECT | c;
	}
	
	void ascii_write_controller(unsigned char byte)
	{
		delay_250ns();
		ascii_ctrl_bit_set(B_E);
		*GPIO_ODR_HIGH = byte;
		delay_250ns();
		ascii_ctrl_bit_clear(B_E);
		delay_250ns();
	}
	unsigned char ascii_read_controller(void)
	{
		char c;
		ascii_ctrl_bit_set(B_E);
		delay_250ns();
		delay_250ns();
		c = *GPIO_IDR_HIGH;
		ascii_ctrl_bit_clear(B_E);
		return c;
	}
	void ascii_write_cmd(unsigned char command)
	{
		ascii_ctrl_bit_clear(B_RS);
		ascii_ctrl_bit_clear(B_RW);
		ascii_write_controller(command);
		
	}
	void ascii_write_data(unsigned char data)
	{
		ascii_ctrl_bit_clear(B_RW);
		ascii_ctrl_bit_set(B_RS);
		ascii_write_controller(data);
	}
	unsigned char ascii_read_status(void)
	{
		char c;
		*GPIO_MODER = 0x00005555;
		ascii_ctrl_bit_set(B_RW);
		ascii_ctrl_bit_clear(B_RS);
		c = ascii_read_controller();
		*GPIO_MODER = 0x55555555;
		return c;
	}
	unsigned char ascii_read_data(void)
	{
		char c;
		*GPIO_MODER = 0x00005555;
		ascii_ctrl_bit_set(B_RS);
		ascii_ctrl_bit_set(B_RW);
		c = ascii_read_controller();
		*GPIO_MODER = 0x55555555;
		return c;
	}
	
	void ascii_init(void)
	{	while((ascii_read_status() & 0x80) == 0x80){}
		delay_mikro(8); 
		
		ascii_write_cmd(0x38); // Rader teckenstorlek
		delay_mikro(39);		
		ascii_write_cmd(0x0F);	// Tänd display
		delay_mikro(39);
		ascii_write_cmd(1); //clear display
		delay_milli(2);
		ascii_write_cmd(0x06); //Entry mode
		delay_mikro(39);
	
	}
	
	void ascii_gotoxy(int x, int y)
	{
		int adress = x-1;
		if( 2 == y)
			adress = adress + 0x40;
		ascii_write_cmd(0x80 | adress);
	}
	
	void ascii_write_char(unsigned char c)
	{
		while((ascii_read_status() & 0x80) == 0x80){}
		delay_mikro(8); 
		ascii_write_data(c);
		delay_mikro(43); 
	}

	

	



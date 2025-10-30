/*
 * 	startup.c
 *
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
#define GPIO_D 0x40020C00

#define STK_CTRL ((volatile unsigned int *) (SYSTICK))
#define STK_LOAD ((volatile unsigned int *) (SYSTICK + 0x4))
#define STK_VAL ((volatile unsigned int *) (SYSTICK + 0x8))

#define GPIO_OTYPER ((volatile unsigned short *) (GPIO_D+0x4))
#define GPIO_MODER ((volatile unsigned int *) (GPIO_D))
#define GPIO_ODR_LOW ((volatile unsigned char *) (GPIO_D+0x14))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (GPIO_D+0x15))


void main(void)
{
	init_app();
	while(1)
	{
		*GPIO_ODR_LOW = 0;
		delay_milli(500);
		*GPIO_ODR_LOW = 0xFF;
		delay_milli(500);
	}
}

void init_app(void)
{	
	*GPIO_MODER = 0x00005555;
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


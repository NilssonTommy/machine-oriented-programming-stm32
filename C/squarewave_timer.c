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
#define SCB_VTOR ((volatile unsigned int *)(0xE000ED08))
#define STK_CTRL ((volatile unsigned int *)(0xE000E010))
#define STK_LOAD ((volatile unsigned int *)(0xE000E014))
#define STK_VAL ((volatile unsigned int *)(0xE000E018))

#define GPIO 0x40021000
#define GPIO_E_MODER ((volatile unsigned int*)(GPIO))
#define GPIO_OTYPER ((volatile unsigned int*)(GPIO+4))
#define GPIO_ODR ((volatile unsigned char*)(GPIO+0x14))

#define PERIOD 1000

/*void InitSquareWave();

void StartSquareWave(unsigned int period_in_us);

void StopSquareWave();

void systick_irq_handler( void );

void delay( unsigned int count );

void delay_1mikro( void );*/

static volatile int systick_flag;
static volatile int delay_count;
int flip = 0;



void StartSquareWave(unsigned int period_in_us){
		
	delay(period_in_us);
	while(1){
		if(systick_flag)
		{
			*GPIO_ODR = flip;
			flip ^= 1;
			systick_flag = 0;
			delay_count = period_in_us;
		}
		else
			systick_irq_handler();		
	}
}


void delay_1mikro( void )
{
	*STK_CTRL = 0;
	*STK_LOAD = ( 168 - 1 );
	*STK_VAL = 0;
	*STK_CTRL = 7;
}

void systick_irq_handler()
{
	*STK_CTRL = 0;
	delay_count -- ;
	if( delay_count > 0 ) delay_1mikro();
	else systick_flag = 1;
}

void delay( unsigned int count )
{
	if( count == 0 ) return;
	delay_count = count;
	systick_flag = 0;
	delay_1mikro();
}

void InitSquareWave(){
	*GPIO_E_MODER = 01;
	*GPIO_OTYPER = 0x00000000;
	*SCB_VTOR = 0x2001C000;
	*((void(**)(void))0x2001C03C) = systick_irq_handler;

}
void main(void)
{
	InitSquareWave();	
	StartSquareWave(PERIOD); 
	while(1) { /* Gör ingenting */ }
	StopSquareWave();
}



/*void InitSquareWave(){
	*GPIO_E_MODER = 01;
	*GPIO_OTYPER = 0x00000000;
	*SCB_VTOR = 0x2001C00;
	*((void(**)(void))0x2001C3C) = &systick_irq_handler;

}



void StartSquareWave(unsigned int period_in_us){
		
	delay(period_in_us);
	while(1){
		if(systick_flag)
		{
			*GPIO_ODR = flip;
			flip ^= 1;
		}
		else
			systick_irq_handler();		
	}
}


void delay_1mikro( void )
{
	*STK_CTRL = 0;
	*STK_LOAD = ( 168 - 1 );
	*STK_VAL = 0;
	*STK_CTRL = 7;
}

void systick_irq_handler( void )
{
	*STK_CTRL = 0;
	delay_count -- ;
	if( delay_count > 0 ) delay_1mikro();
	else systick_flag = 1;
}

void delay( unsigned int count )
{
	if( count == 0 ) return;
	delay_count = count;
	systick_flag = 0;
	delay_1mikro();
}*/
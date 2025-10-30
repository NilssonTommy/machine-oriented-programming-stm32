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

#define GPIO_IDR_LOW ((volatile unsigned char *) (GPIO+0x10))
#define GPIO_IDR_HIGH ((volatile unsigned char *) (GPIO+0x11))
#define GPIO_PUPDR ((volatile unsigned int *) (GPIO+0xC))
#define GPIO_ODR_LOW ((volatile unsigned char *) (GPIO+0x14))
#define GPIO_ODR_HIGH ((volatile unsigned char *) (GPIO+0x15))


#define SYSCFG_EXTICR3 ((volatile unsigned short *)(0x40013810))
#define EXTI_IMR ((unsigned int*)(0x40013C00))
#define EXTI_RTSR ((unsigned int*)(0x40013C08))
#define EXTI_PR ((unsigned int*)(0x40013C14))
#define NVIC_ISER0 ((unsigned int*)(0xE000E100))
#define NVIC_ISER1 ((unsigned int*)(0xE000E104))


unsigned char Key(void);
void KeyActivate(unsigned int row);
int KeyColon(void);

void InitSquareWave();

void StartSquareWave(unsigned int period_in_us);

void StopSquareWave();

void systick_irq_handler( void );

void interuppt_handler(void);

int flip = 0;

unsigned char currently_pressed_key = 1;
void InitKeyboard();

void main(void)
{
	InitSquareWave();
	InitKeyboard();
	StartSquareWave(PERIOD); 
	while(currently_pressed_key != 0) { /* Gör ingenting */ }
	StopSquareWave();
	while(currently_pressed_key != 5) {/* Gör ingenting */ }
	StartSquareWave(PERIOD * 2); 
	while(currently_pressed_key != 10) {/* Gör ingenting */ }
	StopSquareWave(); 
}


void interuppt_handler(void){
	
	*EXTI_IMR &= ( 0<<8);
	*EXTI_IMR &= ( 0<<9);
	*EXTI_IMR &= ( 0<<10);
	*EXTI_IMR &= ( 0<<11);
	
	if(*EXTI_PR & ( 1<<8))
	{		
		*EXTI_PR |= ( 1<<8);
	}
	if(*EXTI_PR & ( 1<<9))
	{	
		*EXTI_PR |= ( 1<<9);
	}
	if(*EXTI_PR & ( 1<<10))
	{	
		*EXTI_PR |= ( 1<<10);
	}
	if(*EXTI_PR & ( 1<<11))
	{	
		*EXTI_PR |= ( 1<<11);
	}
	
	
	currently_pressed_key = Key();
	
	*EXTI_IMR |= ( 1<<8);
	*EXTI_IMR |= ( 1<<9);
	*EXTI_IMR |= ( 1<<10);
	*EXTI_IMR |= ( 1<<11);
	
	*GPIO_ODR_HIGH = 0xF0;


}


void StartSquareWave(unsigned int period_in_us)
{
	*STK_CTRL = 0;
	*STK_LOAD = ( 168 - 1 ) * (period_in_us/2);
	*STK_VAL = 0;
	*STK_CTRL = 7;
}


void StopSquareWave()
{
	*STK_CTRL = 0;
	
}

void systick_irq_handler()
{
	*GPIO_ODR = flip;
	flip ^= 1;
}


void InitSquareWave(){
	
	*GPIO_E_MODER = 01;
	*GPIO_OTYPER = 0x00000000;
	*SCB_VTOR = 0x2001C000;
	*((void(**)(void))0x2001C03C) = systick_irq_handler;

}

void InitKeyboard(void)
{
	*GPIO_E_MODER = 0x0000FFFF & *GPIO_E_MODER ;	
	*GPIO_E_MODER = 0x55000000 | *GPIO_E_MODER ;
	*GPIO_PUPDR = 0x00AA0000;
	*SYSCFG_EXTICR3 = 0x4444;
	*EXTI_IMR |= ( 1<<8);
	*EXTI_IMR |= ( 1<<9);
	*EXTI_IMR |= ( 1<<10);
	*EXTI_IMR |= ( 1<<11);
	*EXTI_RTSR |= ( 1<<8);
	*EXTI_RTSR |= ( 1<<9);
	*EXTI_RTSR |= ( 1<<10);
	*EXTI_RTSR |= ( 1<<11);
	*NVIC_ISER0 |= ( 1<<23);
	*NVIC_ISER1 |= ( 1<<8);
	*((void (**)(void))(0x2001C0E0)) = interuppt_handler;
	*((void (**)(void))(0x2001C09C)) = interuppt_handler;
	*GPIO_ODR_HIGH = 0xF0;
}

unsigned char Key(void){
	unsigned char karta[] = {1,2,3,0xA,4,5,6,0xB,7,8,9,0xC,0xE,0,0xF,0xD};
	
	int row, col;
	for(row = 1; row <= 4; row++){
		KeyActivate(row);
		if((col = KeyColon())){
			KeyActivate(0);
			return karta[4*(row - 1) + (col - 1)];
		}
	}
	KeyActivate(0);
	return 0xFF;
}
					


int KeyColon(void)
{
	unsigned char c;
	c = *GPIO_IDR_HIGH;
	if(c & 0x8) return 4;
	if(c & 0x4) return 3;
	if(c & 0x2) return 2;
	if(c & 0x1) return 1;
	return 0;
}


void KeyActivate(unsigned int row){
	switch(row)
	{
		case 1: *GPIO_ODR_HIGH = 0x10; break;
		case 2: *GPIO_ODR_HIGH = 0x20; break;
		case 3: *GPIO_ODR_HIGH = 0x40; break;
		case 4: *GPIO_ODR_HIGH = 0x80; break;
		case 0: *GPIO_ODR_HIGH = 0x00; break;
		
	}
}

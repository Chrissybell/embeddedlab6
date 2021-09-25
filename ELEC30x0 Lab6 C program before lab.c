/*--------------------------------------
Christobel Nweke
ELEC 3050
Lab 5
8/31/21
---------------------------------------*/
/* Obtained delay() function code from https://www.geeksforgeeks.org/time-delay-c/ */
//#include <time.h>
//#include <stdio.h>
#include "stm32l432xx.h" /* Microcontroller information */
/* Define global variables */
//uint32_t zero = 0x0;
//uint32_t one = 0x1;
//uint32_t two = 0x2;
//uint32_t three = 0x3;
//uint32_t four = 0x4;
//uint32_t five = 0x5;
//uint32_t six = 0x6;
//uint32_t seven = 0x7;
//uint32_t eigth = 0x8;
//uint32_t nine = 0x9;
//uint32_t ten = 0xA;
//uint32_t eleven = 0xB;
//uint32_t twelve = 0xC;
//uint32_t thirteen = 0xD;
//uint32_t asterisk = 0xE;
//uint32_t hash = 0xF;
//char keys[4][4] = {{one,two,three,ten}, 
//                   {four,five,six,eleven},
//                   {seven,eigth,nine,twelve},
//                   {asterisk,zero,hash,thirteen}};
//char keys[4][4] = {{0x1,0x2,0x3,0xa}, 
 //                    {0x4,0x5,0x6,0xb},
  //                   {0x7,0x8,0x9,0xc},
    //                {0xe,0x0,0xf,0xd}};
// Define to locate which key was pressed on keypad
//typedef struct locate {
   //int col;
  // int row;
//} locate;
char keys[4][4] = {{0xd,0xf,0x0,0xe}, 
                    {0xc,0x9,0x8,0x7},
                     {0xb,0x6,0x5,0x4},
                     {0xa,0x3,0x2,0x1}};	
char keys1[4][4] = {{0x1,0x2,0x3,0xa}, 
                     {0x4,0x5,0x6,0xb},
                     {0x7,0x8,0x9,0xc},
                    {0xe,0x0,0xf,0xd}}; 

int      keep_track;
uint32_t duty;
uint32_t period;
uint32_t psc_value = 0x1FFF; 
uint32_t arr_value = 0XFA0; /*4000 hz in decimal. internal timer clock is 4MZ and we want 1khz switching frequency
//uint32_t arr_value = 0xFF;
uint32_t count_ISR;
uint32_t key_pressed;
uint32_t counter; /* c */
uint32_t reg;			
int start_count;									 
int row;
int col;

//#define psc_value (uint32_t)  255
//#define arr_value (uint32_t)  8191
//uint32_t run_interrupt; 
#define GPIOA_MODER_11to8_OUTPUT; 0x0055000
#define GPIOA_MODER_PA0_AF	0x1
#define GPIOB_MODER_6to3_OUTPUT  0x00001540
//#define GPIOB_MODER_7to4_OUTPUT  0x00005500

/*---------------------------------------------------*/
//GPIO MODER SETTINGS
// 00 = Digital Input Mode
// 01 = Digital Output mode
// 10 = Alternate Function Mode
// 11 = Analog Input Mode

//GPIO PUPDR SETTINGS
// 00 = No pull-up or pull-down
// 01 = Activate pull-up
// 10 = Activate pull-down

/* Initialize GPIO pins used in the program */
/* PA[5:2]   = Keypad rows. Pin 8:5 on keypad */
/* PA[11:8]  = Keypad Columns. Pin 4:1 on keypad */
/* PB[7:4]   = Output of BCD stopwatch timer */


/*---------------------------------------------------*/
void PinSetup(void) {
	RCC->AHB2ENR |= 0x01; /* Enable GPIOA clock (bit 0) */
	/* Configure PA[5:2] as input port and activate PA[11:8] */
	GPIOA->MODER &= ~(GPIO_MODER_MODER11 | GPIO_MODER_MODER10 | GPIO_MODER_MODER9 | GPIO_MODER_MODER8 |
						GPIO_MODER_MODER5 | GPIO_MODER_MODER4 | GPIO_MODER_MODER3 | GPIO_MODER_MODER2 | GPIO_MODER_MODER0); 
					

	/* Configure PA[11:8] as output mode */								
	GPIOA->MODER |= GPIOA_MODER_11to8_OUTPUT;  
	/* Configure PA0 as Alternate function mode */
	GPIOA->MODER |= GPIOA_MODER_PA0_AF;			   
	

	RCC->AHB2ENR |= 0x02; /* Enable GPIOB clock (bit 1) */
	/* Set PB0 to input mode and activate PB[6:3]  */
	GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER5 | GPIO_MODER_MODER4 | GPIO_MODER_MODER3 | GPIO_MODER_MODER0);
	/* Set PB[6:3] as output mode */
	GPIOB->MODER |=  GPIOB_MODER_6to3_OUTPUT;

	/* Get PB[7:4] ready to be configured as output mode  */
	//GPIOB->MODER &= ~(GPIO_MODER_MODER7 | GPIO_MODER_MODER6 | GPIO_MODER_MODER5 | GPIO_MODER_MODER4 ); 
	/* Set PB[7:4] as output mode */
	//GPIOB->MODER |=  GPIOB_MODER_7to4_OUTPUT; 

   	//PUPDR setups
   	GPIOA->PUPDR &= ~(0x00000FF0); //Clears bits Pull up/down resistors for PA5-2
   	GPIOA->PUPDR |= (0x00000550); //Set bits to 01 for PUPDR pull-up PA5-2

	

	GPIOA->ODR &= ~(0xF00); // drive 11:8 low.
   	

 
}

/* void delay(void) {
   int i,j,n;
   for (i=0; i<20; i++) { //outer loop
      for (j=0; j<8225; j++) { //inner loop
         n = j; //dummy operation for single-step test
      } //do nothing
   }
}
*/

//void delay(void) {
   //int i,j,n;
  // for (i=0; i<20; i++) { //outer loop
  //    for (j=0; j<8225; j++) { //inner loop
    //     n = j; //dummy operation for single-step test
  //    } //do nothing
  // }
//}
//void delay(int number_of_seconds)
//{
    // Converting time into milli_seconds
    //int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
   // clock_t start_time = clock();
  
    // looping till required time is not achieved
  //  while (clock() < start_time + milli_seconds);
        
//}

void TIM2_Pin_Setup() {
	/* Enable AF1 */
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL0);
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0;
	/* Enable TIM2 */
	RCC->APB1ENR1 |= 0x01;
	/* Enable Timer counter */
	//TIM2->CR1 |= TIM_CR1_CEN;
	/* Enable CH1 as timer channel output */
	TIM2->CCMR1 &= ~(TIM_CCMR1_CC1S);
	/* Configure output mode for PWM mode 1 */
	TIM2->CCMR1 &= ~(0x60);
	TIM2->CCMR1 |= 0x60;
	/*selects enable and output polarity, polarity 0 = active high */
	TIM2->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1P );
	TIM2->CCER |= TIM_CCER_CC1E;
	period = arr_value + 1; 
	TIM2->PSC = psc_value; //prescaler value
	TIM2->ARR = arr_value; //auto-reload value
	//TIM2->DIER |= TIM_DIER_UIE;
	//NVIC_EnableIRQ(TIM7_IRQn);
	//__enable_irq();

}


void PA0_Interrupt_Pin_Setup(void) {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Turn on system config peripheral
	// Select PA0 as EXTI0
	SYSCFG->EXTICR[0]  &= ~(SYSCFG_EXTICR1_EXTI0);
	SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI0_PA;

	EXTI->FTSR1 |= EXTI_FTSR1_FT0; //Enable EXTI0 as falling-edge trigger
	EXTI->PR1 = EXTI_PR1_PIF0;     //Clear EXTI0 pending bit
	EXTI->IMR1 |= EXTI_IMR1_IM0;   //Enable EXTI0

	//NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);
	__enable_irq();
	
}

//int find_key() {
	//row = 0;
	//col = 0;
	//GPIOA->ODR = (0xF << 8); /* Set all columns high initially */
	//for (int i = 0; i < 4; i++) { // Scan each column
		//GPIOA->ODR = 1 << 8 ; // write a 0 to each column starting from column corresponding to pin 8 up to pin 11
	//	GPIOA->ODR = GPIOA->ODR ^(1<<(8+i));
	//	reg = GPIOA->IDR;
	//	for(int j =0; j < 4; j++) { 
	//		if (!(reg & (1 << (j+2)) ) ) { // if a row is not set then we have found the key row[i] and col[j]
	//			row = j;
		//		col = i;
		//		return 0;
	//		}
	//	}
	//}
	// if key is not found set row and col to -1 for inside our interrupt where we will handle this case
	//row = -1;
	//col = -1;
	//return 0;

//}




//Interrupt handler for PA0
void EXTI0_IRQHandler(void) {
	__disable_irq();
	
	/* start find key */
	row = 0;
	col = 0;
	GPIOA->ODR = (0xF << 8); /* Set all columns high initially */
	for (int i = 0; i < 4; i++) { // Scan each column
		//GPIOA->ODR = 1 << 8 ; // write a 0 to each column starting from column corresponding to pin 8 up to pin 11
		GPIOA->ODR = GPIOA->ODR ^(1<<(8+i));
		reg = GPIOA->IDR;
		for(int j =0; j < 4; j++) { 
			if (!(reg & (1 << (j+2)) ) ) { // if a row is not set then we have found the key row[i] and col[j]
				row = j;
				col = i;
			}
		}
		if ( (row == 0) && (col == 0) ) { // if key is not found set row and col to -1 for inside our interrupt where we will handle this case
			row = -1;
			col = -1;
		}
	}
	/* end find key */

	/* start find key pressed and assign duty cycle */

	/* 0% */
	if ( (keys[row][col]) == 0x0 ) {
		duty = (period * 0);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 10% */
	else if ( (keys[row][col]) == 0x1 ) {
		duty = (period * 0.1);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 20% */
	else if ( (keys[row][col]) == 0x2 ) {
		duty = (period * 0.2);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 30% */
	else if ( (keys[row][col]) == 0x3 ) {
		duty = (period * 0.3);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 40% */	
	else if ( (keys[row][col]) == 0x4 ) {
		duty = (period * 0.4);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 50% */
	else if ( (keys[row][col]) == 0x4 ) {
		duty = (period * 0.5);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 60% */
	else if ( (keys[row][col]) == 0x6 ) {
		duty = (period * 0.6);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 70% */	
	else if ( (keys[row][col]) == 0x7 ) {
		duty = (period * 0.7);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 80% */
	else if ( (keys[row][col]) == 0x8 ) {
		duty = (period * 0.8);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 90% */
	else if ( (keys[row][col]) == 0x9 ) {
		duty = (period * 0.9);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	/* 100% */
	else if ( (keys[row][col]) == 0xA ) {
		duty = (period * 1);
		TIM2->CCR1 = duty;
		TIM2->CR1 |= TIM_CR1_CEN; //enable TIM2 to count
	}
	else {
		//do nothing
	}								
	EXTI->PR1 = EXTI_PR1_PIF0;
	//clear pending flag in NVIC
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	GPIOA->ODR &= ~(0xF00); // Clear PA11-8
	__enable_irq();
}




/*----------------------------------------------------------*/
/* Delay function - do nothing for about 1 seconds */
/*----------------------------------------------------------*/
// Delay is exactly 0.5s


int main() {
	PinSetup();
  	PA0_Interrupt_Pin_Setup();
	TIM2_Pin_Setup();
	duty = 0;
	counter = 0;
	row = 0;
	col = 0;
	GPIOB->ODR &= 0xC03F;
	while(1) {

	}
}


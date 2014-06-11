/*
 Lifan Smily (320) car rear wiper control module
 
 MCU pic12F683, hardware PWM
 Hi-Tech C STD 9.60PL3
 v0.1
 
 Yuriy Gorbachev, 2014
 urvindt@gmail.com
*/

//----------------------------------------------------------------------------//

#include <htc.h>

//----------------------------------------------------------------------------//
// CONFIGURATION
//----------------------------------------------------------------------------//

/* Program device configuration word
 * Oscillator = Internal RC No Clock
 * Watchdog Timer = Off
 * Power Up Timer = Off
 * Master Clear Enable = Internal
 * Code Protect = Off
 * Data EE Read Protect = Off
 * Brown Out Detect = BOD and SBOREN disabled
 * Internal External Switch Over Mode = Disabled
 * Monitor Clock Fail-safe = Disabled
 */
__CONFIG(INTIO & WDTDIS & PWRTDIS & MCLRDIS & UNPROTECT & UNPROTECT & BORDIS & IESODIS & FCMDIS);

//----------------------------------------------------------------------------//

// Useful variable types
#define uint32 unsigned long
#define uint16 unsigned int
#define uint8 unsigned char

//----------------------------------------------------------------------------//

//PINS
#define SWITCH_PIN GPIO5
#define SWITCH_PIN_ON 0
#define SWITCH_PIN_OFF 1

#define WIPER_PIN GPIO0
#define WIPER_PIN_ON 0
#define WIPER_PIN_OFF 1


volatile bit fSwitchState;
volatile uint8 fSwitchDebouncer;
#define SWITCH_DEBOUNCE_MAX_TICKS 5

volatile uint8 fWiperOnTimer;
VOLATILE bit fWiperAlwaysOn;
#define WIPER_ON_MAX_TICKS 24

//----------------------------------------------------------------------------//

void initSoftware(void)
{
	WIPER_PIN = WIPER_PIN_OFF;
	fSwitchState = SWITCH_PIN_OFF;
	fSwitchDebouncer = 0;
	
	fWiperOnTimer = 0;
	fWiperAlwaysOn = 0;
}

void initHardware(void)
{
	// Disable watch dog
	WDTCON = 0;
	
	// IO Port direction (1 - input)
	TRISIO = 0b00011000;
	GPIO = 0;
	
	//Internal oscillator set to 8MHz
	OSCCON = 0b01110000;
	
	// Prescale to 1:256 (30 owerflows per second)
	OPTION = 0b10000111;
	//         ||||||||- Prescaler rate
	//         |||||---- Prescaler assighnment (0 - Timer0, 1 - WDT)
	//         ||||----- Timer0 edge (0 - low to high)
	//         |||------ Timer0 source (0 - internal)
	//         ||------- Interrupt edge (0 - falling)
	//         |-------- GPIO Pull up resistors (1 to disable)
	
	// Disable comparators and PWM
	CMCON1 = 0x07;
	CMCON0 = 0x07;	
	CCP1CON = 0x00;
	
	// ADC Configuration 
	ADCON0 = 0x00;
	ANSEL = 0x00;
	
	// Interrupts
	INTCON = 0b01100000;	
	//         |||||- GPIO change
	//         ||||-- GP2/INT
	//         |||--- Timer0
	//         ||---- Peripherial interrupts
	//         |----- Global intterupt (off while initialization)
	
	// Peripherial interrupts
	PIE1 = 0x00;
}

//----------------------------------------------------------------------------//

void turnWiperOn()
{
	WIPER_PIN = WIPER_PIN_ON;
	fWiperOnTimer = WIPER_ON_MAX_TICKS;
}

void turnWiperOff()
{
	WIPER_PIN = WIPER_PIN_OFF;
}


void processWiper()
{
	if(fWiperOnTimer > 0)
		fWiperOnTimer--;
	else
	{
		if(!fWiperAlwaysOn)
			turnWiperOff();
	}
}

void processSwitch()
{
	if(fSwitchDebouncer == 0)
	{
		if(SWITCH_PIN != fSwitchState)
			fSwitchDebouncer = SWITCH_DEBOUNCE_MAX_TICKS;
	}
	else
	{
		if(fSwitchDebouncer == 1 && SWITCH_PIN != fSwitchState)
		{
			fSwitchState = SWITCH_PIN;
			
			if(fSwitchState == SWITCH_PIN_ON)
			{
				turnWiperOn();
			}
			else
			{
				
			}
		}
	}
}

//----------------------------------------------------------------------------//

void interrupt isr(void)
{

	// Timer0
	if((T0IE) && (T0IF))
	{
		processSwitch();
		processWiper();
		T0IF = 0;
	}
}

//----------------------------------------------------------------------------//

void main(void)
{
	initHardware();
	initSoftware();
	ei();	
		
	while (1)
	{

	}
}
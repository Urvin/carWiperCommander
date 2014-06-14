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
#define SWITCH_PIN GPIO3
#define SWITCH_PIN_ON 0
#define SWITCH_PIN_OFF 1

#define WIPER_PIN GPIO0
#define WIPER_PIN_ON 1
#define WIPER_PIN_OFF 0

#define DEBUG_LED_PIN GPIO1

volatile bit fSwitchState;
volatile uint8 fSwitchDebouncer;
#define SWITCH_DEBOUNCE_TICKS 3

#define WIPER_WORK_MIN_TICKS 15
#define WIPER_WORK_MAX_TICKS 300
#define WIPER_WORK_INFINITY 65535
#define WIPER_WAIT_MIN_TICKS 60
#define WIPER_WAIT_MAX_TICKS 1800

volatile uint16 fWiperWorkTime;
volatile uint16 fWiperWaitTime;
volatile uint16 fWiperWorkTimer;
volatile uint16 fWiperWaitTimer;

volatile bit fTimersMode;
#define TIMERS_WRITE 0
#define TIMERS_USE 1

volatile bit shouldTurnWiperOff;

//----------------------------------------------------------------------------//

void initSoftware(void)
{
	WIPER_PIN = WIPER_PIN_OFF;
	fSwitchState = SWITCH_PIN_OFF;
	fSwitchDebouncer = 0;
	
	fTimersMode = TIMERS_USE;
	
	fWiperWaitTimer = 0;
	fWiperWorkTimer = 0;
	fWiperWorkTime = 0;
	fWiperWaitTime = 0;
	
	shouldTurnWiperOff = 0;	
}

void initHardware(void)
{
	// IO Port direction (1 - input)
	TRISIO = 0b00011000;
	GPIO   = 0b00000000;
	
	// Disable watch dog
	WDTCON = 0;
	
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

void turnWiperOn(void)
{
	WIPER_PIN = WIPER_PIN_ON;
	fWiperWorkTimer = 0;
}

void turnWiperOff(void)
{
	WIPER_PIN = WIPER_PIN_OFF;
}

void processWiper(void)
{
	uint16 lWorkTime = (fTimersMode == TIMERS_USE) ? fWiperWorkTime : WIPER_WORK_MIN_TICKS;
	
	if(fWiperWorkTimer < lWorkTime)
	{
		fWiperWorkTimer++;
	}
	else
	{
		if(shouldTurnWiperOff)
		{
			turnWiperOff();
			shouldTurnWiperOff = 0;
		}
	}
}

//----------------------------------------------------------------------------//

void onSwitchTimersUse(void)
{
	fTimersMode = TIMERS_WRITE;
	
	fWiperWaitTime = 0;
	fWiperWorkTime = 0;	
	
	shouldTurnWiperOff = 0;	
	turnWiperOn();
}

void onSwitchTimersWrite(void)
{
	fTimersMode = TIMERS_USE;		
			
	//Wiper turns on automatically on fWiperWaitTimer == 0
	shouldTurnWiperOff = 0;
	fWiperWaitTimer = 0;
}

void onSwitchOn(void)
{
	if(fTimersMode == TIMERS_USE)
	{
		onSwitchTimersUse();
	}
	else
	{
		if((fWiperWaitTime < WIPER_WAIT_MIN_TICKS) || (fWiperWaitTime > WIPER_WAIT_MAX_TICKS) || (fWiperWorkTime > WIPER_WORK_MAX_TICKS) )
			onSwitchTimersUse();
		else
			onSwitchTimersWrite();
	}
}

void onSwitchOff(void)
{
	
	if(fTimersMode == TIMERS_USE)
	{
		if(fWiperWorkTimer >= WIPER_WORK_MIN_TICKS)
			fWiperWorkTimer = WIPER_WORK_INFINITY;
	}
	else
	{
		if(fWiperWorkTime < WIPER_WORK_MIN_TICKS)
			fWiperWorkTime = WIPER_WORK_MIN_TICKS;
	}
		
	shouldTurnWiperOff = 1;
}

void processSwitch(void)
{
	if(fSwitchDebouncer == 0)
	{
		if(SWITCH_PIN != fSwitchState)
			fSwitchDebouncer = SWITCH_DEBOUNCE_TICKS;
	}
	else
	{
		if(fSwitchDebouncer == 1 && SWITCH_PIN != fSwitchState)
		{
			fSwitchState = SWITCH_PIN;
			
			if(fSwitchState == SWITCH_PIN_ON)
				onSwitchOn();
			else
				onSwitchOff();
		}
		
		fSwitchDebouncer--;
	}
}

//----------------------------------------------------------------------------//

void processTimers(void)
{
	if(fTimersMode == TIMERS_WRITE)
	{
		if(fWiperWaitTime <= WIPER_WAIT_MAX_TICKS)
			fWiperWaitTime++;
		
		if( (fSwitchState == SWITCH_PIN_ON) && (fWiperWorkTime <= WIPER_WORK_MAX_TICKS) )
			fWiperWorkTime++;
	}
	else
	{
		if(fSwitchState == SWITCH_PIN_ON)
		{
			if(fWiperWaitTimer == 0)
			{
				turnWiperOn();
				shouldTurnWiperOff = 1;
				fWiperWaitTimer = fWiperWaitTime;
			}
			fWiperWaitTimer--;
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
		processTimers();
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
		DEBUG_LED_PIN = (fSwitchState == SWITCH_PIN_ON) && fTimersMode;
	}
}
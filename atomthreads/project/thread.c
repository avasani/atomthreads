#include "atom.h"
#include "atom_os.h"
#include "Arduino.h"

int led = 13;
uint32_t app_main_start (void)
{
  int failures, i = 0;

    /* Default to zero failures */
    failures = 0;
	int count = 10;
    /* Compiler warnings */
	//    param = param;
    pinMode(led, OUTPUT);

    while (1) {
	    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
	    atomTimerDelay(3000);// wait for a second
	    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
	    atomTimerDelay(3000);      // wait for a second 
	    count++;
      	    printf("Hello : %d\n",i++);
    }
    return failures;
}

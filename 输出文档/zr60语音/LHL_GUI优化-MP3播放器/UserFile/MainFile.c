#include "includes.h"
int main(void)
{
	CreatGUITask();
	/* Start the scheduler. */
	vTaskStartScheduler();
	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}

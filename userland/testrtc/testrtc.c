#include <stdint.h>

#include "../../fish/ece391support.h"
#include "../../fish/ece391syscall.h"

#define SCREENWIDTH 79
#define ENDING 2
#define BUFMAX SCREENWIDTH+ENDING
#define START 1
#define STARTLOOP START+1
#define LOOPMAX BUFMAX-ENDING-1
#define STARTCHAR 'A'
#define ENDCHAR 'Z'

int main ()
{
    int32_t i = 0;
    int32_t j = 0;
    uint8_t curchar = STARTCHAR;
    uint8_t update = 1;
    int ret_val;
    int garbage;
    int rtc_fd;
    uint8_t buf[BUFMAX];

    int counter = 0;
    int fast_speed = 32;
    int slow_speed = 8;
    int speed = 8;

    char empty_char = ' ';

    // Clear buffer
    for(i = 0; i < BUFMAX; i++)
	    buf[i]=' ';

    // Initialize buffer with walls
    buf[BUFMAX-1]='\0';
    buf[BUFMAX-2]='\n';
    buf[BUFMAX-3]='|';
    buf[START]='|';

    // Open and set RTC Frequency
    rtc_fd = ece391_open((uint8_t*)"rtc");
    ret_val = 32;
    ret_val = ece391_write(rtc_fd, &ret_val, 4);

    void can_change_speed()
    {
    	counter++;
    	if (counter % 20 == 0)
    	{
    		if (speed == slow_speed)
    		{
    			speed = fast_speed;
	    		empty_char = ' ';
	    	}
    		else
    		{
    			speed = slow_speed;
    			empty_char = '-';
    		}
	    	ret_val = ece391_write(rtc_fd, &speed, 4);
    	}
    }

    while(1)
    {
	// Move out
	for(j = STARTLOOP; j < LOOPMAX; j++)
	{
		// Clear inner portion of world
		for(i = STARTLOOP; i < LOOPMAX; i++)
		{
			buf[i]=empty_char;
		}

		can_change_speed();

		// Draw character
		buf[j] = curchar;
		ece391_fdputs (1, buf);

		// Wait for RTC tick
		ece391_read(rtc_fd, &garbage, 4);
	}

	// Bounce back
    	for(j = LOOPMAX - 1; j >= STARTLOOP; j--)
    	{
		// Clear inner portion of the world
		for(i = STARTLOOP; i < LOOPMAX; i++)
		{
			buf[i]=empty_char;
		}

		can_change_speed();

		// Draw character
		buf[j] = curchar;
		ece391_fdputs (1, buf);

		// Wait for RTC tick
		ece391_read(rtc_fd, &garbage, 4);
    	}

	// Edge case on characters
	if(curchar == ENDCHAR)
	{
		curchar = STARTCHAR;
	}
	else
	{
		// Update current character
		curchar = curchar + update;
	}
    }
    return 0;
}

#include <stdint.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <libusb-1.0/libusb.h>

static uint64_t
GetMilli (void)
{
	struct timespec  ts;
	uint64_t		milli;
	
	clock_gettime(CLOCK_MONOTONIC, &ts);
	
	milli = (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
	return (milli);
}

static void
DrawBox(void)
{
	// this is only to demonstrate the characters to build a box
	// this code should be modified to draw a box of arbitrary size
	mvaddch (2, 32, ACS_ULCORNER);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_URCORNER);
	mvaddch (3, 32, ACS_VLINE);
	mvaddch (3, 38, ACS_VLINE);
	mvaddch (4, 32, ACS_VLINE);
	mvaddch (4, 38, ACS_VLINE);
	mvaddch (5, 32, ACS_VLINE);
	mvaddch (5, 38, ACS_VLINE);
	mvaddch (6, 32, ACS_LLCORNER);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_HLINE);
	addch (ACS_LRCORNER);
}
		
static void
GetSetTime(void)
{
    time_t		myRawTime;
    time_t		myRawTime2;
    struct tm	myTime;
    struct tm	myTime2;
    int			result = 73;

	// get current system time:
	time(&myRawTime);
	gmtime_r(&myRawTime, &myTime);
	mvprintw(10,5,"%d-%d-%d %02d:%02d:%02d",
		myTime.tm_year+1900,
		myTime.tm_mon+1,
		myTime.tm_mday,
		myTime.tm_hour,
		myTime.tm_min,
		myTime.tm_sec);
	
	// modify the system time:
	/*
	myTime.tm_year++;
	myTime.tm_hour += 2;
	myRawTime = mktime(&myTime);	
	result = stime(&myRawTime);
	*/
	
	// check new system time:
	time(&myRawTime2);
	gmtime_r(&myRawTime2, &myTime2);
	mvprintw(11,5,"%d-%d-%d %02d:%02d:%02d [status:%d]",
		myTime2.tm_year+1900,
		myTime2.tm_mon+1,
		myTime2.tm_mday,
		myTime2.tm_hour,
		myTime2.tm_min,
		myTime2.tm_sec,
		result);
}

int getxpad(libusb_device_handle* h, int& transferred)
{
	unsigned char data[32];
	error = libusb_interrupt_transfer(h, 0x81, data, sizeof data, &transferred, 0);
	if (error != 0) return (error);
	int i = 0;
	for (; i < transferred; i++)
	{
		if (i > 14) break;
		if (i != 0) printw(".");
		else { mvprintw(0, 0, "%02X", data[i]); continue; }
		printw("%02X", data[i]);				
	}
	//printf(" ");
}

int led(int n)
{
	unsigned char data[] = { 1, 3, (unsigned char)n };	
	int error = libusb_interrupt_transfer(h, 0x02, data, sizeof data, &transferred, 0);	
	return error;
}

int rumbler(int n)
{
	unsigned char b = ((unsigned char)n << 4);
	unsigned char data[] = { 0, 8, 0, 0, b, 0, 0, 0 };	
	error = libusb_interrupt_transfer(h, 0x02, data, sizeof data, &transferred, 0);	
	return error;
}

int 
main(void)
{
	libusb_device_handle *h;
	int error, transferred;

	int k = libusb_init(NULL);//233
	if (k != 0) printf("%d\n", k);
	h = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);
	if (h == NULL) {
		fprintf(stderr, "Failed to open device\n");
		return (1);
	}
	
	int v_led = 0, v_rumbler = 0;
	
    int     ch;
    unsigned int     i = 0;
    char    x[] = "|/-\\";

    initscr();              /* Start curses mode        */
    raw();                  /* Line buffering disabled  */
    nodelay(stdscr, TRUE);  /* getch() returns immediately      */
    noecho();               /* no cursor */
    curs_set(0);			/* no cursor */

	DrawBox();
	GetSetTime();
    mvprintw(3,5,"press a key ('q' to quit)");
    
    ch = getch();           /* If raw() hadn't been called
                             * we have to press 'enter' before it
                             * gets to the program      
                             */
	
    while (ch != 'q')
    {
		usleep(9000);	// wait a while
        ch = getch();
        if (ch == ERR)
        {
            // no key pressed: show that the while-loop is busy
            i++;
            if (i > strlen (x))
            {
                i = 0;
            }
            mvprintw(7,11,"%c %12lld", x[i], GetMilli());
			getxpad();
        }
		/**
		 * Set the LED
		 */
        else if (ch == 'a')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (--v_led < 0) v_led = 13;
			error = led(v_led);
        }
		else if (ch == 's')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (++v_led > 13) v_led = 0;
			error = led(v_led);
        }
		/**
		 * Set the rumbler
		 */
        else if (ch == 'z')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (--v_rumbler < 0) v_rumbler = 15;
			error = rumbler(v_rumbler);
        }
		else if (ch == 'x')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (++v_rumbler > 15) v_rumbler = 0;
			error = rumbler(v_rumbler);
        }
		if (error != 0) 
		{
			endwin();
			fprintf(stderr, "Transfer failed: %d\n", error);
			return (1);
		}		
    }
    
    endwin();           /* End curses mode        */

    return (0);
}

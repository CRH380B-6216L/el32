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
DrawBox(int x, int y)
{
	int step = 0;
	// this is only to demonstrate the characters to build a box
	// this code should be modified to draw a box of arbitrary size
	mvaddch(y, x, ACS_ULCORNER);
	for (; step < 5; step++) addch (ACS_HLINE);
	addch(ACS_URCORNER);
	for (step = 1; step < 4; step++)
	{
	mvaddch(step + y, x, ACS_VLINE);
	mvaddch(step + y, x + 6, ACS_VLINE);
	}
	mvprintw(y + 2, x + 3, "+");
	mvaddch(y + 4, x, ACS_LLCORNER);
	for (step = 0; step < 5; step++) addch (ACS_HLINE);
	addch(ACS_LRCORNER);
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

static void stick(short sx, short sy, int cx, int cy, char* ch)
{
	int x = -1;
	for (; x < 2; x++)
		mvprintw(cy + x, cx - 2, "     ");
	
	mvprintw(cy - sy / 16385, cx + sx / 16383, ch);	
}

static void trigger(char vcalue, int cx, int cy)
{
	int y = cy;
	for (; y > cy - 4; y--)
		mvprintw(y, cx, "  ");
	for (y = cy - 1; y > cy - vcalue / 63; y--)
		mvprintw(y, cx, "++");	
}

int getxpad(libusb_device_handle* h)
{
	int transferred;
	unsigned char data[32];
	int error = libusb_interrupt_transfer(h, 0x81, data, sizeof data, &transferred, 0);
	if (error != 0) return (error);
	int i = 0;
	for (; i < transferred; i++)
	{
		if (i > 14) break;
		if (i != 0) printw(".");
		else { mvprintw(0, 0, "%02X", data[i]); continue; }
		printw("%02X", data[i]);				
	}
	///============================================
	///	Up Down Left Reft
	///============================================
	int x = 53, y = 5;
	move(y - 1, x);
	(data[2] & 0x01) > 0 ? printw("^") : printw(".");
	move(y, x - 2);
	(data[2] & 0x04) > 0 ? printw("<   ") : printw(".   ");
	(data[2] & 0x08) > 0 ? printw(">") : printw(".");
	move(y + 1, x);
	(data[2] & 0x02) > 0 ? printw("v") : printw(".");	
	///============================================
	/// Centre 3 buttons
	///============================================
	x += 4;
	move(y, x);
	(data[2] & 0x20) > 0 ? printw("Bk ") : printw(".. ");
	(data[3] & 0x04) > 0 ? printw("Xb ") : printw(".. ");
	(data[2] & 0x10) > 0 ? printw("St") : printw("..");
	///============================================
	/// XYAB
	///============================================
	x += 11;
	move(y - 1, x);
	(data[3] & 0x80) > 0 ? printw("Y") : printw(".");
	move(y, x - 2);
	(data[3] & 0x40) > 0 ? printw("X   ") : printw(".   ");
	(data[3] & 0x20) > 0 ? printw("B") : printw(".");
	move(y + 1, x);
	(data[3] & 0x10) > 0 ? printw("A") : printw(".");
	///============================================
	/// L1 R1
	///============================================
	move(3, 66);
	(data[3] & 0x02) > 0 ? printw("R1") : printw("..");
	move(3, 54);
	(data[3] & 0x01) > 0 ? printw("L1") : printw("..");	
	///============================================
	/// triggers
	///============================================
	trigger(data[4], 52, 3);
	move(3, 52);
	data[4] > 0 ? printw("L2") : printw("..");
	trigger(data[5], 68, 3);
	move(3, 68);
	data[5] > 0 ? printw("R2") : printw("..");
	///============================================
	/// sticks
	///============================================
	char* lc; char* rc;
	rc = (data[2] & 0x80) > 0 ? "@" : "+";
	lc = (data[2] & 0x40) > 0 ? "@" : "+";
	short lx, ly;
	lx = data[6];
	lx |= (data[7] << 8);
	ly = data[8];
	ly |= (data[9] << 8);
	stick(lx, ly, 54, 9, lc);	
	short rx, ry;
	rx = data[10];
	rx |= (data[11] << 8);
	ry = data[12];
	ry |= (data[13] << 8);
	stick(rx, ry, 67, 9, rc);
	return 0;
	//printf(" ");
}

int led(libusb_device_handle* h, int n)
{
	int transferred;
	unsigned char data[] = { 1, 3, (unsigned char)n };	
	int error = libusb_interrupt_transfer(h, 0x02, data, sizeof data, &transferred, 0);	
	return error;
}

int rumbler(libusb_device_handle* h, int n)
{
	int transferred;
	unsigned char b = ((unsigned char)n << 4);
	unsigned char data[] = { 0, 8, 0, 0, b, 0, 0, 0 };	
	int error = libusb_interrupt_transfer(h, 0x02, data, sizeof data, &transferred, 0);	
	return error;
}

int 
main(void)
{
	libusb_device_handle *h;
	int error;

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
    
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    
    attron(COLOR_PAIR(1));
    mvprintw(14, 5, "This ");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
    printw("is ");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(2));
    printw("a ");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(6));
    printw("color");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(4));
    printw("ful ");
    attroff(COLOR_PAIR(4));
    attron(COLOR_PAIR(5));
    printw("string");
    attroff(COLOR_PAIR(5));
    mvprintw(15, 5, "Press 'a' and 's' to change the LED");
    mvprintw(16, 5, "Press 'z' and 'x' to change the rumbler");

	DrawBox(51, 7);	
	DrawBox(64, 7);
	GetSetTime();
    mvprintw(3,5,"press a key ('q' to quit)");
    mvprintw(4,5,"%d", LIBUSB_TRANSFER_ERROR);
    
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
			getxpad(h);
        }
		/**
		 * Set the LED
		 */
        else if (ch == 'a')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (--v_led < 0) v_led = 12;
			error = led(h, v_led);
        }
		else if (ch == 's')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (++v_led > 12) v_led = 0;
			error = led(h, v_led);
        }
		/**
		 * Set the rumbler
		 */
        else if (ch == 'z')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (--v_rumbler < 0) v_rumbler = 15;
			error = rumbler(h, v_rumbler);
        }
		else if (ch == 'x')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (++v_rumbler > 15) v_rumbler = 0;
			error = rumbler(h, v_rumbler);
        }
		if (error > 0) 
		{
			endwin();
			fprintf(stderr, "Transfer failed: %d\n", error);
			return (1);
		}		
    }
    
    endwin();           /* End curses mode        */

    return (0);
}

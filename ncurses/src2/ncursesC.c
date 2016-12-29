#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <libusb-1.0/libusb.h>

typedef enum
{
    STATE_INITIAL,
    STATE_ACTIVE,
    STATE_STOP
} STATE_t;

typedef enum
{
    EVENT_NONE,
    EVENT_EXIT,
    EVENT_START,
    EVENT_UP,
    EVENT_DOWN,
    EVENT_LEFT,
    EVENT_RIGHT
} EVENT_t;

typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} DIR_t;
//
EVENT_t getEvent(libusb_device_handle* h, unsigned char* data)
{
    EVENT_t ev = EVENT_NONE;
    char ch = getch();
    
    switch (ch)
    { 
        case 113:
            ev = EVENT_EXIT;
            break;
        case 119:
            ev = EVENT_UP;
            break;
        case 97:
            ev = EVENT_LEFT;
            break;
        case 115:
            ev = EVENT_DOWN;
            break;
        case 100:
            ev = EVENT_RIGHT;
            break;
        case 32:
            ev = EVENT_START;
            break;
    }
    if (h != NULL)
    {
        int error = getxpad(h, data);
        if (error > 0) return ev;
        if ((data[2] & 0x20) > 0) ev = EVENT_EXIT;
        if ((data[2] & 0x01) > 0) ev = EVENT_UP;
        if ((data[2] & 0x04) > 0) ev = EVENT_LEFT;
        if ((data[2] & 0x08) > 0) ev = EVENT_RIGHT;
        if ((data[2] & 0x02) > 0) ev = EVENT_DOWN;
        if ((data[2] & 0x10) > 0) ev = EVENT_START;
    }
    //mvprintw(4, 2, "%d", ch);
    return (ev);
}


static void
DrawBox()
{
	int step = 1;
	// this is only to demonstrate the characters to build a box
	// this code should be modified to draw a box of arbitrary size
	mvaddch(0, 0, ACS_ULCORNER);
	for (; step < 79; step++) addch (ACS_HLINE);
	addch(ACS_URCORNER);
	for (step = 1; step < 23; step++)
	{
	mvaddch(step, 0, ACS_VLINE);
	mvaddch(step, 79, ACS_VLINE);
	}
	//mvprintw(23, x + 3, "+");
	mvaddch(23, 0, ACS_LLCORNER);
	for (step = 1; step < 79; step++) addch (ACS_HLINE);
	addch(ACS_LRCORNER);
}
		/*
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
	/
	myTime.tm_year++;
	myTime.tm_hour += 2;
	myRawTime = mktime(&myTime);	
	result = stime(&myRawTime);
	*
	
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
*/
static void clean()
{
    //int x = 1;
    int y = 1;
	for (; y < 23; y++)
    {
        move(y, 1);
        int x = 1;
        for (; x < 79; x++)
            printw(" ");
    }
}
/*
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
*/

int getxpad(libusb_device_handle* h, unsigned char* data)
{
	int transferred;
	//unsigned char data[32];
	int error = libusb_interrupt_transfer(h, 0x81, data, sizeof data, &transferred, 0);
	return (error);
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

void connect(libusb_device_handle* h)
{
    int k = libusb_init(NULL);//233
	if (k != 0) printf("%d\n", k);
	h = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);
}

void reset_item(short* snake, int* length, short* itempos)
{
    int itemx = 1 + rand() % 39;
    int itemy = 1 + rand() % 22;
    mvprintw(0, 15, "%02d, %02d", itemx, itemy); 
    int i = 0;
    for (; i < *length; i++)
    {
        int snakeX = ((snake[i] & 0xFF00) >> 8) & 0xFF;
        int snakeY = snake[i] & 0x00FF;
        if (snakeX == itemx)
        {
            itemx = 1 + rand() % 36;
            i = 0;
        }
        if (snakeY == itemy)
        {
            itemy = 1 + rand() % 21;
            i = 0;
        }
    } 
    short ipos = itemx << 8 | itemy;
    *itempos = ipos; 
}

void snake_init(short* snake)
{
    snake[0] = 1283;
    snake[1] = 1027;
    snake[2] = 771;
}

void draw_snake(short* snake, int length, int itempos)
{
	clean();
    int i = 0;
    for (; i < length; i++)
    {
        int snakeX = ((snake[i] & 0xFF00) >> 8) & 0xFF;
        int snakeY = snake[i] & 0x00FF;
        if (i == 0) {mvprintw(1, 72, "%04x", snake[i]);
         mvprintw(1, 62, "%d, %d", snakeX, snakeY);} 
        mvaddch(snakeY, snakeX * 2 - 1, ACS_BLOCK);
        addch(ACS_BLOCK);
    }
    int itemX = ((itempos & 0xFF00) >> 8) & 0xFF;
    int itemY = itempos & 0x00FF;
    mvprintw(itemY, itemX * 2 - 1, "$$");    
    mvprintw(1, 2, "%d", length);
}

void snake_eat(short* snake, int* length, short* itempos)
{
    (*length)++;
    int i = *length;
    for (; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }
    snake[0] = *itempos;
    reset_item(snake, length, itempos);
    draw_snake(snake, *length, *itempos);
}

STATE_t snake_move(short* snake, int* length, DIR_t dir, short* itempos)
{
    short newdir = snake[0];
    switch (dir)
    {
        case DIR_UP:
            if ((newdir & 0x001F) == 1) { mvprintw(1, 10, "up"); return STATE_STOP; }
            newdir--;
            break;
        case DIR_DOWN:
            if ((newdir & 0x001F) == 22) return STATE_STOP;
            newdir++;
            break;
        case DIR_LEFT:
            if ((newdir & 0x3F00) == 256) { mvprintw(1, 10, "left"); return STATE_STOP; }
            newdir -= 256;
            break;
        case DIR_RIGHT:
            if ((newdir & 0x3F00) == 9984) return STATE_STOP;
            newdir += 256;
            break;
    }
    if (newdir == snake[1]) return STATE_ACTIVE;
    if (newdir == *itempos)
    {
        snake_eat(snake, length, itempos);
        return STATE_ACTIVE;
    }
    int i = 0;
    for (; i < *length; i++)
    {
        if (newdir == snake[i]) { mvprintw(1, 10, "self %04x %04x %d", newdir, snake[i], i); return STATE_STOP; }
    }
    for (; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }
    snake[0] = newdir;
    draw_snake(snake, *length, *itempos);
    //mvprintw(1, 70, "%d", newdir);
    return STATE_ACTIVE;
}

int 
main(void)
{
	libusb_device_handle *h = NULL;
	int error = 0;
    STATE_t st = STATE_INITIAL;
    EVENT_t ev = EVENT_NONE;
    
    DIR_t d;
    short snake[897];
    short itempos = 3333;
    int length;
    
    connect(h);
	
	//int v_led = 0, v_rumbler = 0;
	
    //int     ch;
    //unsigned int     i = 0;
    //char    x[] = "|/-\\";    
	//unsigned char data[32];
	
    initscr();              /* Start curses mode        */
    raw();                  /* Line buffering disabled  */
    nodelay(stdscr, TRUE);  /* getch() returns immediately      */
    noecho();               /* no cursor */
    curs_set(0);			/* no cursor */
    DrawBox();
    mvprintw(2, 2, "SNAKE - Embedded Linux Assignment G");
    mvprintw(3, 2, "===================================");
    mvprintw(4, 2, "Use direction buttons on your gamepad");
    mvprintw(5, 2, "You can go to the space where the score and position shows");
    mvprintw(6, 2, "Connect your gamepad and press start");

    //snake = (short*) malloc(3 * sizeof(short));
    
    /*
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
    
    ch = getch();          / If raw() hadn't been called
                             * we have to press 'enter' before it
                             * gets to the program      
                             */

    while (true)
    {
        ev = getEvent(h, data);
        switch (st)
        {
            case STATE_STOP:
                mvprintw(2, 2, "You died! Score: %d", length);
                mvprintw(3, 2, "Press start");
                if (h != NULL) 
                    error = led(h, 10);
            case STATE_INITIAL:
                if (h != NULL && st = STATE_INITIAL) 
                    error = led(h, 1);
                switch (ev)
                {
                    case EVENT_START:
						DrawBox();
                        length = 3;
                        snake_init(snake);
						reset_item(snake, &length, &itempos);
                        draw_snake(snake, length, itempos);
                        st = STATE_ACTIVE;
                        break;
                    case EVENT_EXIT:
                        endwin();
                        return (0);
                        break;
                    default: break;
                }  
                break;
            case STATE_ACTIVE:
                if (h != NULL) 
                    error = led(h, 0);
                switch (ev)
                {
                    case EVENT_UP:
                        d = DIR_UP;
                        break;
                    case EVENT_DOWN:
                        d = DIR_DOWN;
                        break;
                    case EVENT_LEFT:
                        d = DIR_LEFT;
                        break;
                    case EVENT_RIGHT:
                        d = DIR_RIGHT;
                        break;
                    default: break;
                }
                if (ev <= 3) st = snake_move(snake, &length, d, &itempos);
                break;
        }
        if (h == NULL)
        {
            mvprintw(21, 2, "WARNING: gamepad is not connected");
            connect(h);
        }
        usleep(9000);
        //ev = EVENT_NONE;
    }

    /*
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
		/
		 * Set the LED
		 /
        else if (ch == 'a')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (--v_led < 0) v_led = 12;
        }
		else if (ch == 's')
        {
            // show pressed key
            //mvprintw(5,13,"%c (%02x)     ", ch, ch);
			if (++v_led > 12) v_led = 0;
			error = led(h, 10);
        }
		/
		 * Set the rumbler
		 /
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
    
    endwin();           / End curses mode        */

    return (0);
}

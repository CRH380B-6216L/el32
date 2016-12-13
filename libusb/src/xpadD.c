/*
 * de volgende code laat het ledje op de gamepad rondspinnen:
*/

#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
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
	
	if (argc <= 1)
	{		
		//unsigned char data[32];
		while (1)
		{
			unsigned char data[32];
			error = libusb_interrupt_transfer(h, 0x81, data, sizeof data, &transferred, 0);
			if (error != 0) 
			{
				fprintf(stderr, "Transfer failed: %d\n", error);
				return (1);
			}	
			
			//printf("transferred: %d\n", transferred);
			int i = 0;
			for (; i < transferred; i++)
			{
				if (i > 14) break;
				if (i != 0) printf(".");
				printf("%02X", data[i]);				
			}
			printf(" ");
			(data[2] & 0x80) > 0 ? printf("R3") : printf("  ");
			(data[2] & 0x40) > 0 ? printf("L3") : printf("  ");
			(data[2] & 0x20) > 0 ? printf("Bk") : printf("  ");
			(data[2] & 0x10) > 0 ? printf("St") : printf("  ");
			(data[2] & 0x08) > 0 ? printf(">") : printf(" ");
			(data[2] & 0x04) > 0 ? printf("<") : printf(" ");
			(data[2] & 0x02) > 0 ? printf("v") : printf(" ");
			(data[2] & 0x01) > 0 ? printf("^") : printf(" ");
			(data[3] & 0x80) > 0 ? printf("Y") : printf(" ");
			(data[3] & 0x40) > 0 ? printf("X") : printf(" ");
			(data[3] & 0x20) > 0 ? printf("B") : printf(" ");
			(data[3] & 0x10) > 0 ? printf("A") : printf(" ");
			(data[3] & 0x04) > 0 ? printf("Xb") : printf("  ");
			(data[3] & 0x02) > 0 ? printf("R1") : printf("  ");
			(data[3] & 0x01) > 0 ? printf("L1") : printf("  ");			
			data[4] > 0 ? printf("L2%02X", data[4]) : printf("    ");
			data[5] > 0 ? printf("R2%02X", data[5]) : printf("    ");
			short lx, ly;
			lx = data[6];
			lx |= (data[7] << 8);
			ly = data[8];
			ly |= (data[9] << 8);
			printf(" Ls(%d,%d)", lx, ly);
			short rx, ry;
			rx = data[10];
			rx |= (data[11] << 8);
			ry = data[12];
			ry |= (data[13] << 8);
			printf(" Rs(%d,%d)", rx, ry);
			printf("\n");
			//printf("Hello, world! (try --help)\n");
		}
	}
	else
	{
		char flag = 0;
		int i = 0;
		for (i = 1; i < argc; i++) 
		{
			int n = atoi(argv[i]);
			if (n >= 0 && flag != 0)
			{
				if ((flag &= 0xf0) != 0)
				{
					//if (n > 13)
					unsigned char data[] = { 1, 3, (unsigned char)n };	
					error = libusb_interrupt_transfer(h, 0x02, data, sizeof data, &transferred, 0);	
				}
				else // if ((flag &= 0x0f) > 0)
				{
					unsigned char b = ((unsigned char)n << 4);
					unsigned char data[] = { 0, 8, 0, 0, b, 0, 0, 0 };	
					error = libusb_interrupt_transfer(h, 0x02, data, sizeof data, &transferred, 0);	
				}
				if (error != 0) 
				{
					fprintf(stderr, "Transfer failed: %d\n", error);
					return (1);
				}				
				flag = 0xff;
				n = 0;
			}
			if (strcmp(argv[i], "--help") == 0)
			{
				printf("(no parameters)  show the value of variables of the gamepad\n");
				printf("  -l  [00 - 13]  change the lighting\n");
				printf("  -r  [00 - 15]  change the rumbler\n");
				break;
			}
			else if (strcmp(argv[i], "-l") == 0)
				flag |= 0xf0;
			else if (strcmp(argv[i], "-r") == 0)
				flag |= 0x0f;
			else
				if ((flag &= 0xff) == 0) printf("Unidentified arg %s! Check --help!\n", argv[i]);
				else flag = 0;
		}
	}
	return (0);
}


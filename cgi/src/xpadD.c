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


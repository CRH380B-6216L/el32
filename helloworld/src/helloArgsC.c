/* Hello World program in C */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
int main(int argc, char* argv[])
{
	int i = 0;
	printf("\ncmdline args count=%d", argc);

	/* First argument is executable name only */
	printf("\nexe name=%s", argv[0]);

	for (i = 1; i < argc; i++) 
	{
		printf("\narg%d=%s", i, argv[i]);
	}
	printf("\n");
	
	if (argc <= 1)
	{
		if (strcmp(argv[0], "ochtend") == 0) 
			printf("Goedemorgen (try --help)\n");
		else if (strcmp(argv[0], "middags") == 0) 
			printf("Goedenavond (try --help)\n");
		else 
		    printf("Hello, world! (try --help)\n");
	}
	else
	{
		char flag = 0;
		for (i = 1; i < argc; i++) 
		{
			int n = atoi(argv[i]);
			if (n > 1 && flag != 0)
			{
				int a = 1;
				if ((flag &= 0xf0) != 0)
					for (a = 1; a < n; a++) printf("Goedemorgen\n");
				else // if ((flag &= 0x0f) > 0)
					for (a = 1; a < n; a++) printf("Goedenavond\n");					
				flag &= 0;
				n = 0;
				continue;
			}
			if (strcmp(argv[i], "--help") == 0)
			{
				printf("  -m  [int >= 2]  Says 'Goedemorgen' for specefic times. Say once if the integer omitted\n");
				printf("  -e  [int >= 2]  Says 'Goedenavond' for specefic times. Say once if the integer omitted\n");
				break;
			}
			else if (strcmp(argv[i], "-m") == 0)
			{
				flag |= 0xf0;
				printf("Goedemorgen\n");
			}
			else if (strcmp(argv[i], "-e") == 0)
			{
				flag |= 0x0f;
				printf("Goedenavond\n");
			}
			else
			{
				printf("Unidentified arg %s!\n", argv[i]);
			}
		}
	}
	return 0;
}

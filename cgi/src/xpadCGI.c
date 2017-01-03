#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char* p;
  char* s1;
  int n = 0;
  char result[12] = "xpadD2 ";
  printf("Content-Type: text/html\n\n");
  printf("<html>\n<head>\n<title>xpadCGI</title></head><body>");
  p = getenv("QUERY_STRING");
  //p = "rumbler=5";
  if (p != NULL && *p != '\0')
  {
    if (strncmp(p, "led=", 4) == 0) 
    { 
		s1 = "-l"; 
		sscanf(p, "led=%d", &n);
    }
    if (strncmp(p, "rumbler=", 8) == 0) 
    { 
		s1 = "-r"; 
		sscanf(p, "rumbler=%d", &n);
	}
    sprintf(result, "xpadD2 %s %d", s1, n);
    system((const char*)result);
    printf("<p>%s</p>", result); 
  } 
  printf("<h4>LED setting</h4><p>");
  int i = 0;
  for (; i < 13; i++)
    printf(" <a href='?led=%d'>%d</a>", i, i);
  i = 0;
  printf("</p><h4>rumbler setting</h4><p>");
  for (; i < 16; i++)
    printf("<a href='?rumbler=%d'>%d</a>&nbsp;", i, i);
  printf("</p></body></html>");
  return (0);
}

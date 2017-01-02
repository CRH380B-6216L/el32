#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  const char* p;
  char* buffer;
  const char* delim = {"="};
  char* s1;
  char* s2;
  printf("Content-Type: text/html\n\n");
  printf("<html>\n<head>\n<title>CGI</title></head><body><h4>LED setting</h4><p>");
  if ((p = getenv("QUERY_STRING")) != NULL && *p != '\0')
  {
    buffer = strtok(p, delim);
    if (strcmp(buffer, "led") == 0) s1 = "-l";
    if (strcmp(buffer, "rumbler") == 0) s1 = "-r";
    buffer = strtok(NULL, delim);
    s2 = buffer;
    system("xpadD2 %s %s", s1, s2);
  }  
  int i = 0;
  for (; i < 13; i++)
    printf("<a href='?led=%d'>%d</a>", i);
  i = 0;
  printf("</p><h4>LED setting</h4><p>&nbsp;");
  for (; i < 15; i++)
    printf("<a href='?rumbler=%d'>%d</a>&nbsp;", i);
  printf("</p></body></html>");
  return (0);
}
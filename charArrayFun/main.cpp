
#include <stdio.h>
#include <string.h>
#include <iostream>
//#include <stdafx.h>

int main()
{
	bool IPFind = false;
	char buff[] = "0000000\0";

	char html[] = "Location: http://68.230.107.205:5211/getIP?d=123123";
	char idStr[] = "http://";
	char c;

	int curState = 0;
	for (int kk = 0; kk < strlen(html); kk++)
	{
		c = html[kk];
		buff[6] = c;
		memmove(buff, &(buff[1]), strlen(&(buff[1])));
		
		printf("%s\r\n", buff);

		if (!strcmp(buff, idStr)) {
			IPFind = true;
			printf("HEre");
		}



	}

	return 0;
}
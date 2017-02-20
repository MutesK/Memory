// ConsoleApplication1.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "my_new.h"


#define new new(__FILE__, __LINE__)


int main()
{
	int *A = new int;
	int *B = new int;
	int *C = new int[10];

	delete C;
	
	return 0;
}

// ConsoleApplication1.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

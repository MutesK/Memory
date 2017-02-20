// ConsoleApplication1.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#define _MemoryAlloc(TYPE, X) Alloc<TYPE>(X, __FILE__, __LINE__)


typedef struct _MEM
{
	int Size;
	int line;
	char *fileName;
	void *Array;
} Memory;

int totalAllocCount = 0;
int totalAllocSize = 0;
Memory LeakTest[100];
int index = 0;



template <typename X>
X* Alloc(int count, char *fileName, int Line)
{
	int size = sizeof(X) * count;
	X* p = new X[count];


	if (index <= 100)
	{
		LeakTest[index].Size = size;
		totalAllocSize += size;

		LeakTest[index].line = Line;
		int len = strlen(fileName);
		LeakTest[index].fileName = new char[len + 1];
		memcpy(LeakTest[index].fileName, fileName, len);
		LeakTest[index].fileName[len] = '\0';

		LeakTest[index].Array = p;
		 
		totalAllocCount++;
		index++;

		return p;
	}

}

template <typename X>
void MemoryRelease(X *Xp)  // -> �ڵ����� �Ѿ��.
{
	// ����ü �迭�� ������ �ּҰ��� �����Ѵٸ� �����. 
	//���ٸ� ��ŵ.
	for (int i = 0; i < 100; i++)
	{
		if (LeakTest[i].Array != nullptr)
		{
			if ((X *)LeakTest[i].Array == Xp)
			{
				delete[]LeakTest[i].Array;
				LeakTest[i].Array = nullptr;
				delete[] LeakTest[i].fileName;
				return;
			}
		}
	}
}


void PrintAlloc()
{
	printf("Total Alloc Size : %d \nTotal Alloc Count : %d\n\n",
		totalAllocSize, totalAllocCount);

	for (int i = 0; i < 100; i++)
	{
		if (LeakTest[i].Array != nullptr)
		{
			printf("Not Release Memory : [%p]  %d Bytes \n", LeakTest[i].Array,
				LeakTest[i].Size);
			printf("File  %s  : %d \n\n", LeakTest[i].fileName, LeakTest[i].line);
		}
	}

}
int main()
{
	int *p = _MemoryAlloc(int, 10);
	char *p1 = _MemoryAlloc(char, 10);
	float *p2 = _MemoryAlloc(float, 3);
	char *p3 = _MemoryAlloc(char, 4);
	char *p4 = _MemoryAlloc(char, 8);
	int *p5 = _MemoryAlloc(int, 10);

	MemoryRelease(p);


	PrintAlloc();

	return 0;
}

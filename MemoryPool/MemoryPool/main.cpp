#include "MemoryPool.h"
#include "Profiler.h"
#include <stdio.h>


class Num
{
private:
	int A;
public:
	Num(){
	}
	Num(int i)
	{
		A = i;
	}
	int GetI()
	{
		return A;
	}
	void SetI(int a)
	{
		A = a;
	}
	~Num()
	{
	}
};

CMemoryPool<Num>Memory(10000, true);
int main()
{
	
	Num* A[10000];

	Num* B[10000];
	
	for (int i = 0; i < 10000; i++)
	{
		PRO_BEGIN("MemoryPool Alloc");
		A[i] = Memory.Alloc();
		PRO_END("MemoryPool Alloc");
	}
	
	for (int i = 0; i < 10000; i++)
	{
		PRO_BEGIN("Standard New");
		B[i] = new Num;
		PRO_END("Standard New");
	}
		
	for (int i = 0; i < 10000; i++)
	{
		PRO_BEGIN("MemoryPool Free");
		Memory.Free(A[i]);
		PRO_END("MemoryPool Free");
	}
	
	for (int i = 0; i < 10000; i++)
	{
		PRO_BEGIN("Standard Delete");
		delete B[i];
		PRO_END("Standard Delete");
	}
	
	ProfileOutText("MemoryPoolPerformanceReport.txt");
	
	return 0;
}

/*
	메모리
	포인터 연산할때 4바이트 또는 8바이트씩 이동하는데
	메모리에서 1바이트가 16진수하나를 의미함.

*/
#pragma once

/*
  삭제 기능에 대해 추가해줘야 한다. 

  // 나중에 이걸 사용할꺼면 cpp에 전부 넣고 확인, 다 썻다면 프로젝트소스에서 제거해야됨.

  - 하나로 합쳐라
*/
class CMemory
{
public:
	enum
	{
		NOALLOC = 0,
		ARRAY = 1,
		LEAK = 2
	};

	typedef struct _MEM
	{
		int Size;
		int line;
		char* fileName;
		void *Pointer;
		bool isArray;
	} Mem;

	typedef struct _error
	{
		int Type;
		void *Pointer;
		int size;
		int line;
		char *fileName;
	} Error;

private:
	Mem Data[100];
	Error errData[100];
	char *errStr;
	int totalAllocCount;
	int totalAllocSize;
	int memindex;
	int errindex;
	
	friend void * operator new (size_t size, char *File, int Line);
	friend void * operator new[](size_t size, char *File, int Line);
	friend void operator delete (void * p);
	friend void operator delete[](void * p);


public:
	CMemory();
	~CMemory();
	void AddMemory(void *p, char * fileName, int fileSize, int line, bool isArray = false);
	void DeleteMemory(void *pointer, bool isArray = false);

	void AddError(int type, void *pointer, int size = 0, int line = 0, char *fileName = nullptr);
	void printError();

	
};

void operator delete (void * p, char *File, int Line);
void operator delete[](void * p, char *File, int Line);
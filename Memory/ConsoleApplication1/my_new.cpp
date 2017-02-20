#include "stdafx.h"
#include "my_new.h"
using namespace std;

CMemory Management;

CMemory::CMemory()
{
	totalAllocCount = 0;
	totalAllocSize = 0;
	memindex = 0;
	errindex = 0;

	for (int i = 0; i < 100; i++)
	{
		Data[i].Size = 0;
		Data[i].line = 0;
		Data[i].fileName = nullptr;
		Data[i].Pointer = nullptr;
		Data[i].isArray = false;

		errData[i].fileName = nullptr;
		errData[i].Pointer = nullptr;
	}
}


CMemory::~CMemory()
{
	/*
	메모리관리 클래스의 파괴자에서 아래 내용을 로그로 남김
	할당 후 파괴가 안된 메모리 / 오류코드가 남은 할당정보.
	*/

	for (int i = 0; i < 100; i++)
	{
		if (Data[i].Pointer != nullptr)
			AddError(LEAK, Data[i].Pointer, Data[i].Size, Data[i].line, Data[i].fileName);
	}

	errStr = (char *)malloc((errindex + 1) * 600);
	memset(errStr,'\0', (errindex + 1) * 600);
	char *errStrindex = errStr;
	for (int i = 0; i < errindex; i++)
	{
		switch (errData[i].Type)
		{
		case NOALLOC:
			sprintf_s(errStrindex, 600, "NOALLOC [%x]\r\n", errData[i].Pointer);

			break;
		case ARRAY:
			sprintf_s(errStrindex, 600, "ARRAY [%x] [%10d] %s : %d\r\n", errData[i].Pointer,
				errData[i].size, errData[i].fileName, errData[i].line);

			break;
		case LEAK:
			sprintf_s(errStrindex, 600, "LEAK [%x] [%10d] %s : %d\r\n", errData[i].Pointer,
				errData[i].size, errData[i].fileName, errData[i].line);

			break;
		}
		errStrindex += 600;
	}

	if(errindex != 0)
		printError();
}

void CMemory::AddMemory(void *p, char * fileName, int fileSize, int line,
	bool isArray)
{
	int len = strlen(fileName);

	if (memindex >= 100)
	{
		cout << "배열이 꽉참 \n";
		return;
	}

	Data[memindex].fileName = (char *)malloc(len + 1);
	strcpy_s(Data[memindex].fileName, len + 1, fileName);

	Data[memindex].Pointer = p;
	Data[memindex].Size = fileSize;
	Data[memindex].line = line;
	Data[memindex].isArray = isArray;


	totalAllocCount++;
	totalAllocSize += fileSize;
	memindex++;
}

void * operator new (size_t size, char *File, int Line)
{
	void *p = malloc(size);
	Management.AddMemory(p, File, size, Line);

	return p;
}
void * operator new[](size_t size, char *File, int Line)
{
	void *p = malloc(size);
	Management.AddMemory(p, File, size, Line, true);

	return p;
}
/*
delete 를 오버로딩하여 메모리 삭제시 오류 확인.
(실제 할당된적이 있는 포인터인지, 배열형태는 맞는지)

3번에서 문제가 없었다면 할당내역 삭제
3번에서 오류가 났다면 해당 메모리 할당 정보에 오류코드 저장
*/

void CMemory::DeleteMemory(void *pointer, bool isArray)
{
	// 오류 확인 1
	// 실제 할당된 적이 있는가
	if (isArray)
	{
		// 배열상태
		for (int i = 0; i < 100; i++)
		{
			if (Data[i].Pointer != nullptr)
			{
				if (Data[i].Pointer == pointer)
				{
					// 이녀석이 배열 형태인가?
					if (Data[i].isArray)
					{
						// 지운다.
						free(Data[i].Pointer);
						return;
					}
					else
					{
						//ARRAY - new[] / delete[] 배열형태 생성과 삭제가 잘못됐을 때
						AddError(ARRAY, Data[i].Pointer, Data[i].Size, Data[i].line, Data[i].fileName);
					}
				}
			}
		}

		// NOALLOC - 메모리 해제시 할당되지 않은 포인터 입력.
		AddError(NOALLOC, pointer);
	}
	else
	{
		for (int i = 0; i < 100; i++)
		{
			if (Data[i].Pointer != nullptr)
			{
				if (Data[i].Pointer == pointer)
				{
					if (!Data[i].isArray)
					{
						// 지운다.
						free(Data[i].Pointer);
						return;
					}
					else
					{
						// ARRAY - new[] / delete[] 배열형태 생성과 삭제가 잘못됐을 때
						AddError(ARRAY, Data[i].Pointer, Data[i].Size, Data[i].line, Data[i].fileName);
					}
				}
			}
		}

		// NOALLOC - 메모리 해제시 할당되지 않은 포인터 입력.
		AddError(NOALLOC, pointer);
	}
}
void operator delete (void * p)
{
	Management.DeleteMemory(p);
}
void operator delete[](void * p)
{
	Management.DeleteMemory(p, true);
}


void CMemory::AddError(int type, void *pointer, int size , int line, char *fileName)
{
	for (int i = 0; i < errindex; i++)
	{
		if (pointer == errData[i].Pointer)
			return;
	}

	errData[errindex].Type = type;
	errData[errindex].Pointer = pointer;

	if (fileName != nullptr)
	{
		errData[errindex].fileName = fileName;
		errData[errindex].size = size;
		errData[errindex].line = line;
	}

	errindex++;
}

void CMemory::printError()
{

	FILE *fp;
	__time64_t long_time;
	_time64(&long_time);
	struct tm *newtime = _localtime64(&long_time);

	//  Alloc_YYYYMMDD_HHMMSS.txt 
	char fpName[50];
	sprintf_s(fpName, 50, "Alloc_%4d%02d%02d__%02d%02d%02d.txt", newtime->tm_year + 1900, newtime->tm_mon,
		newtime->tm_mday, newtime->tm_hour, newtime->tm_min, newtime->tm_sec);

	fopen_s(&fp, fpName, "wb");

	if (fp != nullptr)
	{
		char *index = errStr;
		for (int i = 0; i < errindex; i++)
		{
			fwrite(index, strlen(index), 1, fp);
			index += 600;
		}
		fclose(fp);
	}
}

// 동작하지 않은 delete
void operator delete (void * p, char *File, int Line)
{

}
void operator delete[](void * p, char *File, int Line){}



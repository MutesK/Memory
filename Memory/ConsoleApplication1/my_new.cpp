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
	�޸𸮰��� Ŭ������ �ı��ڿ��� �Ʒ� ������ �α׷� ����
	�Ҵ� �� �ı��� �ȵ� �޸� / �����ڵ尡 ���� �Ҵ�����.
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
		cout << "�迭�� ���� \n";
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
delete �� �����ε��Ͽ� �޸� ������ ���� Ȯ��.
(���� �Ҵ������ �ִ� ����������, �迭���´� �´���)

3������ ������ �����ٸ� �Ҵ系�� ����
3������ ������ ���ٸ� �ش� �޸� �Ҵ� ������ �����ڵ� ����
*/

void CMemory::DeleteMemory(void *pointer, bool isArray)
{
	// ���� Ȯ�� 1
	// ���� �Ҵ�� ���� �ִ°�
	if (isArray)
	{
		// �迭����
		for (int i = 0; i < 100; i++)
		{
			if (Data[i].Pointer != nullptr)
			{
				if (Data[i].Pointer == pointer)
				{
					// �̳༮�� �迭 �����ΰ�?
					if (Data[i].isArray)
					{
						// �����.
						free(Data[i].Pointer);
						return;
					}
					else
					{
						//ARRAY - new[] / delete[] �迭���� ������ ������ �߸����� ��
						AddError(ARRAY, Data[i].Pointer, Data[i].Size, Data[i].line, Data[i].fileName);
					}
				}
			}
		}

		// NOALLOC - �޸� ������ �Ҵ���� ���� ������ �Է�.
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
						// �����.
						free(Data[i].Pointer);
						return;
					}
					else
					{
						// ARRAY - new[] / delete[] �迭���� ������ ������ �߸����� ��
						AddError(ARRAY, Data[i].Pointer, Data[i].Size, Data[i].line, Data[i].fileName);
					}
				}
			}
		}

		// NOALLOC - �޸� ������ �Ҵ���� ���� ������ �Է�.
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

// �������� ���� delete
void operator delete (void * p, char *File, int Line)
{

}
void operator delete[](void * p, char *File, int Line){}



// mutex.cpp: определяет точку входа для консольного приложения.
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms686927(v=vs.85).aspx

/*
В следующем примере функция CreateMutex используется для создания объекта mutex и 
функции CreateThread для создания рабочих потоков.

Когда поток этого процесса записывается в базу данных, он сначала запрашивает владение мьютексом, 
используя функцию WaitForSingleObject. Если поток получает право владения мьютексом, он записывает
в базу данных, а затем освобождает свою собственность на мьютекс, используя функцию ReleaseMutex.

В этом примере используется структурированная обработка исключений, чтобы убедиться, что поток
правильно освобождает объект mutex. Блок кода __finally выполняется независимо от того,
как заканчивается __try-блок (если только блок __try не включает вызов функции TerminateThread).
Это предотвращает непреднамеренный отказ объекта мьютекса.

Если мьютекс заброшен, поток, которому принадлежит мьютекс, не отпустил его до завершения.
В этом случае состояние общего ресурса является неопределенным, и продолжение использования мьютекса
может скрыть потенциально серьезную ошибку. Некоторые приложения могут попытаться восстановить ресурс
в согласованное состояние; этот пример просто возвращает ошибку и перестает использовать мьютекс. 

*/

#include "stdafx.h"

#include <windows.h>
#include <stdio.h>

#define THREADCOUNT 2

HANDLE ghMutex;

DWORD WINAPI WriteToDatabase(LPVOID);

int main(void)
{
	HANDLE aThread[THREADCOUNT];
	DWORD ThreadID;
	int i;

	// Create a mutex with no initial owner

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}

	// Create worker threads

	for (i = 0; i < THREADCOUNT; i++)
	{
		aThread[i] = CreateThread(
			NULL,       // default security attributes
			0,          // default stack size
			(LPTHREAD_START_ROUTINE)WriteToDatabase,
			NULL,       // no thread function arguments
			0,          // default creation flags
			&ThreadID); // receive thread identifier

		if (aThread[i] == NULL)
		{
			printf("CreateThread error: %d\n", GetLastError());
			return 1;
		}
	}

	// Wait for all threads to terminate

	WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

	// Close thread and mutex handles

	for (i = 0; i < THREADCOUNT; i++)
		CloseHandle(aThread[i]);

	CloseHandle(ghMutex);

	return 0;
}

DWORD WINAPI WriteToDatabase(LPVOID lpParam)
{
	// lpParam not used in this example
	UNREFERENCED_PARAMETER(lpParam);

	DWORD dwCount = 0, dwWaitResult;

	// Request ownership of mutex.

	while (dwCount < 20)
	{
		dwWaitResult = WaitForSingleObject(
			ghMutex,    // handle to mutex
			INFINITE);  // no time-out interval

		switch (dwWaitResult)
		{
			// The thread got ownership of the mutex
		case WAIT_OBJECT_0:
			__try {
				// TODO: Write to the database
				printf("Thread %d writing to database...\n",
					GetCurrentThreadId());
				dwCount++;
			}

			__finally {
				// Release ownership of the mutex object
				if (!ReleaseMutex(ghMutex))
				{
					// Handle error.
				}
			}
			break;

			// The thread got ownership of an abandoned mutex
			// The database is in an indeterminate state
		case WAIT_ABANDONED:
			return FALSE;
		}
	}
	getwchar();
	return TRUE;
}


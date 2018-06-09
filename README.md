# MutexObjects

*mutex.cpp*: определяет точку входа для консольного приложения.

https://msdn.microsoft.com/en-us/library/windows/desktop/ms686927(v=vs.85).aspx


В следующем примере функция **CreateMutex** используется для создания объекта **mutex** и функции **CreateThread** для создания рабочих потоков.

Когда поток этого процесса записывается в базу данных, он сначала запрашивает владение мьютексом, используя функцию **WaitForSingleObject**. Если поток получает право владения мьютексом, он записывает в базу данных, а затем освобождает свою собственность на мьютекс, используя функцию **ReleaseMutex**.

В этом примере используется структурированная обработка исключений, чтобы убедиться, что поток правильно освобождает объект **mutex** . Блок кода **__finally** выполняется независимо от того, как заканчивается **__try-блок** (если только блок **__try** не включает вызов функции **TerminateThread**).

Это предотвращает непреднамеренный отказ объекта мьютекса.

Если мьютекс заброшен, поток, которому принадлежит мьютекс, не отпустил его до завершения. В этом случае состояние общего ресурса является неопределенным, и продолжение использования мьютекса может скрыть потенциально серьезную ошибку. Некоторые приложения могут попытаться восстановить ресурс в согласованное состояние; этот пример просто возвращает ошибку и перестает использовать мьютекс. 


***


The following example uses the **CreateMutex** function to create a **mutex object** and the **CreateThread** function to create worker threads.

When a thread of this process writes to the database, it first requests ownership of the mutex using the **WaitForSingleObject** function. If the thread obtains ownership of the mutex, it writes to the database and then releases its ownership of the mutex using the **ReleaseMutex** function.

This example uses structured exception handling to ensure that the thread properly releases the mutex object. The **__finally** block of code is executed no matter how the **__try** block terminates (unless the **__try** block includes a call to the **TerminateThread** function). This prevents the mutex object from being abandoned inadvertently.

If a mutex is abandoned, the thread that owned the mutex did not properly release it before terminating. In this case, the status of the shared resource is indeterminate, and continuing to use the mutex can obscure a potentially serious error. Some applications might attempt to restore the resource to a consistent state; this example simply returns an error and stops using the mutex.

compile - gcc -pthread q1.c

Code Explanation :

Machine is used as a semaphore, hence only m persons can use critical section ie. machine at a given point of time.

Students are used as n threads, hence thread access to critical region is samae as students accesiing machines.

A thread waits till the corresponding student arrives for washing.

Then sem_timedwait() is used and thread waits till patience time.

If thread enters critical section, it is in critical region for washing time after which it leaves the machine using se_post funtion.

It follows FCFS policy.
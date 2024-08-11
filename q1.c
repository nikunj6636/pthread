#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<time.h>
#include<errno.h>
#include<stdlib.h>

sem_t machines;
int n, m; // people, washing machines

typedef struct student_t{
    int t, w, p, i;
}student_t;

// global varible for communication of threads

int left_without_washing = 0, time_wasted = 0; 
time_t start;

void *thread(void *arg)
{
    student_t *s = (student_t*)arg;

    sleep(s->t); // thread starts execution when student comes
    usleep(s->i*100); // for FCFS-index policy

    time_t arrives = time(NULL), leaves;
    printf("\033[0;37m%ld:Student %d arrives\n", time(NULL)-start,s->i);

    // wait for the washing machines to be empty

    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(0);
    }
    ts.tv_sec += s->p;
    ts.tv_nsec +=(100000000);
    if (sem_timedwait(&machines, &ts) == 0)
    {
        // critical section: uses washing machine
        printf("\033[0;32m%ld:Student %d starts washing\n", time(NULL)-start, s->i);
        leaves = time(NULL);
        sleep(s->w);

        sem_post(&machines); // making machine available
        printf("\033[0;33m%ld:Student %d leaves after washing\n",time(NULL)-start, s->i);
    }

    else if(errno = ETIMEDOUT){ // leaves without using machine
        left_without_washing++;
        leaves = time(NULL);
        printf("\033[0;31m%ld:Student %d leaves without washing\n",time(NULL)-start, s->i);
    }

    else 
    {
        perror("sem_timedwait");
        exit(0);
    }

    time_wasted += leaves - arrives; // wated time
    return NULL;
}

int main()
{
    scanf("%d %d", &n, &m);
    sem_init(&machines, 0, m); 
    // 0 for threads of the same parent process, atmost m threads can use semaphore
    
    start = time(NULL);

    student_t students[n];
    pthread_t t[n]; // people as threads

    int i;
    for(i=0; i<n; i++)
    {
        scanf("%d %d %d", &students[i].t, &students[i].w, &students[i].p);
        students[i].i = i+1;
        pthread_create(&t[i], NULL, thread, &students[i]);
    }

    for(i=0; i<n; i++) pthread_join(t[i], NULL); // destroying threads, semaphores
    sem_destroy(&machines);

    printf("\033[0;37m%d\n%d\n", left_without_washing, time_wasted);

    if ((float)left_without_washing*100/n >= 25) printf("Yes\n");
    else printf("No\n"); // need of new washing machines
}

/*
5 2
6 3 5
3 4 3
6 5 2
2 9 6
8 5 2
*/

/*
3 1
2 5 1
1 2 4
2 4 2
*/
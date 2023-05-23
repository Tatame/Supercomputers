#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>

using namespace std;
#define err_exit(code, str) { cerr << str << ": " << strerror(code) << endl; exit(EXIT_FAILURE); }

const int TASKS_COUNT = 10;
int task_list[TASKS_COUNT];
int current_task = 0;
//pthread_mutex_t mutex;
pthread_spinlock_t lock;

void do_task(int task_no)
{
    int sum = task_no;
    for (int i = 0; i < 10e7; ++i) {
        sum += 1;
    }
    //cout << sum << endl;
}

void *thread_job(void *arg) {
    clock_t start = clock();
    int task_no;
    int err;

    while (true) {
        /*err = pthread_mutex_lock(&mutex);
        if (err != 0) err_exit(err, "Cannot lock mutex");*/
        err = pthread_spin_lock(&lock);
        if(err != 0)
        err_exit(err, "Cannot lock spinlock");

        task_no = current_task;
        current_task++;

        /*err = pthread_mutex_unlock(&mutex);
        if (err != 0) err_exit(err, "Cannot unlock mutex");*/
        pthread_spin_unlock(&lock);

        if (task_no < TASKS_COUNT) {
            do_task(task_no);
            //cout << task_no << " from Thread " << *(int *) arg << endl;
        } else {
            clock_t finish = clock();
            cout << "Thread " << *(int *) arg << " time " << (double) (finish - start) / CLOCKS_PER_SEC << endl;
            return NULL;
        }
    }
}

int main()
{
    pthread_t thread1, thread2;
    int err;

    for(int i=0; i<TASKS_COUNT; ++i)
        task_list[i] = rand() % TASKS_COUNT;

    /*err = pthread_mutex_init(&mutex, NULL);
    if(err != 0)
    err_exit(err, "Cannot initialize mutex");*/
    err = pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    if(err != 0)
    err_exit(err, "Cannot initialize spinlock");

    int first = 1, second = 2;
    err = pthread_create(&thread1, NULL, thread_job, &first);
    if(err != 0)
    err_exit(err, "Cannot create thread 1");
    err = pthread_create(&thread2, NULL, thread_job, &second);
    if(err != 0)
    err_exit(err, "Cannot create thread 2");

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    /*pthread_mutex_destroy(&mutex);*/
    pthread_spin_destroy(&lock);
}


#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
using namespace std;
/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    int *param = (int *) arg;
    cout << "Thread " << *param << " is running..." << endl;
    double sum = 0;
    for (int i = 0; i < 10e8; ++i) {
        sum += 0.001;
    }
    cout << sum << endl;
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int n = (argc > 1 ? std::stoi(argv[1]) : 1);

    pthread_t threads[n];
    pthread_attr_t thread_attrs[n];
    int err[n];

    for (int i = 0; i < n; ++i) {
        err[i] = pthread_attr_init(&thread_attrs[i]);
        if(err[i] != 0) {
            cout << "Cannot create thread attribute: " << strerror(err[i]) << endl;
            exit(-1);
        }

        err[i] = pthread_attr_setstacksize(&thread_attrs[i], (i+1)*1024*1024);
        if(err[i] != 0) {
            cout << "Setting stack size attribute failed: " << strerror(err[i]) << endl;
            exit(-1);
        }
    }

    clock_t start = clock();

    int param[n];
    for (int i = 0; i < n; ++i) {
        param[i] = i+1;
    }
    for (int i = 0; i < n; ++i) {
        err[i] = pthread_create(&threads[i], &thread_attrs[i], thread_job, (void*)&param[i]);

        if(err[i] != 0) {
            cout << "Cannot create a thread: " << strerror(err[i]) << endl;
            exit(-1);
        }
    }

    clock_t finish = clock();

    for (int i = 0; i < n; ++i) {
        pthread_join(threads[i], NULL);
    }

    cout << (double)(finish - start) / CLOCKS_PER_SEC << endl;

}
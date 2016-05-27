#include <stdio.h>
#include <list>
#include <unistd.h>
#include <sys/errno.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#define TRACE(fmt, ...) \
    printf("%s(%d)-<%s>: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

void* Producer(void* arg);
void* Consumer(void* arg);

pthread_t pid_producer, pid_consumer1, pid_consumer2;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

std::list<int> list;

int main(int argc, char* argv[])
{
    if (pthread_create(&pid_producer, NULL, Producer, NULL) != 0) {
        printf("can not create thread\n");
        return 0;
    }
    int flag1 = 1;
    if (pthread_create(&pid_consumer1, NULL, Consumer, (void*)&flag1) != 0) {
        printf("can not create thread\n");
        return 0;
    }
    int flag2 = 2;
    if (pthread_create(&pid_consumer2, NULL, Consumer, (void*)&flag2) != 0) {
        printf("can not create thread\n");
        return 0;
    }

    pthread_join(pid_producer, NULL);
    printf("producer thread  exit\n");
    pthread_join(pid_consumer1, NULL);
    printf("consumer thread  exit\n");

    return 0;
}

void* Consumer(void* arg)
{
    int* index = (int*)arg;
    for (size_t i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        TRACE("LOCK%d", *index);

        if (list.empty()) {
            printf("consume%d : waiting\n", *index);
            pthread_cond_wait(&cond, &mutex);
        }

        if (!list.empty()) {
            printf("consume%d : %d\n", *index, list.back());
            list.pop_back();
        }

        pthread_mutex_unlock(&mutex);
        TRACE("UNLOCK%d", *index);
        
        sleep(1);
    }
    return NULL;
}

void* Producer(void* arg)
{
    for (size_t i = 0; i < 20; i++) {
        pthread_mutex_lock(&mutex);
        TRACE("LOCK");
        
        printf("produce : %d\n", (int)i);
        list.push_front(i);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        TRACE("UNLOCK");
        
        sleep(1);
    }
    return NULL;
}

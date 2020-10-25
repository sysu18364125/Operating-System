#include<math.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/mman.h>
#include <stdio.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<semaphore.h>
#include<string.h>
#include<pthread.h>


double produce_time(double lambdba_c);
typedef int buffer_item;
#define BUFFER_SIZE 20
struct buf{
    int rear;
    int front;
    buffer_item buffer[BUFFER_SIZE];
};


sem_t *full;
sem_t *empty;
sem_t *s_mutex;
void *ptr;
int lambda_c;

void *consumer();

int main (int argc,char *argv[]){
        pthread_t tid1;
        pthread_t tid2;
        pthread_t tid3;
        pthread_attr_t attr1;
        pthread_attr_t attr2;
        pthread_attr_t attr3;
        full = sem_open("full",O_CREAT);//与生产者函数相接
        empty = sem_open("empty",O_CREAT);
        s_mutex = sem_open("mutex",O_CREAT);
        int shm_fd = shm_open("buffer",O_RDWR,0666);
        ptr = mmap(0,sizeof(struct buf),PROT_READ | PROT_WRITE,MAP_SHARED,shm_fd,0);
        lambda_c=atof(argv[1]);
        pthread_attr_init(&attr1);
        pthread_attr_init(&attr2);
        pthread_attr_init(&attr3);
        pthread_create(&tid1,&attr1,consumer,NULL);
        pthread_create(&tid2,&attr2,consumer,NULL);
        pthread_create(&tid3,&attr3,consumer,NULL);
        pthread_join(tid1,NULL);
        pthread_join(tid2,NULL);
        pthread_join(tid3,NULL);


}

void *consumer(){

    do{
        double interval_time = lambda_c;
        sleep((unsigned int)produce_time(interval_time));
        struct buf *shm_ptr = ((struct buf *)ptr);
        sem_wait(full);//Wait for a full buffer
        sem_wait(s_mutex);

        buffer_item item = shm_ptr->buffer[shm_ptr->front];
        printf("PID: %d Consuming the data %d from buffer[%d] by pid %ld\n",getpid(),item,shm_ptr->front,pthread_self());
        shm_ptr->front = (shm_ptr->front+1) % BUFFER_SIZE;
        sem_post(s_mutex);
        sem_post(empty);//empty+1

    }while (1);
    pthread_exit(0);
}

double produce_time(double lambda_c){
    double z;
    do
    {
        z = ((double)rand() / RAND_MAX);
    }
    while((z==0) || (z == 1));
    return (-1/lambda_c * log(z));
}



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
typedef int buffer_item;
#define BUFFER_SIZE 20
struct buf{
    int rear;
    int front;
    buffer_item buffer[BUFFER_SIZE];
};


//producer
void *producer(); 
void *ptr;
double produce_time(double lambda_p);

sem_t *full;
sem_t *empty; 
sem_t *s_mutex; 

int lambda_p;
int main(int argc,char *argv[]){
        pthread_t tid1;
        pthread_t tid2; 
        pthread_t tid3;
        pthread_attr_t attr1;  
        pthread_attr_t attr2;
        pthread_attr_t attr3;
        full = sem_open("full",O_CREAT,0666,0);//sem_open returns a sem_t pointer
        empty = sem_open("empty",O_CREAT,0666,0); 
        s_mutex = sem_open("mutex",O_CREAT,0666,0);
        sem_init(full,1,0);//第二个参数1表示共享信号量,第三个为初始值
        sem_init(empty,1,BUFFER_SIZE); 
        sem_init(s_mutex,1,1);

        lambda_p = atof(argv[1]);

        int shm_fd = shm_open("buffer",O_CREAT | O_RDWR,0666); //创建共享内存
        ftruncate(shm_fd,4096);

        ptr = mmap(0,sizeof(struct buf),PROT_WRITE,MAP_SHARED,shm_fd,0);

		struct buf *temp=((struct buf*)ptr);//将结构体指针初始化
        temp->front=0;
        temp->rear=0;
		
        pthread_attr_init(&attr1);
        pthread_attr_init(&attr2);
        pthread_attr_init(&attr3);
        pthread_create(&tid1,&attr1,producer,NULL);
        pthread_create(&tid2,&attr2,producer,NULL);
        pthread_create(&tid3,&attr3,producer,NULL);
        pthread_join(tid1,NULL);
        pthread_join(tid2,NULL);
        pthread_join(tid3,NULL);

        return 0;
}
double produce_time(double lambda_p){
    double z;
    do
    {
        z = ((double)rand() / RAND_MAX);
    }
    while((z==0) || (z == 1));
    return (-1/lambda_p * log(z));
}
void *producer(){
    do{
        double interval_time = lambda_p;
        //printf("sss");
        usleep((unsigned int)(produce_time(interval_time)*1e6)); // sleep 
        buffer_item item = rand() % 260;
        struct buf *shm_ptr = ((struct buf *)ptr);// read the round queue's information from shared memory.
        sem_wait(empty); //缓冲区满则上锁
        sem_wait(s_mutex);//锁定二进制互斥锁s_mutex，并在关键区域执行代码
        printf("PID: %d Producing the data %d to buffer[%d] by id %ld\n",getpid(),item,shm_ptr->rear,pthread_self());
        shm_ptr->buffer[shm_ptr->rear] = item; // Put the data to round queue.
        shm_ptr->rear = (shm_ptr->rear+1) % BUFFER_SIZE;
        sem_post(s_mutex);//Unlock s_mutex
        sem_post(full);// full+1
    }while(1);
    pthread_exit(0);
}



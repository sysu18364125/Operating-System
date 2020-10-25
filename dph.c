#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<semaphore.h>
#include<string.h>
#include<pthread.h>



enum {THINKING,HUNGRY,EATING}state[5];
pthread_cond_t self[5];// Behalf on each philosophers
pthread_mutex_t mutex[5]; // For conditional variables
pthread_t tid[5];// For pthread_create()
int id[5];// For philo function
void *philo(void *param);
pthread_attr_t attr[5];
void pickup_forks(int i);
void return_forks(int i);
void test(int i);
int main(){

  for(int i = 0;i<5;i++){//依次按序号创建哲学家线程
  id[i]= i;
   // printf("id: %d ",id[i]);
    pthread_mutex_init(&mutex[i]  ,NULL);
    pthread_cond_init(&self[i],NULL);
    pthread_attr_init(&attr[i]);
    pthread_create(&tid[i],&attr[i],philo,&id[i]);
}

  for(int i = 0;i<5;i++)
 { pthread_join(tid[i],NULL); }
}

#define TRUE 1
void *philo(void *param){
    do{
    int id = *( (int *)param);
    /* Try to pickup a chopstick*/
    pickup_forks(id);
    printf("The philosopher %d is eating...\n",id);
    /* Eat a while*/
    srand((unsigned)time(NULL));
    int sec = (rand()%((3-1)+1)) +1;// 将sec控制在[1,3]
    sleep(sec);
    /* Return a chopstick */
    return_forks(id);
    printf("The philosopher %d is thinking...\n",id);
    
    srand((unsigned)time(NULL));//思考一定的时间
    sec = (rand()%((3-1)+1)) +1;
    sleep(sec);
    }while(TRUE);
    pthread_exit(NULL);
}

void pickup_forks(int i){
    state[i] = HUNGRY;
    test(i);// 检查是否正在吃饭
    pthread_mutex_lock(&mutex[i]);
    while (state[i] != EATING){
        pthread_cond_wait(&self[i],&mutex[i]);//等待相邻吃完
    }
    pthread_mutex_unlock(&mutex[i]);
}

void return_forks(int i){
    state[i] = THINKING;
    //告诉相邻哲学家正在吃
    test((i+4)%5);
    test((i+1)%5);
}

void test(int i){
    //个哲学家可以在他想吃的时候吃，同时他的邻居们没有在吃。
    if ( (state[(i+4)%5] != EATING)&&
    (state[i] == HUNGRY) &&
    (state[(i+1)%5] != EATING)
    ){
        pthread_mutex_lock(&mutex[i]);
        state[i] = EATING;
        pthread_cond_signal(&self[i]);
        pthread_mutex_unlock(&mutex[i]);
    }

}



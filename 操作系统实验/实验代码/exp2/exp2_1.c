#include<stdio.h>
#include<stdlib.h>
#include<linux/sem.h>
#include<pthread.h>
#include<sys/types.h>
#define pthread_Num 5

int totalTkt = 100; // 记录总票数为100
int sellTkt = 0;  //记录已售票数
int mutex;  //互斥信号灯集


//P操作定义
void P(int semid,int index){
    struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op = -1;	
    sem.sem_flg = 0; //操作标记：0或IPC_NOWAIT等
    semop(semid,&sem,1);	//1：表示执行命令的个数	
    return;
}
//V操作定义
void V(int semid,int index){	
    struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op =  1;
    sem.sem_flg = 0;	
    semop(semid,&sem,1);	
    return;
}

void *SellTicket(void* threadNum){
    int threadid = (int)threadNum;
    int mysell = 0;
    while(1){
        P(mutex,0);
        if(sellTkt<totalTkt){
            mysell+=1;
            sellTkt+=1;
            printf("now pthread %d sells %d tickets.All sell %d tickets.\n",threadid,mysell,sellTkt);
        }
        else{
            printf("Finally,pthread %d has sold %d tickets!\n",threadid,mysell);
            V(mutex,0);
            break;
        }
        V(mutex,0);
    }
    return;
}

void main(){
    int i;
    mutex = semget(IPC_PRIVATE,1,IPC_CREAT|0666);  //创建mutex
    if(mutex == -1){
        printf("create semaphore fail!\n");
        exit(1);
    }
    union semun sem_arg;
    sem_arg.val = 1;
    if(semctl(mutex,0,SETVAL,sem_arg)==-1){  //初始化mutex
        printf("initialize semaphroe fail!\n");
        exit(1);
    }

    pthread_t p[pthread_Num];
    for(i=0;i<pthread_Num;i++){   //创建pthread_Num个线程
        if(pthread_create(&p[i],NULL,SellTicket,(void *)(i+1))==-1){
            printf("create pthread %d fail!\n",i+1);
            exit(1);
        }
    }

    for(i=0;i<pthread_Num;i++){  //等待所有线程结束
        if(pthread_join(p[i],NULL)==-1){
            printf("pthread %d finish fail!\n",i+1);
            exit(1);
        }
    }
    semctl(mutex, 0, IPC_RMID, NULL);
    printf("ALL DONE!!!\n");
    exit(0);
}

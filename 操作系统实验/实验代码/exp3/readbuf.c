#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<unistd.h>
#define SHMKEY 123   //readbuf与writebuf进程之间的共享内存key值
#define SIZEKEY 321  //写进程向读进程传递的放置长度公共内存的key值
#define SEMKEY 111  //公共信号灯集key值
#define SHMNUM 10   //定义共享内存组数
#define SHMSIZE 4096  //共享内存一组为4K
int shmid;
int sizeid;
int semid;  //0号指示empty，1号指示full
char *bufaddr;
int *sizeaddr;
void P(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}
void V(int semid, int index)
{
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}
int main(){
    if((shmid = shmget(SHMKEY, 40960, IPC_CREAT|0666)) == -1){
        printf("readbuf：get shared memory fail!\n");
        exit(1);
    }
    bufaddr = shmat(shmid,NULL,0);
    if ((sizeid = shmget(SIZEKEY, 4, IPC_CREAT|0666)) == -1){
        printf("writebuf：get size shared memory fail！\n");
        exit(1);
    }
    sizeaddr = shmat(sizeid,NULL,0);
    if ((semid = semget(SEMKEY, 2, IPC_CREAT|0666))== -1)
    {
        printf("readbuf：get semaphore fail!\n!");
        exit(1);
    }

    FILE *fp = fopen("text-copy.txt","w");
    int j = 0;
    char *now_out;
    while(1){
        P(semid,1);
        now_out = bufaddr + (j % SHMNUM) * SHMSIZE;
        if(*(now_out + SHMSIZE - 3) == 'E' && *(now_out + SHMSIZE - 2) == 'O' && *(now_out + SHMSIZE - 1) == 'F'){
            printf("now read buf num：%d\n",j + 1);
            if(*(sizeaddr) == 0){
                fwrite(now_out,1,SHMSIZE,fp);
            }
            else{
                fwrite(now_out,1,*(sizeaddr),fp);
            }
            V(semid,0);
            break;
        }
        printf("now read buf num：%d\n",j + 1);
        fwrite(now_out,1,SHMSIZE,fp);
        j++;
        V(semid,0);
    }
    printf("Readbuf done!\n");
    exit(0);
}
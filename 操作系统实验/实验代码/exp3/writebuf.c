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
        printf("writebuf：get shared memory fail!\n");
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
        printf("writebuf：get semaphore fail!\n!");
        exit(1);
    }

    FILE *fp = fopen("text.txt","rb");
    if(fp == NULL) printf("open ‘text.txt’ fail\n");
    fseek(fp,0L,SEEK_END);
    int FileLen = ftell(fp); //计算文件长度
    rewind(fp);
    printf("fileLength：%d\n",FileLen);
    int ndnum;
    if(FileLen % SHMSIZE == 0){   //计算所需总缓冲区数
        ndnum = FileLen/SHMSIZE;
    }
    else ndnum = FileLen/SHMSIZE + 1;
    printf("need shared num：%d\n",ndnum);

    int i = 0;
    char *now_in;
    while(1){
        P(semid,0);
        now_in = bufaddr + (i % SHMNUM) * SHMSIZE;
        fread(now_in,1,SHMSIZE,fp);
        printf("now write buf num：%d\n",i + 1);
        if(i + 1 == ndnum){
            *(now_in + SHMSIZE - 3) = 'E';
            *(now_in + SHMSIZE - 2) = 'O';
            *(now_in + SHMSIZE - 1) = 'F';
            *(sizeaddr) = FileLen % SHMSIZE;
            V(semid,1);
            break;
        }
        i++;
        V(semid,1);
    }
    printf("Writebuf done!\n");
    exit(0);
}
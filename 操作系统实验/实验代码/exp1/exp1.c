#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXBuf 30
#define MAXTimes 10

int pipefd[2]; //pipefd[0] for read; pipefd[1] for write
pid_t pid1, pid2;

void my_func_parent(int sig_no)
{
    kill(pid1, SIGUSR1);
    kill(pid2, SIGUSR2);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
    printf("Parent Process is Killed!\n");
    exit(1);
}

void my_func_child1(int sig_no)
{
    close(pipefd[0]);
    printf("Child Process 1 is Killed by Parent!\n");
    exit(1);
}

void my_func_child2(int sig_no)
{
    close(pipefd[0]);
    printf("Child Process 2 is Killed by Parent!\n");
    exit(1);
}

void main()
{
    pipe(pipefd);
    char buf[MAXBuf];
    int counter = 0;
    int times = 1;
    signal(SIGINT, my_func_parent);

    if ((pid1 = fork()) == 0)
    { //Child1`s Process
        signal(SIGINT, SIG_IGN);
        signal(SIGUSR1, my_func_child1);
        close(pipefd[1]);
        while (1)
        {
            if (read(pipefd[0], buf, sizeof(buf)) <= 0)
                break;
            printf("Child1: %s", buf);
            counter++;
        }
        close(pipefd[0]);
        printf("Child1 Process has received %d infos!\n", counter);
        exit(0);
    }
    else
    {
        if ((pid2 = fork()) == 0)
        { //Child2`s Process
            signal(SIGINT, SIG_IGN);
            signal(SIGUSR2, my_func_child2);
            close(pipefd[1]);
            while (1)
            {
                if (read(pipefd[0], buf, sizeof(buf)) <= 0)
                    break;
                printf("Child2: %s", buf);
                counter++;
            }
            close(pipefd[0]);
            printf("Child2 Process has received %d infos!\n", counter);
            exit(0);
        }
        else
        { // Parent`s Process
            close(pipefd[0]);
            while (times <= MAXTimes)
            {
                sprintf(buf, "I send you %d times.\n", times);
                write(pipefd[1], buf, sizeof(buf));
                times++;
                sleep(1);
            }
            close(pipefd[1]);
            wait(NULL);
            wait(NULL);
            printf("Parent Process is Killed!\n");
            exit(0);
        }
    }
}
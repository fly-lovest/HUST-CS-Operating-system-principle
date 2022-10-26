#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>


void printdir(char *dir, int depth){
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL){
        printf("open dir fail!\n");
        return;
    }
    if(chdir(dir) == -1){
        printf("change dir fail!\n");
        return;
    }
    while((entry = readdir(dp)) != NULL){
        lstat(entry->d_name, &statbuf);
        if(statbuf.st_mode & S_IFDIR == S_IFDIR){  //若是目录，进行如下操作
            if(!strcmp(entry->d_name, "..")||!strcmp(entry->d_name, ".")){
                continue;  //当读到目录项为“..”目录为上一目录，“.”目录为当前目录时，跳过该目录项输出
            }
            for(int i = 0; i < depth; i++){
                printf(" ");
            }
            // 输出属性
            printf("d");
            // 输出各权限：owner权限;group权限;other权限
            if(statbuf.st_mode & S_IRUSR)  printf("r");
            else  printf("-");
            if(statbuf.st_mode & S_IWUSR)  printf("w");
            else  printf("-");
            if(statbuf.st_mode & S_IXUSR)  printf("x");
            else  printf("-");
            
            if(statbuf.st_mode & S_IRGRP)  printf("r");
            else  printf("-");
            if(statbuf.st_mode & S_IWGRP)  printf("w");
            else  printf("-");
            if(statbuf.st_mode & S_IXGRP)  printf("x");
            else  printf("-");
            
            if(statbuf.st_mode & S_IROTH)  printf("r");
            else  printf("-");
            if(statbuf.st_mode & S_IWOTH)  printf("w");
            else  printf("-");
            if(statbuf.st_mode & S_IXOTH)  printf("x");
            else  printf("-");
            printf(" %d", statbuf.st_nlink); // 输出链接数
            struct passwd* psd = getpwuid(statbuf.st_uid);  // 获取所有者用户名
            struct group* grp = getgrgid(statbuf.st_gid);   // 获取所属组名
            printf(" %s", psd->pw_name);
            printf(" %s", grp->gr_name);
            printf(" %d", statbuf.st_size); // 输出文件大小，byte为单位
            struct tm* t = localtime(&statbuf.st_mtime);  // 获取最后修改时间
            printf(" %0d-%0d-%0d %0d:%0d", t->tm_year, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);
            printf(" %s\n", entry->d_name); // 输出目录名

            printdir(entry->d_name, depth + 4);
        }
        else{
            for(int i = 0; i < depth; i++){
                printf(" ");
            }
            // 输出属性
            if(S_ISREG(statbuf.st_mode))  printf("-");  // 普通文件属性
            else if(S_ISLNK(statbuf.st_mode))  printf("l");
            else if(S_ISCHR(statbuf.st_mode))  printf("c");
            else if(S_ISBLK(statbuf.st_mode))  printf("b");
            else if(S_ISFIFO(statbuf.st_mode))  printf("f");
            else if(S_ISSOCK(statbuf.st_mode))  printf("s");
            // 输出各权限：owner权限;group权限;other权限
            if(statbuf.st_mode & S_IRUSR)  printf("r");
            else  printf("-");
            if(statbuf.st_mode & S_IWUSR)  printf("w");
            else  printf("-");
            if(statbuf.st_mode & S_IXUSR)  printf("x");
            else  printf("-");
            
            if(statbuf.st_mode & S_IRGRP)  printf("r");
            else  printf("-");
            if(statbuf.st_mode & S_IWGRP)  printf("w");
            else  printf("-");
            if(statbuf.st_mode & S_IXGRP)  printf("x");
            else  printf("-");
            
            if(statbuf.st_mode & S_IROTH)  printf("r");
            else  printf("-");
            if(statbuf.st_mode & S_IWOTH)  printf("w");
            else  printf("-");
            if(statbuf.st_mode & S_IXOTH)  printf("x");
            else  printf("-");
            printf(" %d", statbuf.st_nlink); // 输出链接数
            struct passwd* psd = getpwuid(statbuf.st_uid);  // 获取所有者用户名
            struct group* grp = getgrgid(statbuf.st_gid);   // 获取所属组名
            printf(" %s", psd->pw_name);
            printf(" %s", grp->gr_name);
            printf(" %d", statbuf.st_size); // 输出文件大小，byte为单位
            struct tm* t = localtime(&statbuf.st_mtime);  // 获取最后修改时间
            printf(" %0d-%0d-%0d %0d:%0d", t->tm_year, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min);
            printf(" %s\n", entry->d_name); // 输出目录名
        }
    }
    chdir("..");
    closedir(dp);
    return;
}

int main(){
    char name[50];
    printf("input directory：");
    scanf("%s", name);
    printdir(name, 0);
    printf("ALL DONE!\n");
    exit(0);
}
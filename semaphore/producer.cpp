#include <semaphore.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 信号量操作
void sem_wait(int semid) {
 
    struct sembuf sb = {0, -1, 0};
    if(-1==semop(semid, &sb, 1)){
      perror("sem_wait error");
    }
}

void sem_signal(int semid) {
  // V操作, 下标0，增加signal_num个value值，flag 0表示阻塞
  struct sembuf sb = {0, 1, 0};
  if (-1 == semop(semid, &sb, 1)) {
    perror("sem_signal error");
  }
}



int main() {
  // 进程间信号量 线程间信号量暂不介绍

  int semid;
  key_t key;
  int key_num = 12; // 通信方保持一致
  key = ftok(".", key_num);
  printf("key:%#x\n", key);

  
  // 创建信号量.
  // nsems：创建的信号量的个数（PV操作的钥匙个数），是当前信号集中的信号量的个数，不是value值，semid是用于标识这一组信号量（即标识信号量集）
  // IPC_CREAT | 0666：当信号量不存在时创建一个新的信号量，并指定权限
  // IPC_CREAT | IPC_EXCL
  // |0666：创建一个新的，唯一的信号量，如果信号量已存在，返回一个错误。

  // 创建一个信号量集，__nsems参数为信号集里面包含的信号量个数，
  // 如：__nsems为3时，表示信号集里有3个信号量，下标为0 1 2
  // 初始的value值都为0（没有通过semctl()修改），所以一旦先执行P操作，就会阻塞。
  semid = semget(key, 1, IPC_CREAT | 0666);

  semctl(semid, 0, SETVAL, 0); // 初始化信号量为0

  usleep(5000000);
  printf("start to signal\n");
  sem_signal(semid);
  printf("from signal\n");
  // 删除整个信号量集
  semctl(semid, 0, IPC_RMID, NULL);
  return 0;
}
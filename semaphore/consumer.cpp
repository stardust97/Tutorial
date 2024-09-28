#include <semaphore.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>

// 信号量操作
void sem_wait(int semid) {
  // V操作, 下标0，增加1个value值，flag 0表示阻塞
  struct sembuf sb = {0, -1, 0};
  if (-1 == semop(semid, &sb, 1)) {
    perror("sem_wait error");
  }
}

void sem_signal(int semid) {
  struct sembuf sb = {0, 1, 0};
  if (-1 == semop(semid, &sb, 1)) {
    perror("sem_signal error");
  }
}


int main(){

  int semid;
  key_t key;
  int key_num = 12; // 通信方保持一致
  key = ftok(".", key_num);
  printf("key:%#x\n", key);

  semid = semget(key, 1, 0666); // 信号量未被创建时会在wait/singal时报错

//todo 
  // semid = semget(key, 1, IPC_CREAT | 0666);

  printf("start to wait sem \n");
  sem_wait(semid); // 等待信号量
  printf("wait from sem \n");
  return 0;
}
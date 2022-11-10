#include "types.h"
#include "stat.h"
#include "user.h"

#define PROC_ITERS 100
#define PROC_COUNT 10

void launchHungryProc(){
  int pid=fork();
  if (pid==0){
    for (int i=0;i<PROC_ITERS*2;i++){
      sleep(1);
    }
    exit();
  }
}

void launchLongProc(){
  int pid=fork();
  if (pid==0){
    for (int i=0;i<PROC_ITERS;i++);
    for (int i=0;i<PROC_ITERS;i++){
      sleep(2);
    }
    exit();
  }
}

int
main(int argc, char *argv[])
{
  launchHungryProc();
  for (int i=0;i<PROC_COUNT;i++){
    launchLongProc();
  }
  int pid=0;
  while (pid>=0) pid=wait();
    exit();
}

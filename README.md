# CISC361-Project4

Overview:  For project 4, you will be modifying the simple round robin scheduler in Xv6 to support simple MLFQ (without starvation).  
To accomplish this you will implement a multi-level feedback queue with 4 levels.  For simplicity, we will do this within the existing ptable structure 
by storing the queue number in the proc structure and selecting the job with the next job in the list with the highest queue number.   
Your scheduler must avoid starvation by periodically raising the queue number of processes that have not run for a while to a maximum of 3. 
Where queue 0 is the lowest priority and 3 is the highest.  Review Chapter 5 of the xv6 book (https://pdos.csail.mit.edu/6.828/2019/xv6/book-riscv-rev0.pdf).

Details:  To implement this, you will need to modify the proc structure to contain the queue number (This should be stored as an integer value between 0 
and 3, and should be initialized to 3 on process creation) and the number of iterations that the process will run at this queue level and the number of 
iterations the process has been idle at this queue level.   We can view each iteration of the inside for loop in the scheduler() method as a timer tick (interrupt) 
as the scheduler thread will only run after a timer interrupt.  The timer interrupt in xv6 will fire approximately every 10ms, so one iteration of the loop, 
can be viewed as a process having 10ms on the cpu.

When a process is created, it gets marked with the number of iterations it can stay on the cpu where queue 3 runs for 80ms (8 iterations) queue 2 runs for 160ms 
(16 iterations) queue 1 runs for 240ms (24 iterations) and queue 0 runs for 5s (500 iterations).

To select a process, we check not only runnable, but if it is at the highest queue and if it has any runs left at this run level.  You can do this inefficiently 
for this project.  You basically want to select the next process in queue 3 if it exists, so an easy not-efficient implementation is to first look through the 
entire list to get the maximum queue number in the list, then use that along with runnable to find the process to run.   

Once selected to run, reset it's count of idle iterations to 0, reduce it's number of runs at this queue level by 1, and switch it in as before.

When a processes number of iterations reaches 0, lower it's queue number by 1 (minimum 0) and update it's number of iterations using the numbers above before 
checking to see if it should run using the policy above.  Also set it's idle count to 0.

Lastly, to avoid starvation, implement a priority boost where any process who's idle count exceeds the number of allowed iterations at that run level gets 
moved up (to a max of 3) and updates its iteration count to the new queue level and it's idle count to 0.

Steps:

Modify proc structure in proc.h by adding 3 fields for the queue number, the remaining iterations at the current queue level, and the count of iterations since 
the process was last run.

Initialize these three fields (always 3 for queue number, 8 for iteration count, and 0 for idle for all new processes).  This can be done in the allocproc() 
method in proc.c

In scheduler.c loop through all processes (inside each iteration)
  You want to first move processes up or down based on idlecount>iterations for current level, and iterationcount=0.  
    Make sure to reset idlecount and iterations when you change the level.
  At the same time, you can select the queue number by finding the highest queue number that exists after you have made the changes in 1.
  After the test, increment the idle count for all processes.

Then modify the if statement to check for runnable and the highest queue found in (3).
  Select the process with the highest queue value that is runnable,
  Reset it's idle count to 0.
  Decrement its iteration count
  Run it as before

Add a user program (like in project 3) with the c file named stressproc.c.  You must add it to the makefile.  The contents of stressproc.c are:
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
Notes:  You can start with your code from project 3 or a clean copy.  Either way, modify the line we added in part1 of project 3 to also display the queue number,
idle count, and iterations left (display this in ms.) (i.e. if idlecount is 5, then it has been idle for 50ms).  If iterations is 3 then it will stay at this queue
level for 30ms.

Extra Credit: (20 points)

This is a very inefficient implementation of MLFQ.  We have to iterate through all processes on each timer interrupt to update them and find the queue we want to
select from.  There are many ways to make this better and more efficient.  Points will be awarded on a case by case basis based on how you have added efficiency.  
A perfect solution would involve finding a way to modify ptable and the logic so that we do not have to touch every process on each timer interrupt, but only the
process we want to run.  This means you would have to find a way to keep track of idle count without actually updating each proc structure.  Points will be given
based on how close to a perfectly efficient solution you come.  If you do this you must hand in a writeup of how you implemented the solution and why it is efficient
when compared to the default solution.

What to hand in:

Your modified proc.c and proc.h files
  A script file of your system running multiple processes.
    script outfile.txt
    make qemu-nox
    Run this command
      stressproc
      ctrl-a x
    exit
Extra credit writeup if applicable

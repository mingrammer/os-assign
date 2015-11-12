
### OS Project
Title : Operating System Project 3 - Synchronization

Author : 201420973 SWCT KwonMinJae, 201420995 SWCT ParkSungMi

Project # : #1 The Sleeping Teaching Assistant

Description : Manage the students (threads) and helping task


### Compile and Run 
1. make
2. ./simulator

### Structure
1. student: student threads have own semaphore
2. ta: ta thread has own semaphore

### Functions
1. stu_programming: students is waiting in random time (they are programming in this time), and notice to ta for help. if ta is helping a student already, other students are waiting chairs in hallway, but if chairs is full, students go to their programming task back
2. ta_teaching: ta is sleeping until notice is arrived from at least one student. if any student wake the ta up, ta will help the student. if helping is finished and chairs are empty, then ta go to sleeping back
3. rand_sleep: make sleeping time (it means programming task or helping tesk) randomly

### Simple process flows
1. make student threads and ta thread (initialize)
2. each thread is running own thread function (student is programming, ta is sleeping)
3. student will wake the ta up after randomly time, then ta will help the arrived student (change student's semaphore to 1 and wait ta's semaphore). but if chairs are full, students go to their programming back and return again after randomly time
4. if ta's help is finished, ta check the remaining students. if there is(are) student(s) ta is keeping the current state (help continuously) and if not, ta go to sleeping back (change ta's semaphore to 1 and wait student's semaphore)
5. repeat 3-4 steps

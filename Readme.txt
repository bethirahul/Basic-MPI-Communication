//********************************************************************
//
//  Rahul Bethi
//  Advanced Operating Systems
//  Programming Project #1: Basic MPI Communication
//  October 13, 2016
//  Instructor: Dr. Ajay K. Katangur
//
//  Read me
//
//********************************************************************

Part 1:
------
This part was only to setup the environment.

Part 2:
------
In this program, a telephone number is given as an input through command line argument into process 0.
mpirun -np 10 aosproj1_1 3618252478
	Process 0 checks for the number of processes as it has to be 10 for all 10 numeric values. It also checks if the number has all digits in numeric and then proceeds with MPI communication. If these are not met, then it broadcasts error code 1 to all other processes, so that the other processes first read the error and exit if it is 1 and proceed if its 0.
	MPI Communication:- First process 0 is taken as default. It broadcasts the phone number to all the remaining 9 processes. It adds the first phone number digit rank to the default message 'hello-0' and transmits to the process which has the same rank as that of first digit of the phone number.
Process 0 send hello-03 to process 3
And then the process 3 adds the next digit of the phone number and sends it to the process which has a rank, same as that of the next digit of the phone number. In this way, the message is incremented and sent to the last process. Along with the message, the digit position is also sent. This helps each process to add its next number. It also helps the last process to know tht the number has ended. The last process, then sends the digit number to all remaining processes. Reading this last digit value, every process will come to know that the data transmission has ended and then they end with MPI_Finalize function.
	The program was written to handle any number of digits as some phone numbers can me more than 10 or less than that.


Part 3:
------
Rock-paper-scissors tournament with MPI
	This program plays rock-paper-scissors game with all the processes. It can handle any number of processes. Minimum of two processes are needed to play this game. Process 0 does more work than the other processes, like calculating the stages, collecting winner information and sorting for next stage, etc. Process 0 first check the number of processes and ends by giving out an error, saying, insufficient players to start.
	Process 0 calculates the number of stages(levels) depending on the number of processes. It runs a for loop to give a signal for other processes to play the game. Signal consists of opponent, position, stage(level).
Every process gets this signal and plays game with one another, and the final result is sent back to process 0.
Every process plays in a pair of two. If there are odd numbers, that process is carried to the next stage. There are two positions assigned to each pair. 0 and 1. process with position 0 takes more responsibility to compare the result and ask the other process to send again if it’s the same. It also sends the result to both process 0 and its opponent. Depending on the result, respective process which lost the game will end by calling MPI_Finalize.
	After every stage, the winners list is sorted and taken as input to the next stage. After the last stage, process 0 will terminate. Even if the process 0 losses the game in the middle, it will be alive, managing the game, but doesn’t play the game with anyone.



//********************************************************************
//  FILE NAMES
//********************************************************************
Readme.txt
Makefile
aosproj1_1.c
aosproj1_1.pdf
aosproj1_2.c
aosproj1_2.pdf

//********************************************************************
//  EXAMPLE OUTPUT
//********************************************************************
PART 2:
------

$ mpirun -np 10 aosproj1_1 3618252478
P:3 Got data hello-03 from process 0
P:1 Got data hello-0361 from process 6
P:6 Got data hello-036 from process 3
P:8 Got data hello-03618 from process 1
P:2 Got data hello-036182 from process 8
P:5 Got data hello-0361825 from process 2
P:2 Got data hello-03618252 from process 5
P:4 Got data hello-036182524 from process 2
P:7 Got data hello-0361825247 from process 4
P:8 Got data hello-03618252478 from process 7


PART 3:
------

$ mpirun -np 10 aosproj1_2
Stage:1 Process 4 wins over process 5
Stage:1 Process 8 wins over process 9
Stage:1 Process 0 wins over process 1
Stage:1 Process 7 wins over process 6
Stage:1 Process 3 wins over process 2
Stage:2 Process 4 wins over process 7
Stage:2 Process 3 wins over process 0
Stage:3 Process 3 wins over process 4
Stage:4 Process 8 wins over process 3
Process 8 wins the rock, paper, scissors tournament
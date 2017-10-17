//********************************************************************
//
// Rahul Bethi
// Advanced Operating Systems
// Programming Project #1: Basic MPI Communication
// October 13, 2016
// Instructor: Dr. Ajay K. Katangur
//
// Part 2
//
//********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h> // to use strings

int main(int argc, char *argv[])
{
	int rank, i, size, l, dl, digit=0, rec, error = 0;
	char num[100];
	char data[100] = "hello-0"; // default message
	
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size); // total number of processes, we need 10 for 10 decimal numbers
	
	if(rank == 0) // process 0
	{
		strcpy(num, argv[1]); // copy the argument into num string
		l = strlen(num); // determine the length of the number (digits)
		if(size != 10)
		{
			printf("Invalid Input: Number of processes must be 10\n", size, l); // error if number of processes is not 10
			error = 1; // error code 1, used to send to all other processes to end
		}
		for(i=0; i<l; i++)
		{
			rec = num[i] - '0';
			if(rec < 0 || rec > 9) // check if number entered is numeric, if not error code 1
			{
				printf("Invalid Input: Number entered is invalid. Only numeric values are accepted\n");
				error = 1;
				i = l;
			}
		}
		MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD); // send the error code to all other processes to send if its 1.
		//for(i=1; i<10; i++)
		//	MPI_Send(&error,  1, MPI_INT,  i, 0, MPI_COMM_WORLD);
		if(error == 1)
		{
			MPI_Finalize(); // end if error code is 1
			return;
		}
		MPI_Bcast(&l, 1, MPI_INT, 0, MPI_COMM_WORLD); // broadcast length of the number
		//for(i=1; i<10; i++)
		//	MPI_Send(&l,  1, MPI_INT,  i, 0, MPI_COMM_WORLD); // message is sent directly to each process ,  but vroadcasting is more efficient in this case
		MPI_Bcast(num, l, MPI_CHAR, 0, MPI_COMM_WORLD); // broadcast the number
		//MPI_Barrier(MPI_COMM_WORLD);
		//for(i=1; i<10; i++)
		//	MPI_Send(num, l, MPI_CHAR, i, 3, MPI_COMM_WORLD); 
		digit = 0;
		dl = strlen(data); // determining the length of the default message, to add numbers to it.
		data[dl] = num[digit]; // add the next number
		dl++;
		rec = num[digit] - '0'; // finding out the next process by rank from next digit
		MPI_Send(&digit, 1, MPI_INT,  rec, 1, MPI_COMM_WORLD); // Send the current digit used in that number to the next process
		MPI_Send(data,  dl, MPI_CHAR, rec, 2, MPI_COMM_WORLD); // send the message after adding the next number to the next process
	}
	else // all other processes
	{
		MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD); // receive the error code from broadcast
		//MPI_Recv(&error,  1,  MPI_INT,  MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		if(error == 1)
		{
			MPI_Finalize(); // end if error code is 1
			return;
		}
		MPI_Bcast(&l, 1, MPI_INT, 0, MPI_COMM_WORLD); // receive total digits
		//MPI_Recv(&l,  1,  MPI_INT,  MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		MPI_Bcast(num, l, MPI_CHAR, 0, MPI_COMM_WORLD); // receive number
		//MPI_Barrier(MPI_COMM_WORLD);
		//MPI_Recv(num, 100, MPI_CHAR, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
	}
	
	while(digit < l) // repeate to be ready to accept next message untill the last digit is used
	{
		MPI_Recv(&digit, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); // receive the next digit
		if(digit < l)
		{
			digit++;
			MPI_Recv(data, 100, MPI_CHAR, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status); // reveive the message
			printf("P:%d Got data %s from process %d \n", rank, data, status.MPI_SOURCE);
			
			if(digit < l)
			{
				dl = strlen(data);
				data[dl] = num[digit]; // add the next number
				dl++;
				rec = num[digit] - '0'; // find the next process
				MPI_Send(&digit, 1,  MPI_INT, rec, 1, MPI_COMM_WORLD);// send digit and message to the next process, same like process 0 did.
				MPI_Send(data,  dl, MPI_CHAR, rec, 2, MPI_COMM_WORLD);
			}
			else
				for(i=0; i<10; i++)
					if(i != rank)
						MPI_Send(&digit, 1,  MPI_INT, i, 1, MPI_COMM_WORLD); // after the last digit is used, send the last digit position to all process, sp that everyone will end.
		}
	}
	
	MPI_Finalize(); // finalize
	return;
}
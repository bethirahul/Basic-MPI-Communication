//********************************************************************
//
// Rahul Bethi
// Advanced Operating Systems
// Programming Project #1: Basic MPI Communication
// October 13, 2016
// Instructor: Dr. Ajay K. Katangur
//
// Part 3
//
//********************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h> // for srand, generating random number

int main(int argc, char *argv[])
{
	int rank, i, j, size, root = 0;
	int signal[4], opp, pos, group, stage = 0, alive = 1;
	int players[100], t_players, t_stages = 0, winner, t, p, c, result;
	
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size); // to know the number of processes in this communication, to know the number of players
	
	srand(time(NULL) + rank); // to generate random number (0, 1, 2) (rock, paper, scossor)
	
	if(size == 1) // check the number of processes, minimum 2 processes are play this game.
	{
		if(rank == 0) // Error message is displayed only once by process 0
			printf("Invalid Input: Cannot play with one player!\n\n"); 
		MPI_Finalize(); // End the process if the size is below 2
		return;
	}
	
	// process 0
	if(rank == 0)
	{
		for(i=0; i<size; i++)
			players[i] = i;   // initiate players
		t_players = size;
		i = t_players;
		while(i > 1) // calculate total number of stages by dividing number of players by 2 everytime
		{
			if(i%2 != 0) // handling odd numbers while calculating number of stages
				i++;
			i = i/2;
			t_stages++;
		}
		
		// stages are now run in a loop, each process is given information to play in each stage
		for(stage=1; stage<=t_stages; stage++)
		{
			t = t_players;
			if(t%2 != 0) // playing only with even numbers, so that the last odd number can be sent to next stage
				t--;
			for(i=0; i<t; i++) // intiating signal (information) for all the participants
			{
				if(players[i] != rank)
				{
					p = i%2; // deciding in which postion the player is, in its pair 
					signal[0] = players[i+(1-(2*p))]; // determining its opponent
					signal[1] = p;
					signal[2] = stage;
					
					if(stage == t_stages) // setting final stage, for the end process to finish after the final winner is found.
						signal[3] = 1;
					else
						signal[3] = 0;
					
					MPI_Send(signal, 4, MPI_INT, players[i], 0, MPI_COMM_WORLD); // this signal is sent from process 0 to all the participants in this stage
				}
			}
			
			if(alive == 1) // alive tells if the process is still ready to play with others or in the next stage
			{
				winner = detWinner(rank, players[rank+1], 0, stage); // this function is used to calculate the winner amoung its pair
				if(winner!= rank)
					alive = 0; // end if teh game is lost
				MPI_Send(&winner, 1, MPI_INT, root, 3, MPI_COMM_WORLD); // send the winner information to the main process 0
			}
			
			for(i = 0; i<(t/2); i++)
			{
				MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status); // collect winner information from all the processes which are on the postion 0 of their pair.
				players[i] = result; // collect every winner into the players array to play with them again in the next stage
			}
			
			p = (t/2)-1; // sorting the obtained winners
			for(j=0; j<p; j++)
			{
				for(i=0; i<p; i++)
				{
					if(players[i+1] < players[i])
					{
						c = players[i];
						players[i] = players[i+1];
						players[i+1] = c;
					}
				}
			}
			
			if( t!= t_players ) // adding the last odd player left into the next stage
			{
				players[t/2] = players[t];
				t_players = (t/2)+1;
			}
			else
				t_players = t/2;
		}
		printf("Process %d wins the rock, paper, scissors tournament\n", players[0]); // final result
	}
	else // remaining processes, except process 0
	{
		while(alive == 1) // repeat if alive
		{
			MPI_Recv(signal, 4, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status); // receive signal from process 0. Opponent, position, stage
			
			winner = detWinner(rank, signal[0], signal[1], signal[2]); // finding out the winner amoung its pair
			if(winner != rank)
				alive = 0; // to end if the game is lost, instead of repeating, instead of waiting for the next stage (signal)
			if(signal[1] == 0)
				MPI_Send(&winner, 1, MPI_INT, root, 3, MPI_COMM_WORLD);  // if the process is in postion 0, send the winner information to process 0
			// the process which is in the position 0 is sent, instead of the one in the position 1, to remove duplications and unnecessary data transfers.
			
			if(signal[3] == 1) // if the last stage is done, then end.
				alive = 0;
		}
	}
	MPI_Finalize(); // to finalize the processes
	return;
}

//********************************************************************
//
// Determining the Winner
//
// This function handles both the processes which are playing against
// eachother. It generates values 0, 1, 2. Which are taken as rock
// paper scissor. For one process it gets value form its opposition
// and check the result, decides the winner and return the winner. For
// the other process
//
// Return Value
// ------------
// winner     int           Winner amoung the two processes
//
// Parameters
// ----------------
// rank		int		   Rank of the process
// opp        int         Rank of the opposite process
// pos        int         Position 0 or 1 to run winner calculation
// stage      int         to display in which stage they are
//
// Local Variables
// ---------------
// status     MPI_Status		MPI status variable to collect status
//
//*******************************************************************

int detWinner(int rank, int opp, int pos, int stage)
{
	int winner = -1, answer, oppAnswer;
	MPI_Status status;
	while(winner == -1) // play until the winner is found out
	{
		answer = (rand() % 10)/3; // random number generation (0, 1, 2)
		if(pos == 1) // process which is in position 1
		{
			MPI_Send(&answer, 1, MPI_INT, opp, 1, MPI_COMM_WORLD); // send result to process in position 0
			MPI_Recv(&winner, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status); // receive the winner information, to end if the winner is found.
		}
		else // process which is in postion 0
		{
			MPI_Recv(&oppAnswer, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status); // receives the result of the process in postion 1 to compare and decide winner
			if(oppAnswer != answer) // winner can be decided if both the results are different
			{
				if(answer == 0) //if rock
				{
					if(oppAnswer == 2)
						winner = rank; // win if opponent is scissor
					else
						winner = opp; // loose if opponent is paper
				}
				else if(answer == 1)
				{
					if(oppAnswer == 0)
						winner = rank;
					else
						winner = opp;
				}
				else if(answer == 2)
				{
					if(oppAnswer == 1)
						winner = rank;
					else
						winner = opp;
				}
			}
			MPI_Send(&winner, 1, MPI_INT, opp, 2, MPI_COMM_WORLD); // send the winner information to opponent (position 1)
		}
	}
	if(rank == winner)
		printf("Stage:%d Process %d wins over process %d\n", stage, rank, opp); // display the result
	return winner;
}
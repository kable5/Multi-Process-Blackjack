/***************************************************************************
*File: krisAblesPgm2.cpp
*Author: Kris Ables
*Procedures:
*main 		-tests the program & splits the program into processes
*shuffle	-shuffles an array based on a random seed
*nullArray	-erases values in an array
*printArray	-prints values in an array for debug purposes
*countTotal	-counts the value of the deck
*isAce		-determines if an array holds a soft ace	
*player1 	-performs the first player process
*player2 	-performs the second player process
*dealer 	-performs the dealer process
***************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

sem_t *sem1 = sem_open("key",O_CREAT,S_IRUSR|S_IWUSR,0);    //initialize semaphore for player 1 as a global variable
sem_t *sem2 = sem_open("key2",O_CREAT,S_IRUSR|S_IWUSR,0);   //initialize semaphore for player 2 as a global variable

/**************************************************
*void shuffle(int array[], int n)
*Author: Kristopher Ables
*Date: 3 March 2021
*Description - shuffles an array of a given size,
		used to shuffel deck at the beginning
		of the dealer process
*Parameters:
*array[] I/P into	-The array to be shuffled
*n I/P int		-The size of the array that is
			shuffled
**************************************************/
void shuffle(int array[], int n)
{
	for(int i=0;i<n;i++)                	//iterates i from zero to n-1
	{
		int r = i + (rand() % (n-i));   //random index from range i to n

		int temp= array[i];             //swaps values of index i & values of index r
		array[i]=array[r];
		array[r]=temp;
	}
}

/**************************************************
*void count total(int array[])
*Author: Kristopher Ables
*Date: 10 March 2021

*Description - sets an array to all null values

*Parameters:
*array[] I/P int	-The array to be totaled
*n I/P int		-Starting index
**************************************************/
void nullArray(int array[],int n)
{
	int i=n;		//begining index=0;
	while(array[i]!=NULL)	//while the index of the array exists
	{
		array[i]=NULL;	//set that index to null
		i++;		//increment
	}
}

/**************************************************
*void printArray(int array[])
*Author: Kristopher Ables
*Date: 3 March 2021

*Description - Prints the array for debug purposes

*Parameters:
*array[] I/P int 	-The array to be totaled
**************************************************/
void printArray(int array[])
{
	int i=0;				//set index to the begining of the array
	while(array[i]!=NULL)			//while there are values in the array
	{
		printf("%d ", array[i]);	//print the value
		i++;				//increment
	}
	printf("\n");				//print newline
}

/**************************************************
*void count total(int array[])
*Author: Kristopher Ables
*Date: 3 March 2021

*Description - adds up all values of a given array

*Parameters:
*array[] I/P int 	-The array to be totaled
**************************************************/
int countTotal(int array[])
{
	int total = 0;					//initialize total
	int totalSoft =0;				//initialize soft total (for when aces=1)
	int i=0;					//initialize index beginning at zero
	while(array[i]!=NULL)				//while index isnt null
	{
		if(array[i]==11)			//if ace occurs
			totalSoft = totalSoft+1;	//add 1 to the soft total
		else
			totalSoft=totalSoft+array[i];   //o/w add as normal to soft total

		total = total + array[i];		//total += array @ index i
		i++;					//increment i
	}
	if(total>21)					//if the total is greater than 21
		return totalSoft;			//return the soft total
							//to account for aces
	else
		return total;				//else return total as normal
}

/**************************************************
*void count total(int array[])
*Author: Kristopher Ables
*Date: 11 March 2021

*Description - determines whether or not an ace is
		present, and wheter or not that ace
		is able to be either a 1 or 11 

*Parameters:
*array[] I/P int 	-The array to be searched
**************************************************/
bool isAce(int array[])
{
	int i=0;					//set index to 0
	int softTotal=0;				//sets soft total to 0
	while(array[i]!=NULL)				//iterates through the array
	{						//to find the soft total of array
		softTotal=softTotal+array[i];
		i++;
	}
	i=0;						//sets index back to 0
	while(array[i]!=NULL)
	{				
		if(array[i]==11 && softTotal<22)	//if there is an ace and its not forced to be 1
			return true;			//return true
		i++;					//increment index
	}
	return false;					//when at the array and cannot satisfy conditions, return false
}

/**************************************************
*void player(1int d2p1[], int p12d[],n)
*Author: Kristopher Ables
*Date: 3 March 2021

*Description - operates the player1 process
		hits if dealer card shown is less
		than 7 & player greater than 13 or
		if player is less than 17 overall
		BUT only hits a maximum of once

*Parameters:
*d2p1[] I/P int 	-pipe from dealer to player 1
*p12d[] I/P int		-pipe from player 1 to dealer
*n I/P int 		-number of trials performed
**************************************************/
void player1(int d2p1[], int p12d[], int n)
{
	close(d2p1[1]);							//close write end of the dealer to player pipe
	close(p12d[0]);							//close write end of the dealer to player pipe

	for(int i=0;i<n;i++)						//loops for n iterations
	{

		int dealer;						//initialize dealers shown card
		int dealt;						//initialize card being dealt to player
		int hand[15];						//initialize hand
		int topOfHand=0;					//initialize begining index of hand

		read(d2p1[0], &dealt, sizeof(dealt));			//read the dealt card
		hand[topOfHand++] = dealt;				//add that dealt card to the hand

		read(d2p1[0], &dealt, sizeof(dealt));			//read dealt card
		hand[topOfHand++] = dealt;				//add that dealt card to the hand

		nullArray(hand,topOfHand);				//ensures that the rest of the array is clear

		read(d2p1[0], &dealer, sizeof(dealer));			//read dealers shown card and save it

		int stay =0;						//initialize decision for hit or stay as hit
		dealt = 0;						//dealt is initialized as 0 to pass to the while loop at least once
		
		while(dealt!=-1)					//while the dealt card is not -1 (no card dealt)
		{
			if(countTotal(hand)>=17 ||			//if the hand is greater than 17 or the hand is
				(countTotal(hand)>=12 && dealer<7))	//greater than 12 when the dealers shown is less than 7
			{
				stay=1;					//stay, else stay 0 for hit
			}

			write(p12d[1],&stay,sizeof(stay));		//send decision to dealer
			read(d2p1[0], &dealt, sizeof(dealt));		//read dealt card

			if(dealt!=-1)					//if the dealt card is actually a card
				hand[topOfHand++] = dealt;		//add that dealt card to the hand
		}

		int player1Total=countTotal(hand);			//get total of players hand
		write(p12d[1],&player1Total,sizeof(player1Total));	//send that total to the dealer

		sem_post(sem1);						//unlock first semaphore to allow player 2 to participate
		sem_wait(sem2);						//wait for second semaphore to complete while player 2 takes its turn
	}

}

/**************************************************
*void player2(int d2p2[], int p22d[],n)
*Author: Kristopher Ables
*Date: 11 March 2021

*Description - operates the player2 process
		hits if dealer card shown is less
		than 7 & player greater than 13 or
		if player is less than 17 overall
		or if ace is in hand and over 17

*Parameters:
*d2p2[] I/P int		-pipe from dealer to player 2
*p22d[] I/P int		-pipe from player 2 to dealer
*n I/P int 		-number of trials performed
**************************************************/
void player2(int d2p2[], int p22d[] ,int n)
{
	close(d2p2[1]);							//close write end of the dealer to player pipe
	close(p22d[0]);							//close write end of the dealer to player pipe

	for(int i=0;i<n;i++)						//loops for n iterations
	{
		sem_wait(sem1);						//wait for first semaphore to complete while player 1 takes its turn				

		int dealer;						//initialize dealers shown card
		int dealt;						//initialize card being dealt to player
		int hand[15];						//initialize hand
		int topOfHand=0;					//initialize begining index of hand

		read(d2p2[0], &dealt, sizeof(dealt));			//read the dealt card
		hand[topOfHand++] = dealt;				//add that dealt card to the hand

		read(d2p2[0], &dealt, sizeof(dealt));			//read dealt card
		hand[topOfHand++] = dealt;				//add that dealt card to the hand

		nullArray(hand,topOfHand);				//ensures that the rest of the array is clear

		read(d2p2[0], &dealer, sizeof(dealer));			//read dealers shown card and save it

		int stay =0;						//initialize 0 decision for hit or stay as hit
		dealt = 0;						//dealt is initialized as 0 to pass to the while loop at least once
		
		while(dealt!=-1)					//while the dealt card is not -1 (no card dealt)
		{
			if(isAce(hand) && countTotal(hand)>=18)		//if there is an ace in hand
			{						//and the total is greater than 18
				stay=1;					//stay, else stay=0 for hit
			}
			else if(countTotal(hand)>=17 ||			//if no ace and the hand is greater than 17 or the hand is
				(countTotal(hand)>=12 && dealer<7))	//greater than 12 when the dealers shown is less than 7
			{
				stay=1;					//stay, else stay=0 for hit
			}
			
			write(p22d[1],&stay,sizeof(stay));		//send decision to dealer
			read(d2p2[0], &dealt, sizeof(dealt));		//read dealt card

			if(dealt!=-1)					//if the dealt card is actually a card
				hand[topOfHand++] = dealt;		//add that dealt card to the hand
		}

		int player2Total=countTotal(hand);			//get total of players hand
		write(p22d[1],&player2Total,sizeof(player2Total));	//send that total to the dealer

		sem_post(sem2);						//unlock the second semaphore to allow the first player to participate
	}
}

/**************************************************
*void dealer(int d2p1[], int p12d[], int d2p2[], int p22d[], int n)
*Author: Kristopher Ables
*Date: 3 March 2021

*Description - operates the dealer process, deals to
		and handles game-side computations and 
		operations for the player processes

*Parameters:
*d2p1[] I/P int		-pipe from dealer to player 1
*p12d[] I/P int		-pipe from player 1 to dealer
*d2p2[] I/P int		-pipe from dealer to player 2
*p22d[] I/P int		-pipe from player 2 to dealer
*n I/P int 		-number of trials performed
**************************************************/
void dealer(int d2p1[], int p12d[], int d2p2[], int p22d[], int n)
{
	close(d2p1[0]);										//close the read end of the dealer to player pipe
	close(p12d[1]);										//close the write end of the player to dealer pipe

	int player1Wins=0;									//initialize the player win counter
	int player2Wins=0;
	int dealerWins=0;									//initialize the dealer win counter

	srand(time(0));										//set the random seed for shuffle function

	for(int i=0;i<n;i++)									//loops for n iterations
	{

		/****************************************
		*******Setup Deck and Dealer Hand********
		****************************************/

		int deck[] = {11,11,11,11,							//initializes the deck with determined values
					2,2,2,2,						//11 for aces, 10 for all face cards
					3,3,3,3,
					4,4,4,4,
					5,5,5,5,
					6,6,6,6,
					7,7,7,7,
					8,8,8,8,
					9,9,9,9,
					10,10,10,10,
					10,10,10,10,
					10,10,10,10,
					10,10,10,10,
					NULL};

		int topOfDeck = 51;								//keeps track of index at the top of the deck
												//kind of a dumb way of doing things, may need to
												//implement through a queue

		shuffle(deck, 52);								//shuffles the deck

		int hand[15];									//initializes hand array
												//(max hand size 11 + 4 for buffer)
		int topOfHand=0;

		hand[topOfHand++]=deck[topOfDeck--];						//deals 2 cards to himself
		hand[topOfHand++]=deck[topOfDeck--];						//while decrementint top of deck index

		nullArray(hand,2);

		/****************************************
		*********Operation for player 1**********
		****************************************/

		write(d2p1[1],&deck[topOfDeck],sizeof(deck[topOfDeck]));			//write the top card of the deck to give to player 1
		topOfDeck--;									//decrement topOfDeck index

		write(d2p1[1],&deck[topOfDeck],sizeof(deck[topOfDeck]));			//write the top card of the deck to give to player 1
		topOfDeck--;									//decrement topOfDeck ind

		write(d2p1[1], &hand[0], sizeof(hand[0]));					//write the dealers shown card to player 1
	
		int stay;									//holds the stay decision

		int deal=0;									//holds dealt card, init as 0 to entire while loop at least once
		while(deal!=-1)									//while there is a card that needs to be dealt
		{
			read(p12d[0], &stay, sizeof(stay));					//reads in stay decision
			if(stay==0)								//if player hits
			{
				deal=deck[topOfDeck];						//deal set to top of deck
				topOfDeck--;							//decrement top of deck
			}
			else 									//if the player stays
				deal=-1;							//deal set to -1 to signal breaking of while loop
												//in dealer and in player2 process
			write(d2p1[1],&deal,sizeof(deal));					//send the dealt card to the player
		}

		int player1Total;								//initialize player total count
		read(p12d[0], &player1Total, sizeof(player1Total));				//read player total count from the player

		/****************************************
		*********Operation for player 2**********
		****************************************/

		write(d2p2[1],&deck[topOfDeck],sizeof(deck[topOfDeck]));			//write the top card of the deck to give to player 2
		topOfDeck--;									//decrement topOfDeck index

		write(d2p2[1],&deck[topOfDeck],sizeof(deck[topOfDeck]));			//write the top card of the deck to give to player 2
		topOfDeck--;									//decrement topOfDeck index

		write(d2p2[1], &hand[0], sizeof(hand[0]));					//write the dealers shown card to player 2

		deal=0;										//holds dealt card, init as 0 to entire while loop at least once
		while(deal!=-1)									//while there is a card that needs to be dealt
		{
			read(p22d[0], &stay, sizeof(stay));					//reads in stay decision
			if(stay==0)								//if player hits
			{
				deal=deck[topOfDeck];						//deal set to top of deck
				topOfDeck--;							//decrement top of deck
			}
			else 									//if the player stays
				deal=-1;							//deal set to -1 to signal breaking of while loop
												//in dealer and in player2 process
			write(d2p2[1],&deal,sizeof(deal));					//send the dealt card to the player
		}

		int player2Total;								//initialize player total count
		read(p22d[0], &player2Total, sizeof(player2Total));				//read player total count from the player

		/****************************************
		**********Finalize Game Result***********
		****************************************/

		while(countTotal(hand)<17)							//deals cards to himself till total greater than 17
		{
			hand[topOfHand++]=deck[topOfDeck--];					//deals card to himself
		}

		int dealerTotal = countTotal(hand);						//initialize dealer total count
		bool dealerWon1 = true;
		bool dealerWon2 = true;

		if((player1Total<=21 && player1Total>dealerTotal)				//victor between dealer & player 1
						||(player1Total <=21 && dealerTotal>21))	//if the player is greater than dealer and had not busted
		{
			//printf("PLAYER 1 WON\n");						//print player won for debug purposes	
			player1Wins++;								//increment total player 1 wins		
			dealerWon1 = false;
		}

		if((player2Total<=21 && player2Total>dealerTotal)				//victor between bealer & player 2
						||(player2Total <=21 && dealerTotal>21))	//if the player is greater than dealer and had not busted
		{
			//printf("PLAYER 2 WON\n");						//print player won for debug purposes	
			player2Wins++;								//increment total player 2 wins	
			dealerWon2 = false;	
		}
		if(dealerWon1 && dealerWon2)							//if the dealer won
		{
			//printf("DEALER WON\n");						//print dealer won for debug purposes
			dealerWins++;								//increment total dealer wins
		}
	}

	/****************************************
	**************Final Result***************
	****************************************/

	printf("PLAYER 1 WINS: %d\n",player1Wins);						//print final number of wins for player 1
	printf("PLAYER 2 WINS: %d\n",player2Wins);						//print final number of wins for player 2
	printf("DEALER WINS: %d\n",dealerWins);							//and dealer

	float player1Rate = ((float)player1Wins/(float)n)*100;					//initialize the win rate for player 1
	float player2Rate = ((float)player2Wins/(float)n)*100;					//& player 2

	printf("Player 1 Win Rate: %.1f%\n",player1Rate);					//display win rate for player 1
	printf("Player 2 Win Rate: %.1f%\n",player2Rate);					//& player 2
}

/**************************************************
*int main(int argc, char*argv[])
*Author: Kristopher Ables
*Date: 3 Feburary 2021

*Description - forks and begins the processes

*Parameters:
*argc I/P int		-Number of arguments on the cmd line
*argv I/P char*[]	-Arguments on the cmd line

*main O/P int		-Status code
**************************************************/
int main(int argc, char *argv[])
{
	int n=1000;					//number of trials

	int d2p1[2];					//initialize dealer to player 1 array
	int p12d[2];					//initialize player 1 to dealer array
	int d2p2[2];					//initialize dealer to player 2 array
	int p22d[2];					//initialize player 2 to dealer array

	pipe(d2p1);					//form dealer to player 1 array into pipe
	pipe(p12d);					//form player 1 to dealer array into pipe
	pipe(d2p2);					//form dealer to player 2 array into pipe
	pipe(p22d);					//form player 2 to dealer array into pipe

	pid_t pid1 = fork();				//First fork the program
	pid_t pid2 = fork();				//Second fork of the program (necessary for >1 child process)

	if(pid1>0 && pid2>0)				//if process is parent for both
	{
		dealer(d2p1, p12d, d2p2, p22d, n);	//perform dealer function
	}
	if(pid1==0 && pid2>0)				//if the process is a child of 1 and a parent of 2
	{
		player2(d2p2,p22d,n);			//perform the player 2 function
	}
	if(pid1==0 && pid2==0)				//if process is child of both
	{					
		player1(d2p1,p12d,n);			//perform player 1 function
	}

	sem_close(sem1);				//close the active semaphores
	sem_close(sem2);

	sem_unlink("key");				//unlink the active semaphores
	sem_unlink("key2");

	return 0;					//end program
}

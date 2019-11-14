// Author @ Kemal Turksonmez
// compile this program using:
// gcc schedAlgo.c -lgsl -lgslcblas -lm -o schedAlgo

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

const int columns = 6; 

// Calculate average tt_i
float getAvgTTi(int arr[][columns], int n){
	float sum = 0;
	for (int i = 0; i < n; i++){
		sum += arr[i][5];
	}
	sum = sum/n;
	return sum;
}

// FIFO scheduling algorithm implementation
// arr: array of initial variables for simulation
// n: number of rows in array
// simNum: simulation number
float fifo(int arr[][columns], int n, int simNum){
	FILE * fp;

	//open file
   	char fileName[30] = "simulationlogs/fifoLog";
    sprintf(fileName+22, "%d", simNum);
    strcat(fileName, ".txt");
   	fp = fopen (fileName,"w");

   	//copy array over
	int tempArr[n][columns];
	for (int i = 0; i < n; i++){
		for (int j = 0; j < columns; j++){
			tempArr[i][j] = arr[i][j];
		}
	}
	//declare array to act as fifo queue
	int queue[n];
	for (int i = 0; i < n; i++){
		queue[i] = -1;
	}
	int elemTrack = 0;
	int time = 0;
	int procNum = -1;
	
	fprintf(fp, "Process\tActive\tArrival Time\tTotal CPU Time\tRemaining CPU Time\tTurnaround Time\n");
	for (int currPlace = 0; currPlace < n; ){
		fprintf(fp,"Time is: %i\n", time);
		//add element to queue
		for (int i = 0; i < n && elemTrack < n; i++){
			if (tempArr[i][2] == time){
				queue[elemTrack] = tempArr[i][0];
				// printf("Added process %i to queue \n", queue[elemTrack]);
				elemTrack++;
			}
		}
		if (queue[currPlace] == -1) {
			// do nothing
		} else if (procNum != -1){
			// decrement remaining time
			tempArr[procNum][4] -= 1;
			if (tempArr[procNum][4] == 0){
				// set active bit to 0
				tempArr[procNum][1] = 0;
				// set tt_i time
				tempArr[procNum][5] = time - tempArr[procNum][2];
				// printf("Done with process %i\n", queue[currPlace]);
				// increment place in queue
				currPlace++;
			}
		}
		if (currPlace < n && procNum != queue[currPlace]){
			//update process number
			procNum = queue[currPlace];
			// set process bit to be active
			tempArr[procNum][1] = 1;
			// printf("Starting process %i with total cpu time %i\n", queue[currPlace], tempArr[procNum][3]);
		}
		for (int i = 0; i < n; i++){
			fprintf(fp, " %i\t  %i\t    %i   \t\t%i\t\t%i\t\t  %i\n", tempArr[i][0],tempArr[i][1],tempArr[i][2],tempArr[i][3],tempArr[i][4],tempArr[i][5]);
		}
		fprintf(fp,"\n\n");
		time++;		
	}
	fclose (fp);
	return getAvgTTi(tempArr, n);
}

// Inserts an element into a queue sorted by some variable
// queue: queue of processes
// arr: holds different variables processes can be sorted by
// n: number of processes
// col: the column that the processes are sorted by
// tail: pointer to tail of queue
void insertSorted(int *queue, int arr[][columns], int n, int col, int proc, int tail){
	if (tail < 0){
		queue[0] = proc;
	}
	else {
		// declare i
		int i;
		// find first element with remaing time greater than new process
		for (i = 0; i <= tail; i++){
			if (arr[queue[i]][col] > arr[proc][col]){
				break;
			}
		}
		int prevVar = queue[i];
		int newVar;
		for (int j = i; j < n; j++){
			if (j == i){
				queue[j] = proc;
			}
			if (j < n-1) {
				// save next variable
				newVar = queue[j+1];
				// push current element one back
				queue[j+1] = prevVar;
				prevVar = newVar;
			}
		}
	}
}

// Removes a process when its done with it, since the simulation only works on the process
// with the shortest time, this will always be the element in first place
// queue: queue of processes sorted by shorted remaining time
// tail: pointer to tail of queue (where the last process is located)
void removeSorted(int *queue, int tail, int currProc){
	if (tail == 0){
		queue[0] = -1;
	} else {
		int proc = 0;
		for (int i = 0; i <= tail; i++){
			if (queue[i] == currProc){
				proc = i;
				break;
			}
		}
		//find currProc in array
		for (int i = proc; i < tail; i++){
			//shift variables back
			queue[i] = queue[i+1];
		}
	}
}

// SRT scheduling algorithm implementation
// arr: array of initial variables for simulation
// n: number of rows in array
// simNum: simulation number
float srt(int arr[][columns], int n, int simNum){
	FILE * fp;

	//open file
   	char fileName[30] = "simulationlogs/srtLog";
    sprintf(fileName+21, "%d", simNum);
    strcat(fileName, ".txt");
   	fp = fopen (fileName,"w");

   	//copy array over
	int tempArr[n][columns];
	for (int i = 0; i < n; i++){
		for (int j = 0; j < columns; j++){
			tempArr[i][j] = arr[i][j];
		}
	}

	//declare array to act as srt queue
	int queue[n];
	//tracks the tail of the array
	int tailPtr = -1;
	for (int i = 0; i < n; i++){
		queue[i] = -1;
	}

	// time since simulation started
	int time = 0;
	//tracks current process
	int currProc = -1;
	// number of elements added to queue since beginning
	int elemTrack = 0;

	fprintf(fp, "Process\tActive\tArrival Time\tTotal CPU Time\tRemaining CPU Time\tTurnaround Time\n");
	for (int numProc = 0; numProc < n; ){
		fprintf(fp,"Time is: %i\n", time);
		// printf("Time is: %i\n", time);
		//add element to queue

		// if no element is in the array
		if (queue[0] == -1){
			// do nothing
			// if simulating a process
		} else if (currProc != -1){
			// swap process if srt queue has a new srt process (preempt)
			if (currProc != queue[0]){
				//flip bits
				// printf("Swapping process %i that has remaining time %i, with process %i that has remaining time %i\n", currProc, tempArr[currProc][4], queue[0],tempArr[queue[0]][4]);
				tempArr[currProc][1] = 0;
				currProc = queue[0];
				tempArr[currProc][1] = 1;
			}
			// decrement remaining time
			tempArr[currProc][4] -= 1;
			// if on a current process
			if (tempArr[currProc][4] == 0){
				// set active bit to 0
				tempArr[currProc][1] = 0;
				// set tt_i time
				tempArr[currProc][5] = time - tempArr[currProc][2];
				// printf("Done with process %i\n", currProc);
				// remove element
				removeSorted(queue, tailPtr, currProc);
				//decrement tail
				tailPtr--;
				// increment number of processes completed
				numProc++;
				currProc = -1;
			} 
		}
		for (int i = 0; i < n && elemTrack < n; i++){
			if (tempArr[i][2] == time){
				// the queue is only sorted when a new element is inserted
				// this is because the simulation only uses process at the 
				// front of the queue, therefore only those processes have
				// a decremeting time. This means that unless a new element
				// with a shorter remaining time is inserted, then queue[0]
				// will maintain the shortest remaining time
				insertSorted(queue,tempArr,n,4,i,tailPtr);
				tailPtr++;
				// printf("Added process %i to queue with remaining time %i, \n", i, tempArr[i][4]);
				elemTrack++;
			}
		}
		// Set currProc to be queue[0]
		if (queue[0] != -1 && currProc == -1){
			//update current process
			currProc = queue[0];
			// set process bit to be active
			tempArr[currProc][1] = 1;
			// printf("Starting process %i with total remaining time %i\n", currProc, tempArr[currProc][4]);
		}
		for (int i = 0; i < n; i++){
			fprintf(fp, " %i\t  %i\t    %i   \t\t%i\t\t%i\t\t  %i\n", tempArr[i][0],tempArr[i][1],tempArr[i][2],tempArr[i][3],tempArr[i][4],tempArr[i][5]);
		}
		fprintf(fp,"\n\n");
		time++;		
	}
	fclose (fp);
	return getAvgTTi(tempArr, n);
}

// SPT scheduling algorithm implementation
// arr: array of initial variables for simulation
// n: number of rows in array
// simNum: simulation number
float spt(int arr[][columns], int n, int simNum){
	FILE * fp;

	//open file
   	char fileName[30] = "simulationlogs/sptLog";
    sprintf(fileName+21, "%d", simNum);
    strcat(fileName, ".txt");
   	fp = fopen (fileName,"w");

   	//copy array over
	int tempArr[n][columns];
	for (int i = 0; i < n; i++){
		for (int j = 0; j < columns; j++){
			tempArr[i][j] = arr[i][j];
		}
	}

	//declare array to act as srt queue
	int queue[n];
	//tracks the tail of the array
	int tailPtr = -1;
	for (int i = 0; i < n; i++){
		queue[i] = -1;
	}

	// time since simulation started
	int time = 0;
	//tracks current process
	int currProc = -1;
	// number of elements added to queue since beginning
	int elemTrack = 0;
	fprintf(fp, "Process\tActive\tArrival Time\tTotal CPU Time\tRemaining CPU Time\tTurnaround Time\n");
	for (int numProc = 0; numProc < n; ){
		fprintf(fp,"Time is: %i\n", time);
		// printf("Time is: %i\n", time);
		//add element to queue
		for (int i = 0; i < n && elemTrack < n; i++){
			if (tempArr[i][2] == time){
				// the queue is only sorted when a new element is inserted
				// this is because the simulation only uses process at the 
				// front of the queue, therefore only those processes have
				// a decremeting time. This means that unless a new element
				// with a shorter remaining time is inserted, then queue[0]
				// will maintain the shortest remaining time
				insertSorted(queue,tempArr,n,3,i,tailPtr);
				tailPtr++;
				// printf("Added process %i to queue with remaining time %i, \n", i, tempArr[i][4]);
				elemTrack++;
			}
		}		

			// if no element is in the array
			if (queue[0] == -1){
				// do nothing
			// if simulating a process
			} else if (currProc != -1){
			// decrement remaining time
			tempArr[currProc][4] -= 1;
			if (tempArr[currProc][4] == 0){
				// set active bit to 0
				tempArr[currProc][1] = 0;
				// set tt_i time
				tempArr[currProc][5] = time - tempArr[currProc][2];
				// printf("Done with process %i\n", queue[0]);
				// remove element
				removeSorted(queue, tailPtr, currProc);
				//decrement tail
				tailPtr--;
				// increment number of processes completed
				numProc++;		
				// set current process back to -1
				currProc = -1;
			} 
		}
		// Set currProc to be queue[0]
		if (queue[0] != -1 && currProc == -1){
			//update current process
			currProc = queue[0];
			// set process bit to be active
			tempArr[currProc][1] = 1;
			// printf("Starting process %i with total cpu time %i\n", currProc, tempArr[currProc][3]);
		}
		for (int i = 0; i < n; i++){
			fprintf(fp, " %i\t  %i\t    %i   \t\t%i\t\t%i\t\t  %i\n", tempArr[i][0],tempArr[i][1],tempArr[i][2],tempArr[i][3],tempArr[i][4],tempArr[i][5]);
		}
		fprintf(fp,"\n\n");
		time++;		
	}
	fclose (fp);

	return getAvgTTi(tempArr, n);
}

//Struct for returning average turnaround times
struct Averages {
   float fifo;
   float spt;
   float srt;
};

// Create and table for storing arrival times, total cpu time, and process number
// k: upper bound of random arrival time
// d: mean of gaussian distribution
// n: number of processes
// simNum: simulation number
struct Averages createSimTable(int k, int d, int n, int simNum){
	//intialize gsl variables
	const gsl_rng_type * T;
    gsl_rng * r;

    //initialize file object
    FILE * fp;

   	//open file
   	char fileName[35] = "simulationlogs/processTable";
    sprintf(fileName+27, "%d", simNum);
    strcat(fileName, ".txt");
   	fp = fopen (fileName,"w");

	//table to store active bit, arrival time, 
	// total cpu time, remaining cpu time, turnaround time
	int simTable[n][columns];

	//setup gsl
	gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    

    // v will always be less than d
    int v = (int)((k/n)%d);
    if (v == 0){
    	v = (int)((n/k)%d);
    }
    // initialize variable for storing random gaussian distributed values
    int tempVal;

    fprintf(fp, "Process\tActive\tArrival Time\tTotal CPU Time\tRemaining CPU Time\tTurnaround Time\n");
	for (int i = 0; i < n; i++){
		// set seed
		gsl_rng_set(r, i*n);
		//set process number
		simTable[i][0] = i;
		// set arrival time
		simTable[i][2] = gsl_rng_uniform_int(r,k);
		// set active bit
		simTable[i][1] = 0;
		//store total cpu time
		tempVal = gsl_ran_gaussian(r, v);
		// shift gauss value by d
		if (tempVal + d <= 0){
			tempVal = -tempVal + d;
		} else { 
		tempVal += d;
		}
		
		// set cpu time
		simTable[i][3] = tempVal;
		// set remaing time
		simTable[i][4] = tempVal;
		// set turnaround time
		simTable[i][5] = -1;
		fprintf(fp, " %i\t  %i\t    %i   \t\t%i\t\t%i\t\t  %i\n", simTable[i][0],simTable[i][1],simTable[i][2],simTable[i][3],simTable[i][4],simTable[i][5]);
	}
	//free gsl
	gsl_rng_free (r);
	//close file
	fclose (fp);

	printf("Variables for simulation = k: %i, n: %i, d: %i\n",k,n,d);
	printf("Running FIFO\n");
	float fifoAv = fifo(simTable,n, simNum);
	printf("Running SPT\n");
	float sptAv = spt(simTable,n, simNum);
	printf("Running SRT\n");
	float srtAv = srt(simTable,n, simNum);

	return (struct Averages) {fifoAv, sptAv, srtAv};
}

void runSimulation(int k, int d, int n, int length){
	FILE * fp1, * fp2, * fp3;

	//open file
   	fp1 = fopen ("results/fifo.txt","w");
   	fp2 = fopen ("results/spt.txt","w");
   	fp3 = fopen ("results/srt.txt","w");

	printf("Starting simulation\n");
	fprintf(fp1, "#d\td/att\n");
	fprintf(fp2, "#d\td/att\n");
	fprintf(fp3, "#d\td/att\n");
	int currD = 0;
	for (int i = 1; i <= length; i++){
		currD = d+i;
		printf("Running simulation: %i\n", i);
		struct Averages avgs = createSimTable(k,currD,n,i);
		fprintf(fp1, "%i\t%f\n", currD, currD/avgs.fifo);
		fprintf(fp2, "%i\t%f\n", currD, currD/avgs.spt);
		fprintf(fp3, "%i\t%f\n", currD, currD/avgs.srt);
	}
	printf("Ending simulation\n");
	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
}	

int main(int argc, char *argv[]){
	if (argc < 5){
		printf("Please enter k, d, n, and number of iterations\n");
	} 
	else{
		int k = atoi(argv[1]);
		int d = atoi(argv[2]);
		int n = atoi(argv[3]);
		int iter = atoi(argv[4]);
		if (k < 1 || d < 0 || n < 1 || iter < 1){
			printf("Input values above 1 for k,d, and iterations and a value greater than or equal to 0 for d\n");
		} else{
			runSimulation(k,d,n,iter);
		}
	} 
	return 0;
}
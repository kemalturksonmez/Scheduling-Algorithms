Provided in the folder there is a schedAlgo.c file, a makefile, a runGnu.sh shell script, a simulationlogs directory, and a results directory.

The program itself has a compile command writen at the top of the file, if one wants to run the program. However, if the simulationlogs and results directories, dont exist then the program will throw a segmentation fault. Therefore it is easier to run the makefile.

So to setup the directories and create executables for the program and shell file, just run:
$make

To run the program:
$./schedAlgo k d n iterations
If the user doesn't run the program with all the necessary arguments the program will remind the user.

Running the program, the program will place each algorithm simulation in its own file with its own iteration label. This will be in the format fifo1.txt or srt5.txt. Each file will have the entire simulation for the set of given processes. The starting simulation set will be held in the processTable.txt files which are also labeled according to iteration. Once the program is completed, it will output three files in the results directory, fifo.txt, spt.txt, and srt.txt. These files store the relationships between d and d/att. To plot these files simply run the command:
$./runGnu.sh

This command should be run in the directory where the shell script is stored. For this shell script to work correctly the user needs to have gnuplot installed. To install this program run this command:
Linux:
$sudo apt-get update -y
$sudo apt-get install -y gnuplot
Mac:
$brew install gnuplot

If you didn't run the make command and compiled the program and created the directories yourself. Then you have to make the shell script executable. To do so run:
$chmod +x runGnu.sh

If you want to delete everything created by the program, run:
$make clean
This will delete the directories and the files in them, along with the program executable.

If you want to run the program again but don't want to delete the directories and create them again. Then simply run:
$make cleanfiles
This command will delete everything in the directories but not the directories themselves or the executable.

This covers everthing about running the program.

My program works by taking in the values for k, d, n and runs them over a certain number of iterations. Each iteration d increases by 1 and k and n stay the same. Since the point of this program was to study relationship between d and d/att. If you input a value less than 1 for k,d, and iterations or a values less than 0 for d, the program will warn you and exit. The way I structered my simulation d will start at d + 1, this is why 0 is a valid input.

In terms of my approach to the individual algorithms. I first make a processt table, this stores all the original values for each process. This includes process number, active bit, arrival time, cpu time, remaining time, and turnaround time. The turnaround time is -1 since this value is calculated after each process is completed. I store this table in a processTable.txt file with its according iteration number and pass the table to each algorithm. 

The first algorithm is FIFO. This algorithm works by creating an array that is a priority queue based on arrival. It tracks how many elements have been placed and a pointer to the current process being run. No elements are removed since the program just iterates the pointer. I could have also removed each element and worked the first element in the queue. To do so I could use the insertSorted and removeSorted methods I wrote. However, since there is no preemption this wasn't that important. The program will write the entire simulation to a file called fifo.txt with its according iteration number. 

Next I wrote SPT. This algorithm creates a priority queue which sorts based on shortest priority time. A process is added to the queue at its arrival time through the method insertSorted. All this method does is given a certain sorting parameter, it places element in the queue according to that sorting scheme. The algorithm picks whatever is at the front of the queue, completes it and removes it by using removeSorted. This process may not remain at the front of the queue since shorter elements can be added in, so the method seeks out the process in the queue and removes it. There is no preemption in this algorithm so each process is atomic. The simulation is stored in a file called spt.txt with its according iteration number.

Finally I wrote SRT. This algorithm creates a priority queue based on shortest remaining time. A process is added to the queue at its arrival time through the method insertSorted. The element is placed in its sorted order. Since there is preemption the simulation runs whatever is at the front of the queue. So if the simulation was running a process and realized it wasn't at the front of the queue; it will swap it for whatever process is at the front. Like the other two, the simulation is stored in a file called srt.txt with its according iteration number.

Finally each time a simulation is run each algorithm returns its ATT and the program stores each d and d/ATT for each algorithm in their own files. These files spt.txt, fifo.txt, and srt.txt can be found in the results directory. 

When the program is finished the user can the run the shell script runGnu to use gnuplot to plot each file. 

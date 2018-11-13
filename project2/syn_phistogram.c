/*
 *
 * ######################################################
 * Date: 12/11/2018
 * @Authors: Aldo Tali 21500097
 *           Umut Akos 21202015   
 * Description: Solution to the second project given
 * in Cs342 Operating Systems Fall 2018-2019.
 * Please Note: Much of this code makes use of the parts 
 * written in project 01. Since I (Aldo Tali) am the sole 
 * owner  of my first project I Aldo Tali, declare that the 
 * .c Files can be legitemily used by both autors of this 
 * project.In particular the methods reuused from project 1 
 * are :
 *      writeHistogramToFile(FILE *file, int *histogram, int sizei);
 *      initializeArray (int *array, int size, int initializer)
 *      getHistogram (FILE *file, int *histogram, double minvalue,double maxvalue, double width);
 * ######################################################
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h> 
#include <sys/time.h>
#include <time.h> 
#include <semaphore.h> 
#include <fcntl.h>         
#include <sys/stat.h>        
#include <sys/shm.h>


void initializeArray (int *array, int size, int initializer);
void getHistogram (FILE *file, int *histogram, double minvalue,double maxvalue, double width);
void writeHistogramToFile(FILE *file, int *histogram, int sizei);
int mainSimulator(int argc, char*argv[]);

//use this semaphore name for global
const char *semName = "osproject";


int main(int argc, char*argv[])
{
     
    struct timeval start, end; 
    gettimeofday(&start, NULL);
    mainSimulator(argc,argv);
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + 
              ((end.tv_usec - start.tv_usec)/1000000.0);
    printf("The elapsed time is: %f\n", elapsed);
 
    return 0;
}

//runs on wrong executable run
void runErrorDisplay(){
    printf("We are really sorry but the information provided is not the correct one. \n Try to check the way you run the executable. It should have the following format: \nsyn_phistogram minvalue maxvalue bincount N file1 ... fileN outfile\n\n");
    exit(1);
}

int mainSimulator(int argc, char*argv[]){
    //check for wrong user input
    if (argc < 7){
        runErrorDisplay();
    }

    //define the numbers gotten from the input
    double minvalue,maxvalue,width;
    int     bincount,N,i;

    //put the arguments in the variables for ease of use
    sscanf(argv[1],"%lf",&minvalue);
    sscanf(argv[2],"%lf",&maxvalue);
    sscanf(argv[3],"%d",&bincount);
    sscanf(argv[4],"%d",&N);

    //prevents segmentation fault if for some reason there are not N files
    if (argc != (N + 6)){
       runErrorDisplay();
    }
    // define the processid and the variables needed for each file name
    int pid;
    char name [60];
    char *temp;

    //make sure that the histogram is initially all 0
    int histogram[bincount];
    static int *sharedMemArray;
    initializeArray (histogram,bincount,0);
    
    int ids[N]; //keep this array to store the proccesses ids 
    int sharedMemID; //keep this for identyfing the shared memory space 
    void *shared_memory = (void *)0; //keep this for the shared memory
   
    //get the shared memory
    // It had the size of bincount size array of integers.
    sharedMemID = shmget((key_t)bincount, sizeof(int)*bincount, IPC_CREAT|0666);
    if (sharedMemID == -1){
        perror("We failed to get the adress of the shared memory : \n");
        exit(0);
    }

    //attach to the shared memory segment
    //identify that with the sharedMemID we declared before
    shared_memory = (int *)shmat(sharedMemID, NULL, 0);
    if (shared_memory == (void *)-1){
        perror("We failed to attach to the adress of the shared memory :\n");
        exit(0);
    }

    //run N process
    for (i = 0; i < N; i++)
    {
        //create child process and store its id
        pid = fork();
        ids[i] = pid;
        //run this for child processes
        if (pid == 0){

            //open the semaphores
            sem_t *blockingSemaphoreIdentifier = sem_open(semName, O_CREAT, 0600, 0);
            sem_t *blockingSemaphoreIdentifier2 = sem_open("semName", O_CREAT, 0600, 0);

            //semaphore access is garantued now, attach the shared memory
            shared_memory = (int *)shmat(sharedMemID, NULL, 0);
            sharedMemArray = (int *)shared_memory;    

            //make sure initially the array is full of 0 despite its previous values
            initializeArray (sharedMemArray,bincount,0);

            //use the respective file to read and generate the histogram
            FILE  *file = fopen(argv[5+i], "r");
            width = (maxvalue - minvalue)/ (double) bincount;
            getHistogram (file, sharedMemArray,minvalue,maxvalue,width);
         
            // release your semaphore  usage and try to do so with no errors
            if( sem_post(blockingSemaphoreIdentifier) < 0 ){
                 printf("Child Process : We could not release the semaphore properly \n");
            }
            
            exit(0);

        } else {

            //open the semaphores
            sem_t *blockingSemaphoreIdentifier = sem_open(semName, O_CREAT, 0600, 0);
            sem_t *blockingSemaphoreIdentifier2 = sem_open("semName", O_CREAT, 0600, 0);
        
            // wait for the children to finish their work before parent accesses memory
            if( sem_wait(blockingSemaphoreIdentifier) < 0 ){
                printf("The parent failed to wait, possibly your child processes were immaturely closed or lost\n"); 
            } 

            //reattach to the shared memory space  and access it 
            shared_memory = (int *)shmat(sharedMemID, NULL, 0);
            sharedMemArray = (int *)shared_memory;
            
            //each partial histogram given that it represents a count just gets added to the overall histogram
            for (int j = 0; j < bincount; j++){
                histogram[j] = sharedMemArray[j] + histogram[j];
            }
            
        }

    }
    
    for (int i = 0; i <N; i++){
        //make sure parent does not close prematurely
        pid = ids[i];
        wait(&pid);
    }
    
    //open the semaphore
    sem_t *blockingSemaphoreIdentifier = sem_open(semName, O_CREAT, 0600, 0);
    
    //close the semaphore since we finished working with it
    if (sem_close(blockingSemaphoreIdentifier) != 0) {
        perror("Parent  closes the sempahore but it failed\n");
        return -1;
    } 

    //detach from the sempahore and finish the project
    if( sem_unlink(semName) < 0 ){
        printf("Parent unlinking from the semaphore failed\n");
        return -1;
    }
    //open the output file and output the result
    FILE  *outputfile = fopen(argv[N+5], "w"); 
    writeHistogramToFile(outputfile,histogram,bincount );

    return 0;   
}


//initializes int array to a given value
void initializeArray (int *array, int size, int initializer){
    int i;
    for (i = 0; i < size; i++){
        array[i] = initializer;
    }
}

//reads a given file in C and populates histogram array
//each number tries to go in the appropriate bin
void getHistogram (FILE *file, int *histogram, double minvalue,double maxvalue, double width)
{
    //define the variables needed for the file read
    double currentNo;
    int index;
    char *line = NULL;
    size_t length = 0;
    ssize_t singleLine;

    // get the number of bins the histogram has.
    int bincount = (int) ((maxvalue - minvalue) /(double) width);

    //read file line by line
    while ((singleLine = getline(&line, &length, file)) != -1) 
    {
        //read the float number from file
        currentNo = atof(line);
        
        //checks if the number is in the given range and if it is, it will update the array content
        if ( currentNo > maxvalue || currentNo < minvalue){
            index = -1;
        } else {
            //deal only with the numbers that are within the given boundaries, ignore everything else 
            index = (int) ((currentNo - minvalue) / (double) width);
            if (index == bincount){
                index = bincount - 1;
            }
            histogram[index] += 1;
        }

     }

    //deallocate the memory
    free(line);
}


//given the size, the file and the array writes the histogram to a file 
void writeHistogramToFile(FILE *file, int *histogram, int size){
    int i;
    for (i = 0; i <  size; i++){
        fprintf(file, "%d:%d\n", i+1, histogram[i]);
    }
}

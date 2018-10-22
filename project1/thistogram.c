/*
 *
 * ######################################################
 * Date: 22/10/2018
 * @Author: Aldo Tali 21500097
 * Description: Solution to the first project given
 * in Cs342 Operating Systems Fall 2018-2019.
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
#include <pthread.h>
#include <sys/time.h>
#include <time.h> 

void getHistogram (FILE *file, int *histogram, double minvalue,double maxvalue, double width);
void writeHistogramToFile(FILE *file, int *histogram, int sizei,int intermediate);
int getIndex(double start, double end,  double width, double no);

//this structure will hold the stored values of each thread
struct thread_info {
    int     threadID;
    double  minvalue;
    double  maxvalue;
    char    *readfile;
    int     bincount;
    int     *info;
};

// this function executes the tas kof each thread. it will read a file of numbers
// and generate the corresponding histogram as needed. 
// output will be in the info parameter of the thread
void *childThread(void *threadStruct )
{
    //initialize the thread and cast it
    // prevents segmentation fault
    struct thread_info *thisThread;
    thisThread = (struct thread_info *) threadStruct;

    //varibales needed for histogram values computation
    double width;
    int i;
    int histogram[thisThread->bincount];
    FILE  *file = fopen(thisThread->readfile, "r");
    
    //calculate bin width
    width = (thisThread->maxvalue - thisThread->minvalue)/ (double) thisThread->bincount;
    
    //given the read file, read all the numbers and generate the needed histogram
    getHistogram (file, thisThread->info,thisThread->minvalue,thisThread->maxvalue,width);

    fclose(file);

   /* for (i = 0; i < thisThread->bincount; i++) {
            thisThread->info[i] =  histogram[i];
    }*/
  
}


int main(int argc, char*argv[])
{
    struct timeval start, end; 
    gettimeofday(&start, NULL);

  
    //prompt user if he does not enter the right number of arguments
    //user input is of the following format:
    //phistogram minvalue maxvalue bincount N file1 ... fileN outfile
    //Assuming the very least case N = 1 then we need 6 or more arguments
    if (argc < 7)
    {
        printf("Please enter the correct input information.\n");
        printf("It has the follow the follwoing format: \n ");
        printf("phistogram minvalue maxvalue bincount N file1 ... fileN outfile\n\n");
        exit(1);
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
        printf("Please enter the correct input information.\n");
        printf("It has the follow the follwoing format: \n");
        printf("phistogram minvalue maxvalue bincount N file1 ... fileN outfile\n\n");
        exit(1);
    }
    
    //variables for the thread creation storage
    int response;
    pthread_t threads[N];
    struct thread_info to[N];
    int histogram [N+1][bincount];

    //initializes the array of partial histograms
    for (i = 0; i <  N+1; i++)
    {
        for (int j = 0; j < bincount; j++){
            histogram[i][j] = 0;
        }
    }
    

    //run N threads
    for (i = 0; i < N; i++)
    {
        //printf("Thread No %d is created N:%d\n", i,N);
        
        //give each thread an id, a histogram to fill, min max, readfile name and
        // bin count parameters to knwo where to generate the histogram from
        to[i].threadID = i;
        to[i].info =  histogram[i] ;
        to[i].minvalue = minvalue;
        to[i].maxvalue = maxvalue;
        to[i].readfile = argv[5+i];
        to[i].bincount = bincount;
   
        response = pthread_create(&threads[i], NULL, childThread ,( void * ) &to[i] );
        
    } 
    
    
    //make sure you wait for each thread
    for (int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    //merge the threads
    int loop1, loop2;
    for ( loop1 = 0; loop1 < N;  loop1++){
        for (loop2 = 0; loop2 < bincount; loop2++){
            histogram[N][loop2] += to[loop1].info[loop2];
        }
    }  
    
    //write the merged result into the output file
    FILE  *outputfile = fopen(argv[N+5], "w"); 
    writeHistogramToFile(outputfile,histogram[N],bincount,0);
    fclose(outputfile);

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + 
              ((end.tv_usec - start.tv_usec)/1000000.0);
    printf("The elapsed time is: %f\n", elapsed);

    return(0);
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
        //this line is purely for debugging purposes
        //printf("%s", line);

        //read the float number from file
        currentNo = atof(line);

        //gets the index where the number is found in the histogram
        index = getIndex(minvalue,maxvalue,width,currentNo);

        //this line is purely for debugging purposes
        //printf (" Current index  %d and current no %f  ", index,currentNo);
        
        //deal only with the numbers that are within the given boundaries, ignore everything else
        if (index >= 0 )
        {
            if (index == bincount)
            {
                index = bincount - 1;
            }
            histogram[index] += 1;
        }
     }

    //deallocate the memory
    free(line);
}


//return the index of the histogram for the given number no
int getIndex(double start, double end,  double width, double no)
{
    //check only numebers in the bound [start:end]
    if (no < start || no > end)
    {
        return -1;
    }
    
    return (int) ((no - start) / (double) width);
}

//writes the histogram to a file 
//intermediate bit is only used for internal formating purposes, allowing to write the same
//code for part a and b.
void writeHistogramToFile(FILE *file, int *histogram, int size , int intermediateBit)
{
    int i;

    for (i = 0; i <  size; i++)
    {
        if (intermediateBit > 0) 
        {
            fprintf(file, "%d\n", histogram[i]);
        } else {
            fprintf(file, "%d:%d\n", i+1, histogram[i]);
        }
    }
}

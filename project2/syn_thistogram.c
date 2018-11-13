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
 * c Files can be legitemily used by both autors of this 
 * project.In particular the methods used from project 1 
 * are :
 *      writeHistogramToFile(FILE *file, int *histogram, int sizei,int intermediate);
 *      initializeArray (int *array, int size, int initializer)
 *      initializeDoubleArray (double *array, int size, double initializer)
 *      struct thread_info (modified)
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

void writeHistogramToFile(FILE *file, int *histogram, int sizei);

//define the global variables that need to be shared in between the threads.
struct histogramNode* head = NULL;
struct histogramNode* endList = NULL;
int numberOfThreads;
int finishedThreads = 0;
pthread_mutex_t linkedListAvailabilityLock;
pthread_mutex_t threadCountMutex = PTHREAD_MUTEX_INITIALIZER;

//this structure will hold the stored values of each thread
struct thread_info {
    int     threadID;
    char    *readfile;
    int     B;
    int     counter;
};

//Linked List node
struct histogramNode {
    double number;
    struct histogramNode *next;  
};

//use this method to initialize the head and the end of the linked list
void createLinkedList(struct histogramNode** head, struct histogramNode** end ){
    *head = NULL;
    *end = NULL;
}

//remove a node from the head of the linked list.
void removeNodeFromHead(struct histogramNode** head,struct histogramNode** end){
    //list is empty prevent seg. error
    if (*head == NULL){
        (*end) = NULL;
        return;
    } else {
        // keep poiner to free dynamic memory space
        struct histogramNode* temp = *head;
        *head = (*head)->next;

        // the list became empty update the endlist as well.
        if (*head == NULL){
            (*end) = NULL;
        }
        //deallocate memory
        free(temp);
    }
}

//adds a new node to the end of the list.
void addNodeToEnd(struct histogramNode** head,struct histogramNode** end, double num ){
    //create the new node
    struct histogramNode* newNode = (struct histogramNode*)malloc(sizeof(struct histogramNode));
    newNode->number = num;
    newNode->next = NULL;
    
    //if there is no endlist then start and end are the same
    if (*end == NULL){
        *head = newNode;
    } else {
        (*end)->next = newNode; //add the node to the end.
    }
    *end = newNode;
}

//used for testing purposes.
void printLinkedList(struct histogramNode** head){
    int counter = 0;
    struct histogramNode* iter = *head;
    if (iter == NULL){
        printf("Linked List is empty \n");
        return;
    }
    while(iter != NULL){
        counter++;
        printf("%lf\t",iter->number);
        iter = iter->next;
        if (counter % 4 == 0){
            printf("\n");
        } 
    }
}

//initializes int array to a given value
void initializeArray (int *array, int size, int initializer)
{
    int i;
    for (i = 0; i < size; i++)
    {
        array[i] = initializer;
    }
}
//initializes double array to a given value
void initializeDoubleArray (double *array, int size, double initializer)
{
    int i;
    for (i = 0; i < size; i++)
    {
        array[i] = initializer;
    }
}

//returns the lenght od he list
int getListLength(){
    int counter  = 0;
    struct histogramNode* iter = head;

    while (iter != NULL){
        counter++;
        iter = iter->next;
    }
    return counter;
}

// this function executes the tas kof each thread. it will read a file of numbers
// and generate the corresponding histogram as needed. 
// output will be in the info parameter of the thread
void *childThread(void *threadStruct )
{
    //initialize the thread and cast it
    // prevents segmentation fault
    struct thread_info *thisThread;
    thisThread = (struct thread_info *) threadStruct;

    //decalre the neede variabls
    double currentNo;
    double numbersArray[thisThread->B];
    int i,j;
    char *line = NULL;
    size_t length = 0;
    ssize_t singleLine;

    FILE  *file = fopen(thisThread->readfile, "r");

    //make sure the local array of read files in the thread is initialized to 0
    initializeDoubleArray(numbersArray,thisThread->B, 0);

    //read file line by line
    while ((singleLine = getline(&line, &length, file)) != -1) 
    {   
        //this line is purely for debugging purposes
        //printf("%s", line);

        //read the float number from file
        currentNo = atof(line);

        //update the array with each file read
        numbersArray[thisThread->counter] = currentNo;
        thisThread->counter ++;
        
        //when we have read B values in this thread then lock your global linked list read
        if (thisThread->counter == thisThread->B){
            //link list about to be updated so lock accordingly
            pthread_mutex_lock(&linkedListAvailabilityLock);
            thisThread->counter = 0;
            //pass all elements in the array to the linked list
            for (j = 0; j < thisThread->B; j++){
                addNodeToEnd(&head,&endList,numbersArray[j]);
            }
            //re-initialize the array to 0
            initializeDoubleArray(numbersArray,thisThread->B, 0);
            //printf("Thread nr %d took the lock and added B vaues\n", thisThread->threadID);
            //printLinkedList(&head);
            
            //release the lock
            pthread_mutex_unlock(&linkedListAvailabilityLock);
        }
     }


    //account for the last batch that is not of size B
    //NOTE: Think about putting this in a method;
    if (thisThread->counter >= 0){
        pthread_mutex_lock(&linkedListAvailabilityLock);
        for (j = 0; j < thisThread->counter; j++){
            addNodeToEnd(&head,&endList,numbersArray[j]);
        }
        //printf("Thread nr %d took the lock and added B vaues in last left values \n", thisThread->threadID);
        //printLinkedList(&head);
     }
    
    pthread_mutex_unlock(&linkedListAvailabilityLock);

    //going to update the number of finished threads so go in a lock to protect the change
    pthread_mutex_lock(&threadCountMutex);
    finishedThreads++;
    pthread_mutex_unlock(&threadCountMutex);
    free(line);   //deallocate the memory
  
}  

//runs on wrong executable run
void runErrorDisplay(){
    printf("We are really sorry but the information provided is not the correct one. \n Try to check the way you run the executable. It should have the following format: \nsyn_phistogram minvalue maxvalue bincount N file1 ... fileN outfile B\n\n");
    exit(1);
}

int main(int argc, char*argv[])
{
    
    struct timeval starttime, endtime; 
    gettimeofday(&starttime, NULL);
    
    //check for wrong user input
    if (argc < 8){
       runErrorDisplay();
    }

    //define the numbers gotten from the input
    double minvalue,maxvalue,width;
    int     bincount,N,i,B;

    //put the arguments in the variables for ease of use
    sscanf(argv[1],"%lf",&minvalue);
    sscanf(argv[2],"%lf",&maxvalue);
    sscanf(argv[3],"%d",&bincount);
    sscanf(argv[4],"%d",&N);
    sscanf(argv[6+N],"%d",&B);

    //prevents segmentation fault if for some reason there are not N files
    if (argc != (N + 7)){
        runErrorDisplay();
    }

    if (pthread_mutex_init(&linkedListAvailabilityLock, NULL) != 0)
    {
        printf("We couldn't even initialize the mutex :////\n");
        return -1;
    }

    //declare the threads array
    pthread_t threads[N];
    struct thread_info to[N]; //information kept that needs to be pased to each thread
    int histogram [bincount]; 
    
    numberOfThreads = N;
    initializeArray(histogram,bincount,0);

    //run N threads
    for (i = 0; i < N; i++){
        //update the thread information for the newly to be created thread
        to[i].threadID = i;
        to[i].readfile = argv[5+i];
        to[i].B = B;
        to[i].counter = 0;
    
        pthread_create(&threads[i], NULL, childThread ,( void * ) &to[i] );  
    }

    //delcare loop variables and computanionally needed variables
    int p = 0,index;
    width = (maxvalue-minvalue)/ (double) bincount;
    int loopConstraint = B;
    double elapsed;

    while(1){
        if (head != NULL ){
            //potentlially list read will start. Acquire the lock
            pthread_mutex_lock(&linkedListAvailabilityLock);
            loopConstraint = getListLength();

            //on cases where the batchsize was not B to account for the change read what is left not B elements
            if (loopConstraint%B == 0){
                loopConstraint = B;
            } else {
                loopConstraint = loopConstraint% B;
            }

            // read the number elements from the linked list and update the parents histogram
            for (p = 0; p < loopConstraint; p++){
                index = (int)((head->number - minvalue)/ (double) width); 
                if (index == bincount && ((int) head->number) == maxvalue){
                    index--;
                }
                histogram[index]++;
                removeNodeFromHead(&head, &endList); // for each node read just deallocate it
            }
            //printf("Main took the lock and read B values\n");
            //printLinkedList(&head);

            //release the lock
            pthread_mutex_unlock(&linkedListAvailabilityLock);
        } else {

           // potentially all threads finished but still some ndoes left to be read
           pthread_mutex_lock(&threadCountMutex);
           if (finishedThreads >= numberOfThreads){
               if (head == NULL) {
                    //write result to file
                    FILE  *outputfile = fopen(argv[N+5], "w"); 
                    writeHistogramToFile(outputfile,histogram,bincount);
                    fclose(outputfile);

                    //return elapsed time
                    gettimeofday(&endtime, NULL);
                    elapsed = (endtime.tv_sec - starttime.tv_sec) + 
                            ((endtime.tv_usec - starttime.tv_usec)/1000000.0);
                    printf("The elapsed time is: %f\n", elapsed);

                    exit(0);
               }    
           }
           pthread_mutex_unlock(&threadCountMutex);
        } 
    }
    
    pthread_mutex_destroy(&linkedListAvailabilityLock);

    return(0);
}


//given the size, the file and the array writes the histogram to a file 
void writeHistogramToFile(FILE *file, int *histogram, int size){
    int i;
    for (i = 0; i <  size; i++){
        fprintf(file, "%d:%d\n", i+1, histogram[i]);
    }
}

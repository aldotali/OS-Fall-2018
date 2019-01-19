/*
 *
 * ######################################################
 * Date: 29/12/2018
 * @Author: Aldo Tali 21500097
 * Description: Solution to the fourth project
 * in Cs342 Operating Systems Fall 2018-2019.
 * ######################################################
 *
 * */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int queueSize;

//structure to keep a disk queue of 5000 cylinders
struct DiskQueue {
    int firstIndex, endIndex,headPosition;
};
void initializeArray (int *array, int size, int initializer);
void getRequests (char *readfile, int *requests);
void runRandom(int noMoves[],int headPos);
void runFileRead(int noMoves[], int headPos,char* fileName);
void runForHundredTimes();
void handleSameRequests(int requests[], int size, int request);

int getIndexOfNextSST(int* requests, int sizeOfRequests, int head, int dir);
int getIndexOfNextSCAN(int* requests, int sizeOfRequests, int head, int dir);
int copyArray(int toCopy[], int whereToCopy[],int size );
int getRandomRequestArray(int requests[], int sizeOfRequests, int randomBound);
int runSCAN(int* requests, int sizeOfRequests, struct DiskQueue* disk);
int runFCFS(int* requests, int sizeOfRequests, struct DiskQueue* disk);
int runSSTF(int* requests, int sizeOfRequests, struct DiskQueue* disk);
int runCSCAN(int requests[], int sizeOfRequests, struct DiskQueue* disk);
int runCLOOK(int requests[], int sizeOfRequests, struct DiskQueue* disk);
int runLOOK(int requests[], int sizeOfRequests, struct DiskQueue* disk);

//creates the disk queue and initializes it
struct DiskQueue* createDiskQueue(int hpos){
    int i;
    struct DiskQueue* newQueue = (struct DiskQueue*) malloc(sizeof(struct DiskQueue));;
    
    newQueue->firstIndex = 0;
    newQueue->endIndex = queueSize -1;
    newQueue->headPosition = hpos;
    return newQueue;
}


int main(int argc, char*argv[]){

    //prompt the user if he does not enter the correct number of inputs
    //the program is called: diskschedule <headpos> <inputfile> 
    if (argc > 4)
    {
        printf("Please enter the correct input information.\n");
        printf("It has the follow the follwoing format: \n ");
        printf("diskschedule <headpos> <inputfile>\n\n");
        exit(1);
    }

    int headPosition;
    int noMoves[6];
    queueSize = 5000;
    srand ( time(NULL) );
    //the numebr of moves for all algorithms is 0 initially
    initializeArray (noMoves,6,0);
    if (argc == 1){    
        headPosition = rand()%queueSize;
        printf("My head is : %d\n", headPosition);
        runRandom(noMoves,headPosition);
    }else{
        headPosition =atoi(argv[1]);
        if (headPosition == -1){
            runForHundredTimes();
            exit(1);
        }
        if (argc == 4){
            queueSize = atoi(argv[3]);
        }
        //printf("The head = %d\n",headPosition);
        //check the case when the user only inserted the head possition 
        //random integer generation is supposed to happen here.
        if (argc == 2){
            runRandom(noMoves,headPosition);
        }else{
            runFileRead(noMoves,headPosition,argv[2]);
        }
    }

    return 0;
}

void runForHundredTimes(){
    int requests[5000];
    int clone[5000];

    int mov = -27;
    int noReq = 5000;
    int headPos,i = 0;
    struct DiskQueue* disk; 

    for (i = 0; i < 100; i++){
        mov = -27;
        headPos = rand()%queueSize;
        disk = createDiskQueue(headPos);
        initializeArray (requests,noReq,-27);
        getRandomRequestArray(requests,noReq,queueSize);

        copyArray(requests, clone,noReq);
        mov = runFCFS(clone,noReq,disk);
        printf("%d,",mov);

        copyArray(requests, clone,noReq);
        mov = runSSTF(clone,noReq,disk);
        printf("%d,",mov);

        copyArray(requests, clone,noReq);
        mov = runSCAN(clone,noReq,disk);
        printf("%d,",mov);

        copyArray(requests, clone,noReq);
        mov = runCSCAN(clone,noReq,disk);
        printf("%d,",mov);

        copyArray(requests, clone,noReq);
        mov = runLOOK(clone,noReq,disk);
        printf("%d,",mov);;

        copyArray(requests, clone,noReq);
        mov = runCLOOK(clone,noReq,disk);
        printf("%d",mov);
        printf("\n");
    }
}
//runs the algorithms with the requests generated from the fileRead
void runFileRead(int noMoves[], int headPos,char* fileName){
    int requests[5000];
    int clone[5000];

    int mov = -27;
    int noReq = 5000;
    struct DiskQueue* disk = createDiskQueue(headPos);
    initializeArray (requests,noReq,-27);
    getRequests(fileName, requests);

    copyArray(requests, clone,noReq);
    mov = runFCFS(clone,noReq,disk);
    printf("FCFS: %d\n",mov);
    noMoves[0] = mov;

    copyArray(requests, clone,noReq);
    mov = runSSTF(clone,noReq,disk);
    printf("SSTF: %d\n",mov);
    noMoves[1] = mov;

    copyArray(requests, clone,noReq);
    mov = runSCAN(clone,noReq,disk);
    printf("SCAN: %d\n",mov);
    noMoves[2] = mov;

    copyArray(requests, clone,noReq);
    mov = runCSCAN(clone,noReq,disk);
    printf("C-SCAN: %d\n",mov);
    noMoves[3] = mov;

    copyArray(requests, clone,noReq);
    mov = runLOOK(clone,noReq,disk);
    printf("LOOK: %d\n",mov);
    noMoves[4] = mov;

    copyArray(requests, clone,noReq);
    mov = runCLOOK(clone,noReq,disk);
    printf("C-LOOK: %d\n",mov);
    noMoves[5] = mov;  
}

//runs the algorithms with random integer generation
void runRandom(int noMoves[],int headPos){
    int requests[5000];
    int clone[5000];

    int mov = -27;
    int noReq = 5000;

    struct DiskQueue* disk = createDiskQueue(headPos);
    initializeArray (requests,noReq,-27);
    getRandomRequestArray(requests,noReq,queueSize);
    copyArray(requests, clone,noReq);
    mov = runFCFS(clone,noReq,disk);
    printf("FCFS: %d\n",mov);
    noMoves[0] = mov;

    copyArray(requests, clone,noReq);
    mov = runSSTF(clone,noReq,disk);
    printf("SSTF: %d\n",mov);
    noMoves[1] = mov;

    copyArray(requests, clone,noReq);
    mov = runSCAN(clone,noReq,disk);
    printf("SCAN: %d\n",mov);
    noMoves[2] = mov;

    copyArray(requests, clone,noReq);
    mov = runCSCAN(clone,noReq,disk);
    printf("C-SCAN: %d\n",mov);
    noMoves[3] = mov;

    copyArray(requests, clone,noReq);
    mov = runLOOK(clone,noReq,disk);
    printf("LOOK: %d\n",mov);
    noMoves[4] = mov;

    copyArray(requests, clone,noReq);
    mov = runCLOOK(clone,noReq,disk);
    printf("C-LOOK: %d\n",mov);
    noMoves[5] = mov;
}

//reads a file of requests and populates the requests array
void getRequests (char *readfile, int *requests)
{
    FILE  *file = fopen(readfile, "r");
    //define the variables needed for the file read
    double currentNo;
    int index;
    char *line = NULL;
    char splitter[] = " ";
    char *ptr;
    int requestNo = 0;
    int cylinderNo = 0;
    size_t length = 0;
    ssize_t singleLine;


    //read file line by line
    while ((singleLine = getline(&line, &length, file)) != -1) 
    {
        //this line is purely for debugging purposes
        //printf("%s", line);
        
        //get the request number
        ptr = strtok(line, splitter);
        requestNo = atoi(ptr);
        //get the cylinder number
        ptr = strtok(NULL, splitter);
        cylinderNo = atoi(ptr);
        requests[requestNo-1] = cylinderNo;    
    
    }
    int i;
    /*for (i= 0; i<10; i++){
        printf("\t%d",requests[i]);
    }*/
    //deallocate the memory
    free(line);
    fclose(file);
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

//runs the FCFS alfgorithm given the array of requests to be made to disk
int runFCFS(int* requests, int sizeOfRequests, struct DiskQueue* disk){
    int i;
    int currentHead  = disk->headPosition;
    int noMovements = 0; 
    int additional = 0;

    for (i = 0; i < sizeOfRequests; i++){
        if (requests[i] == -27){}
        else{
            if (requests[i] > disk->endIndex){
                noMovements = noMovements + disk->endIndex - currentHead; 
                currentHead = disk->endIndex;
            } else {
            
                if (requests[i] < 0){
                    noMovements = noMovements + currentHead;
                    currentHead = 0; 
                } else {
                    additional = requests[i] - currentHead;
                    if (additional < 0){
                        additional = additional * (-1);
                    }
                    noMovements = noMovements + additional;
                    currentHead = requests[i];
                    handleSameRequests(requests, sizeOfRequests,requests[i]);
                }
            }
        }
        //printf("[%d]\t[%d]\t[%d]\n",noMovements,currentHead,requests[i]);
    }
    return noMovements;
}

//runs the SST alfgorithm given the array of requests to be made to disk
int runSSTF(int* requests, int sizeOfRequests, struct DiskQueue* disk){
    int i;
    int currentHead  = disk->headPosition;
    int noMovements = 0; 
    int additional = 0;
    int sstIndex = 0;

    for (i = 0; i < sizeOfRequests; i++){
       
            //printf("%d\n", requests[currentHead]);
            sstIndex = getIndexOfNextSST(requests,sizeOfRequests,currentHead,1);
            
            //printf("the next index is [%d] \n", sstIndex );
            if (sstIndex >=0){
                additional = requests[sstIndex] - currentHead;
                if (additional < 0){
                    additional = additional * (-1);
                }
                noMovements = noMovements + additional;
            }
            
            currentHead = requests[sstIndex];
            requests[sstIndex] = -27;
            
    }
    return noMovements;
}

//runs the SCAN algorithm given the array of requests to be made to the disk
int runSCAN(int* requests, int sizeOfRequests, struct DiskQueue* disk){
    int i;
    int currentHead  = disk->headPosition;
    int noMovements = 0; 
    int index = -27;
    int addition;

    //run to the right;
    for (i = 0; i < sizeOfRequests; i++){
        index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,1);
        
        if (index < 0){
            i = sizeOfRequests;
        } else {
            //printf("index [%d]\n", requests[index]  );
            //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
            noMovements = noMovements + requests[index] - currentHead;
            currentHead = requests[index];
            requests[index] = -27;
        }
    }
    noMovements = noMovements + disk->endIndex-currentHead;
    currentHead = disk->endIndex;
    //run to the left;
    for (i = 0; i < sizeOfRequests; i++){
        index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,0);
         //printf("index [%d]\n", requests[index]  );
            //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
        if (index < 0){
            i = sizeOfRequests;
        } else {
            noMovements = noMovements + currentHead - requests[index];
            currentHead = requests[index];
            requests[index] = -27;
        }
        
    }
    return noMovements;
}

//runs the C-SCAN algorithm give the array of requests to be made to the disk
int runCSCAN(int requests[], int sizeOfRequests, struct DiskQueue* disk){
    int i;
    int currentHead  = disk->headPosition;
    int noMovements = 0; 
    int index = -27;
    int addition;
    //printf("My head = %d\n", currentHead);
    //run to the right;
    for (i = 0; i < sizeOfRequests; i++){
        index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,1);
        
        if (index < 0){
            i = sizeOfRequests;
        } else {
            //printf("index [%d]\n", requests[index]  );
            //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
            noMovements = noMovements + requests[index] - currentHead;
            currentHead = requests[index];
            requests[index] = -27;
            //printf("[%d]\t[%d]\t[%d]\n",noMovements,currentHead,requests[i]);
        }
    }

    noMovements = noMovements + disk->endIndex-currentHead +  disk->endIndex;
    currentHead = 0;
    //printf("[%d]\t[%d]\t[%d]\n",noMovements,currentHead,-5);
    //run to the left;
    for (i = 0; i < sizeOfRequests; i++){
        index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,1);
        
        if (index < 0){
            i = sizeOfRequests;
        } else {
            //printf("index [%d]\n", requests[index]  );
            //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
            noMovements = noMovements + requests[index] - currentHead;
            currentHead = requests[index];
            requests[index] = -27;
            //printf("[%d]\t[%d]\t[%d]\n",noMovements,currentHead,requests[i]);
        }
    }
    return noMovements;
}

//runs the C-LOOK algorithm give the array of requests to be made to the disk
int runCLOOK(int requests[], int sizeOfRequests, struct DiskQueue* disk){
    int i;
    int currentHead  = disk->headPosition;
    int noMovements = 0; 
    int index = -27;
    int addition;

    //run to the right;
    for (i = 0; i < sizeOfRequests; i++){
        index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,1);
        
        if (index < 0){
            i = sizeOfRequests;
        } else {
            //printf("index [%d]\n", requests[index]  );
            //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
            noMovements = noMovements + requests[index] - currentHead;
            currentHead = requests[index];
            requests[index] = -27;
        }
    }

    int nextMove = -1;
    int min = 6000;
    for (i = 0; i < sizeOfRequests; i++){
        if (requests[i] < min && requests[i] >= 0){
            min = requests[i];
        }
    }

    if (min < 6000){
        noMovements = noMovements + currentHead-min;
        currentHead = min;

            //run to the right;
        for (i = 0; i < sizeOfRequests; i++){
            index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,1);
            
            if (index < 0){
                i = sizeOfRequests;
            } else {
                //printf("index [%d]\n", requests[index]  );
                //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
                noMovements = noMovements + requests[index] - currentHead;
                currentHead = requests[index];
                requests[index] = -27;
            }
        }
    }

    return noMovements;
}

//runs the LOOK algorithm give the array of requests to be made to the disk
int runLOOK(int requests[], int sizeOfRequests, struct DiskQueue* disk){
    int i;
    int currentHead  = disk->headPosition;
    int noMovements = 0; 
    int index = -27;
    int addition;

    //run to the right;
    for (i = 0; i < sizeOfRequests; i++){
        index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,1);
        
        if (index < 0){
            i = sizeOfRequests;
        } else {
            //printf("index [%d]\n", requests[index]  );
            //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
            noMovements = noMovements + requests[index] - currentHead;
            currentHead = requests[index];
            requests[index] = -27;
        }
    }

    int nextMove = -1;
    int max = -1;
    for (i = 0; i < sizeOfRequests; i++){
        if (requests[i] > max && requests[i] >= 0){
            max = requests[i];
        }
    }

    if (max > -1){
        noMovements = noMovements + currentHead-max;
        currentHead = max;

            //run to the right;
        for (i = 0; i < sizeOfRequests; i++){
            index = getIndexOfNextSCAN(requests,sizeOfRequests,currentHead,0);
            
            if (index < 0){
                i = sizeOfRequests;
            } else {
                //printf("index [%d]\n", requests[index]  );
                //printf("the head [%d] adn the disk [%d]\n", requests[index], currentHead);
                noMovements = noMovements - requests[index] + currentHead;
                currentHead = requests[index];
                requests[index] = -27;
            }
        }
    }

    return noMovements;
}

//returns the next shortests seek time cylinder to be read
int getIndexOfNextSST(int requests[], int sizeOfRequests, int head, int dir){
    /*int readFromLeft = 6000;
    int readFromRight = 6000;

    int minLeft = -1;
    int minRight = -1;*/
    int distanceFromHead = -1;
    int min = 6000;
    int index = -1;
    int i = 0;
    //printf("The distance from head [%d] is [%d], %d\n", head, distanceFromHead,requests[i]);

    for (i = 0; i < sizeOfRequests; i++){

        if (requests[i] == -27 ){} 
        else {
            distanceFromHead = head - requests[i];
            if (distanceFromHead < 0 ){
                distanceFromHead = distanceFromHead *(-1); 
            }
            if (distanceFromHead <= min){

                min = distanceFromHead;
                index = i;
            }
            /* //looks for left min
                if (requests[i] < head){
                    if (head - requests[i] < readFromLeft){
                        readFromLeft = requests[i];
                        minLeft = i;
                    }
                } else {
                    //loks fro right min
                }*/
        }
    }
    return index;
}

//returns the next cylinder
int getIndexOfNextSCAN(int requests[], int sizeOfRequests, int head, int dir){
    int min = 6000;
    int distanceFromHead = 6000;
    int index = -1;
    int i;

    for (i = 0; i < sizeOfRequests; i++){

        if (requests[i] == -27 ){} 
        else {
            distanceFromHead = head - requests[i];
            if (distanceFromHead < 0 ){
                distanceFromHead = distanceFromHead *(-1); 
            }

            if (distanceFromHead < min){
                if (dir == 1 && requests[i] >= head){
                    min = distanceFromHead;
                    index = i;
                }
                if (dir == 0 && requests[i] <= head){
                    min = distanceFromHead;
                    index = i;
                }
            }

        }
    }
    return index;
}

//generates an array of random disk requests 
int getRandomRequestArray(int requests[], int sizeOfRequests, int randomBound){
    int i;
    int randomNum;
    
    
    for (i = 0; i < sizeOfRequests; i++){
        randomNum = rand()%randomBound;
        requests[i] = randomNum;
        //printf("%d\n", randomNum);
    }
}

//copies an array of integers
int copyArray(int toCopy[], int whereToCopy[],int size ){
    int i ;
    for (i = 0; i<size; i++){
        whereToCopy[i] = toCopy[i];
    }
}

//handels all requests of the same number
void handleSameRequests(int requests[], int size, int request){
    int i;

    for (i = 0; i<size; i++){
        if (requests[i] == request){
            requests[i] = -27;
        }
    }
}
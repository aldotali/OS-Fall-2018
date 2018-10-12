/*
 *
 * ######################################################
 * Date: 05/10/2018
 * @Author: Aldo Tali 21500097
 * Description: Solution to part 5 of the first 
 *  homework in Cs342 Operating Systems Fall 2018-2019.
 * ######################################################
 *
 * */



#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h> 

//The following are the function routine signatures that simulate the function calls with different parameters
//
//simulates the file creation to write files with different sizes
void createFileWithSizeB(int size, FILE *file);
//simulates the file reading of files with different sizes
void checkReadFileSizeB(int size);
//simulates the opening time of files with different sizes
void checkOpenCommand();
//simulates the delete time of files with different sizes
void checkRemoveCommand();
//simualtes the time of make directory command
void checkMkdirCommand();
void displayPid();

//writes file to a given size
void createFileWithSizeB(int size, FILE *file)
{
    // int pid = getpid();
    //printf("The PID in Routine createFile with fileSize %d bytes is %d\n",size,pid);
    
    //keep the current time
    struct timeval  tv;
    gettimeofday(&tv,NULL);
    
    //write enough to fill the file up to a given file size
    fseek(file,size-1,SEEK_SET);
    fputc('\0', file);
    fclose(file);

    // get the finish time
    struct timeval  tv2;
    gettimeofday(&tv2,NULL);
    
    //get numerical values of the times and print them out
    long long startTime = tv.tv_sec*10000000 + tv.tv_usec;
    long long endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    
    printf("The Time needed to create %d bytes, was %lld microseconds.\n",size, (endTime-startTime));

}

//reads file of a given size of bytes
//CRUCIAL: Be careful it makes use of the files created on the previous function
void checkReadFileSizeB(int size)
{
    //int pid = getpid();
    //printf("The PID in Routine checkRead with file size %d bytes is %d\n",size,pid);
    
    //use this for the file read 
    char *buffer =  (char *) calloc(size, sizeof(char));
    char str[50];
    
    sprintf(str, "%d", size); 

    //record the beggining time 
    struct timeval  tv;
    gettimeofday(&tv,NULL);
    
    int fp = open(strcat(str, "bytes"), O_RDONLY);
    int bytes = read(fp,buffer,size); 
     
    close(fp);
    struct timeval  tv2;
    gettimeofday(&tv2,NULL);
    
    //display tun time
    long long startTime = tv.tv_sec*10000000 + tv.tv_usec;
    long long endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    
    printf("The Time needed to read %d bytes, was %lld microseconds.\n",bytes, (endTime-startTime));

}

//simulates the open command of linux
void checkOpenCommand()
{
    //record begining time
    struct timeval  tv,tv2;
    gettimeofday(&tv,NULL);
    long long startTime = tv.tv_sec*10000000 + tv.tv_usec;
    
    //opem the file and print the time of read
    printf("Opening File Read Only Mode\n");
    int description = open("checkOpenReadOnly",O_RDONLY | O_CREAT);
    gettimeofday(&tv2,NULL); 
    long long endTime = tv2.tv_sec*10000000 + tv2.tv_usec; 
    printf("The Time elapsed to open the files was : %lld microseconds\n", endTime - startTime);


    //repeat same process for different modes
    gettimeofday(&tv,NULL);
    startTime = tv.tv_sec*10000000 + tv.tv_usec;
    printf("Opening File Write Only Mode\n");
    description = open("checkOpenWriteOnly",O_WRONLY | O_CREAT);
    gettimeofday(&tv2,NULL); 
    endTime = tv2.tv_sec*10000000 + tv2.tv_usec; 
    printf("The Time elapsed to open the files was : %lld microseconds\n", endTime - startTime);

    gettimeofday(&tv,NULL);
    startTime = tv.tv_sec*10000000 + tv.tv_usec;
    printf("Opening File Read and Write  Mode\n");
    description = open("checkOpenReadWrite", O_RDWR | O_CREAT);
    gettimeofday(&tv2,NULL); 
    endTime = tv2.tv_sec*10000000 + tv2.tv_usec; 
    printf("The Time elapsed to open the files was : %lld microseconds\n", endTime - startTime);

    gettimeofday(&tv,NULL);
    startTime = tv.tv_sec*10000000 + tv.tv_usec;
    printf("Opening File Read and Write  Mode by erasing all inital info \n");
    description = open("checkOpenReadWriteWithTruncate",O_RDWR | O_TRUNC | O_CREAT);
    
    gettimeofday(&tv2,NULL); 
    endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    
    printf("The Time elapsed to open the files was : %lld microseconds\n", endTime - startTime);

}

//simulates the timings for the remove/delete comand
void checkRemoveCommand()
{
    //record beginign time
    struct timeval  tv,tv2;
    gettimeofday(&tv,NULL);
    long long startTime = tv.tv_sec*10000000 + tv.tv_usec;
    
    //remove the files created before on the read modes
    printf("Deleting File Read Only Mode\n");
    remove("checkOpenReadOnly");
    printf("Deleting File Write Only Mode\n");
    remove("checkOpenWriteOnly");
    printf("Deleting File Read and Write  Mode\n");
    remove("checkOpenReadWrite");
    printf("Deleteing File Read and Write  Mode by erasing all inital info \n");
    remove("checkOpenReadWriteWithTruncate");
    
    gettimeofday(&tv2,NULL); 
    long long endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    
    printf("The Time elapsed to remove the files was : %lld microseconds\n", endTime - startTime);
    
    //check timings for removing each of the 100, 1000 or 10000 bytes files
    gettimeofday(&tv,NULL);
    startTime = tv.tv_sec*10000000 + tv.tv_usec;
    printf("Deleting 100000 bytes file \n");
    remove("100000bytes");
    gettimeofday(&tv2,NULL);
    endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    printf("The Time elapsed to remove the files was : %lld microseconds \n", endTime - startTime);
    
    gettimeofday(&tv,NULL);
    startTime = tv.tv_sec*10000000 + tv.tv_usec;
    printf("Deleting 10000 bytes file \n");
    remove("10000bytes");
    gettimeofday(&tv2,NULL);
    endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    printf("The Time elapsed to remove the files was : %lld microseconds \n", endTime - startTime);
    
    gettimeofday(&tv,NULL);
    startTime = tv.tv_sec*10000000 + tv.tv_usec;
    printf("Deleting 1000 bytes file \n");
    remove("1000bytes");
    gettimeofday(&tv2,NULL);
    endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    printf("The Time elapsed to remove the files was : %lld microseconds\n", endTime - startTime);
    
    gettimeofday(&tv,NULL);
    startTime = tv.tv_sec*10000000 + tv.tv_usec;
    printf("Deleting 100 bytes file \n");
    remove("100bytes");
    gettimeofday(&tv2,NULL);
    endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    printf("The Time elapsed to remove the files was : %lld microseconds\n", endTime - startTime); 

}

//simulates the time requirements for the make directory command
void checkMkdirCommand()
{
    //keep the begin time
    struct timeval  tv,tv2;
    gettimeofday(&tv,NULL);
    long long startTime = tv.tv_sec*10000000 + tv.tv_usec;
    
    //create directory
    printf("Creating Directory \n");
    int description = mkdir("ex5", 0777);
    //record end time
    gettimeofday(&tv2,NULL);
    long long endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    
    printf("The Time elapsed to create the directory was : %lld microseconds \n", endTime - startTime);
    rmdir("ex5");

}

void displayPid()
{
    int pid = getpid();
    printf("The PID in Routine is %d\n",pid);
    
    //keep the current time
    struct timeval  tv;
    gettimeofday(&tv,NULL);

    // get the finish time
    struct timeval  tv2;
    gettimeofday(&tv2,NULL);
    
    //get numerical values of the times and print them out
    long long startTime = tv.tv_sec*10000000 + tv.tv_usec;
    long long endTime = tv2.tv_sec*10000000 + tv2.tv_usec;
    
    printf("The Time needed to get pid, was %lld microseconds.\n", (endTime-startTime));

}

//run the tester
int main()
{
    //4 files needed to serve for the different parameters
    FILE *file = fopen("100000bytes", "w");
    FILE *file2 = fopen("10000bytes", "w");
    FILE *file3 = fopen("1000bytes", "w");
    FILE *file4 = fopen("100bytes", "w");

    // create the files with different sizes and record their timings accordingly
    printf("------------------------------------------------------------------------------------------\n");
    createFileWithSizeB(100000, file);
    createFileWithSizeB(10000, file2);
    createFileWithSizeB(1000, file3);
    createFileWithSizeB(100, file4);
    printf("------------------------------------------------------------------------------------------\n");
    
    // read the files with different sizes and record their timings accordingly
    printf("\n\n------------------------------------------------------------------------------------------\n");
    checkReadFileSizeB(100000);
    checkReadFileSizeB(10000);
    checkReadFileSizeB(1000);
    checkReadFileSizeB(100);
    printf("------------------------------------------------------------------------------------------\n");

    
    // open the files with different parameters and record their timings accordingly
    printf("\n\n------------------------------------------------------------------------------------------\n");
    checkOpenCommand(); 
    printf("------------------------------------------------------------------------------------------\n");
   
    // remove the files with different sizes and record their timings accordingly
    printf("\n\n------------------------------------------------------------------------------------------\n");
    checkRemoveCommand(); 
    printf("------------------------------------------------------------------------------------------\n");

    // check the make directory timing accordingly
    printf("\n\n------------------------------------------------------------------------------------------\n");
    checkMkdirCommand(); 
    printf("------------------------------------------------------------------------------------------\n");


    printf("\n\n------------------------------------------------------------------------------------------\n");
    displayPid();
    printf("------------------------------------------------------------------------------------------\n");



    
    return 0;
}

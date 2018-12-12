#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include<stdlib.h>
#include <time.h> 

void mypause ( void ) 
{ 
  printf ( "Press [Enter] to continue . . ." );
  //fflush ( stdout );
  //getchar();

  clock_t init_timer = clock(); 
  while (clock() < init_timer + 10000) ;  
} 


int main()
{
    int i, *ptr, sum = 0;
    int MAX_ALLOC = 10000;

    printf("We have started the app functioning!\n");
    printf("Press any key to allocate my memory\n");  
    mypause();

    ptr = (int*) malloc(MAX_ALLOC * sizeof(int));
    if(ptr == NULL)                     
    {
        printf("We hit some boundaries no further memory allocation");
        exit(-1);
    }

    for(i = 0; i < MAX_ALLOC; ++i)
    {
        scanf("%d", ptr + i);
        ptr[i] += i;
    }


    printf("We have allocated the memory!\n");
    printf("Press any key deallocate the memory\n");
    mypause();
    free(ptr);


    printf("We have deallocated the memory!\n");
    printf("Press any key exit\n");


    return 0;
}

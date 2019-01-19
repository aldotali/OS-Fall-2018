This implementation of the code takes at most 4 input parameters. 

The several ways to run it are the following:

1-> ./diskschedule
The above will jsut run a random case with disk size 5000;
2->./diskschedule <headpos>
The above will just run a random case with head position set to headpos
3->./diskschedule -1
This will run the algorithms with random numbers for 100 times.
4->./diskschedule <headpos> <inputfile> 
This reads the requests from the input file and runs the algorithms
5->./diskschedule <headpos> <inputfile> queueSize
This does the same as part4 but now it will set the disk size to queueSize. 
queueSize must be less than 5000 and this is only for debugging purposes.
# POSIX-2-Level-Multithreading

### This program accomplishes the following tasks   
0.  The program generates 10,000 random integers.
1.  The program finds the lowest and maximum values using a single thread and prints the values.
2.  Given the parameter -printInput, the program prints the 10,000 random integers to file.
3.  The main process creates 5 threads and each is responsible for processing 2,000 of those random integers.
4.  Each of the 5 threads creates 20 child threads. Each of 20 child threads finds the lowest value in 100 random integers.
5.  This will find the 100 lowest values, 20 in each parent thread.
6.  Each parent thread finds the lowest of its 20 integers, so there are 5 lowest values left.
7.  The lowest 20 values for each child thread are printed.
8.  The lowest 5 values for the parent threads are printed.
9.  The lowest value out of lowest 5 is printed.
10. The main process scans the 5 lowest values to find the lowest value.
11. All output is printed to file vagner-output.txt.  

---   
 
 NOTE: User can pass parameter *-printInput* to see all generated integers printed to file.   
 
---   

### COMPILATION INSTRUCTIONS
```c
gcc -c vagner.c                  // compiles the file to object file.
gcc -o app.exe vagner.o          // links object file to executable.
OPTION 1: ./app.exe              // runs the program without printing the random integers.
OPTION 2: ./app.exe -printInput  // runs the program printing the random input integers.
OPTION 3: ./app.exe -help        // runs the program and prints this message to console and vagner-output.txt file.
```

**Vagner Machado**
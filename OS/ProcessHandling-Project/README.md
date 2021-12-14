<h1> ProcessHandling Project </h1>

In this project, we've implemented a simple map-reduce structure, to divide and assign the following task into different processes. These processes use unnamed pipe to
communicate with their father, and named pipe to communicate with eachother.

Task: Given a number of huge text files, you are to calculate the number of each different word in all files.

A sample output for the *testcases* folder is also provided. 

<h2> to Run </h2>
In a terminal window simply run "make" and then "./main.out". You will notice that a number of named pipes will be generated along with the answer
which is stored in "output.csv" file. You can delete the unnecessary files in the end with "make clean" commmand.

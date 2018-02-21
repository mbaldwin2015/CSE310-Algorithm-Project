A C++ program written for my Data Structures and Algorithms class to sort a list of predefined data using QuickSort and BubbleSort, to test the speed of both algorithms versus each other.

The program doesn't take input from the user but from file redirection, so to run it: browse to the Debug directory in command prompt and run for example:

```
AlgorithmsProject1.exe < 1.txt > output.txt
```

The output of the sorting of the data will be stored in output.txt.  I made a couple of different tests (1.txt, 2.txt, and 3.txt) but you can edit the commands at the bottom of the text files to customize the searchs/sorts if you wish.

Here are some example commands found at the bottom of the test input text files, preceded by the total number of commands to process:

```
3
bsort 2015 penalties decr
qfind 2015 top_per_game min
qsort range 2014 2015 penalties incr
```

Looking at it now 2 years later I realize it needs major refactoring, there is a lot of repeated code but also sections could be split into separate files for ease of access.

I also remember not finding out about template functions until I was already half way done with the program and not having the time to refactor what I had already done - you can see this in the way the "bSort" functions work versus the "quickSort" functions.  After I wrote all of the "bSort" functions I thought there had to be a better way to pass multiple types to the same function, and after some googling that's when I learned about template functions.
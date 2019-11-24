CSC254 A5 Concurrency

Collaborators

    Gao Fan (gfan@u.rochester.edu)
    Jianyuan Zhong (jzhong9@u.rochester.edu)

How to run

    javac Dining.java
    java Dining > output_file
    python3 check_output.py output_file

Implementation

    Inspired by our workshop TA, we did the followings: 
    - Added a boolean to each fork indicating its current availability so that each fork can only be acquired by one philosopher at a time. 
    - Assigned forks to the last philosopher in the reverse order to prevent deadlock. 

    With those two implementations, after a 10-minute test, we have the following result: 
    
    Correctness check passed!
    Fairness breakdown:
                    Think	Wait	Eat
    Philosopher 1:	41.7%	33.2%	25.1%
    Philosopher 2:	40.3%	35.1%	24.6%
    Philosopher 3:	41.3%	33.8%	24.9%
    Philosopher 4:	40.9%	33.6%	25.6%
    Philosopher 5:	39.8%	34.8%	25.4%

    It showed that over the long run, all philosphers get to eat equally often. Therefore we did not implement additional mechanism for optimization. 

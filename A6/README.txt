CSC254 A6 Templates

Collaborators

    Gao Fan (gfan@u.rochester.edu)
    Jianyuan Zhong (jzhong9@u.rochester.edu)

How to run

    g++ -o main.out main.cc
    ./main.out

Implementation

    We have our additional test cases in main.cc. 
    As for the time complexity check given on Piazza, we got decent graphs for both carray and hashed sets but not for bin_search sets. We thought the reason might be that our + for bin_search sets doesn't work well on the extremely large size in the test case. Or there might be too many CSC254 students running the same time complexity check on cycle machine so that it took much longer time to get the result. 

    We didn't use the given hash function for our hashed sets. But use hash = F(item) % size instead, since with this hash function we don't need to consider the case that F(item) should be a nonzero value. As for rehash, we simply add 1 to the original index until there is an available space, or it turns out that current set if full. 

    We followed the std_range_set to implement both carray_range_set and hashed_range_set in a way that they convert the range to single T items and use the methods in their parent classes to achieve the operations. 

    We added additional methods in range class for bin_search_range_set. 
    "overlaps" checks if two ranges has overlap. 
    "merge" merges the existing range with the given range. 
    "split" splits the existing range with the given range. It returns an array of two ranges. When the given range actually covers the existing range in a way that we cannot not split it into two ranges, we return the ranges that has lower bound greater than the higher bound as a represenation of "NULL" range, since using "NULL" directly will cause problem when getting the data from the array. 

    Our implementation of bin_search_range_set, especially for +, is not optimized in our perspective. But we couldn't finish the better version before the ddl. 
    In +, we firstly check if r can be directly added at the end of the array. If not, we check if r precedes any existing ranges. If yes, we check if r overlaps with any ranges from the checkpoint. Otherwise, we check the overlap from the beginning. Then we merge all the overlapped ranges with r and add r back to the array. Then, we clean up the original array and return. We thought that it is possible to check "precede" and "overlap" at the same time because once r overlaps with one of the ranges, it cannot precedes other ranges. 
    In "-", we simply check r with all existing ranges and split the overlapped ranges with our "split" method. 
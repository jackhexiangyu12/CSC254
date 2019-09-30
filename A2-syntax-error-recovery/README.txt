CSC254 A2 syntax tree recovery

Collaborators

    Gao Fan (gfan@u.rochester.edu)
    Jianyuan Zhong (jzhong9@u.rochester.edu)

Build and Run

    cd to directory
    make
    make test NAME_OF_TEST_FILE
    make clean

    Replace NAME_OF_TEST_FILE with the full name of the test file. For example, "make test sample.txt". 

Implementation

    For error recovery machanism of Niklaus Wirth, we refered to the example 2.45 and implemented the basic version, which is required for CSC254 students. 
    For syntax tree, we output the tree in inear, parenthesized form. And the use of "[]" and "()" follows the sample output on website. 

    For tree decoration, we added a stack additional to the existing recursive functions to perform a combination of DFS and BFS to generate the correct output without an explicit tree structure. 
    For productions containing T TT or F FT, we push the left subtree to the stack and pop it out until we compute the right subtree. 

Pros

    Makefile is modified for a better test mechanism. 

Cons

    Due to the missing explicit tree structure, it is hard to implement extra credit 2, 3 and 4. 
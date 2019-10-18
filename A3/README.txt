CSC254 A3 interpretation

Collaborators

    Gao Fan (gfan@u.rochester.edu)
    Jianyuan Zhong (jzhong9@u.rochester.edu)

Build and run

    Compile
        make
    
    Test with given example
        ./interpreter example.calc example.input

    Test with given autograder
        bash autograder.bash interpreter.ml

Extra credit

    We implemented the checking mechanism for unused variable. 
    Uncomment line 652, which is "print_string (warn_mem mem ^ "\n");" in func interpret, to make it work. 

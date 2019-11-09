CSC254 A4 Cross-Indexing

Collaborators

    Gao Fan (gfan@u.rochester.edu)
    Jianyuan Zhong (jzhong9@u.rochester.edu)

How to run

    ruby xref.rb <name of executable file>

    For example, if the executable file is "a.out", then run "ruby xref.rb a.out"
    And the result is stored as "index.html"

Features

    All requirements are implemented. 

    Executables compiled with "-O0" or "-O3" are both supported. 
    Location-specific link to the beginning of the code for "main" is displayed on top of the html page called "main". 

Extra credis

    “Syntax-color” such that different part of the code on the page are visibly distinguished is implemented. 

Implementation

    Ruby is selected because it is easy to read and write. 

    Hash map is heavily used in this proj to create references between the assembly address and the related content and between the file name and the file content. 
    An additional boolean is used for every line of the source code to check if it has been used before, so that it is easier to determine when it should be greyed-out. 
    A sorted version of the llvm-dwarfdump table (based on line number) is used to deal with the disordered line order with "-O3". The range of source lines that is corresponding to a certain assembly block is determined by the current line number and the previous line number in the sorted table. 

    ERB is used for easier HTML generation because it supports inline code. 

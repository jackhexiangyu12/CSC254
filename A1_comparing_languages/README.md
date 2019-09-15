# A1 Comparing Languages

Gao Fan (gfan@u.rochester.edu), Jianyuan Zhong (jzhong9@u.rochester.edu)

## How to Run

Files for each languages are located in their corresponding folders. 

**Ada**

```
gnat make trees.adb
./trees num_of_nodes
```

**C#**

```
mcs trees.cs
mono trees.exe num_of_nodes
```

**Ocaml**

```
ocaml trees.ml num_of_nodes
```

**Prolog**

```
swipl trees.pl
?- trees(num_of_nodes).
```

**Python**

```
python3 trees.py num_of_nodes
```

Replace `num_of_nodes` with any integer greater than or equal to zero. 

## Features

- Since we use dynamic programming (except for OCaml) and store trees as list of strings, our implementations perform better in both runtime and memory consumption. 
- Our implementation in Prolog can print all possible trees without typing semicolons at the interpreter prompt. 

## Programming Experience

**What was easy?  What was hard?**

Generally speaking, imperative languages are easier to implement compare to declarative languages. 

Ada, C# and Python are ranked according to their grammars. Ada is the most complicated one and Python is the easiest and most familiar one. 

On the other hand, OCaml and Prolog require a completely different approach. We used `findall/3` in Prolog to replace the nested loop because variables and lists cannot be updated, and used recursion in OCaml because we did not find a way to implement dynamic programming in functional language. 

**Are there noticeable differences in speed?**

With small `num_of_nodes`, there is not significant differences in speed. 

**What do you like/dislike?**

We like Python most and declarative languages least due to similar reasons in "What was easy? What was hard?". 

**Did you find iterators to be helpful? **


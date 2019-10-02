trees(N) :- gen(0, N, []).

% When trees are generated
gen(I, Target, Result) :-
    TargetPlusOne is Target+1,
    I = TargetPlusOne,

    last(Result, Trees),
    print_trees(Trees),
    halt.

% When N is 0
gen(I, Target, Result) :-
    TargetPlusOne is Target+1,
    I < TargetPlusOne,
    I = 0,
    Next is I+1,

    append(Result, [['']], ResultNext),
    gen(Next, Target, ResultNext).

% When N is 1
gen(I, Target, Result) :-
    TargetPlusOne is Target+1,
    I < TargetPlusOne,
    I = 1,
    Next is I+1,

    append(Result, [['(.)']], ResultNext),
    gen(Next, Target, ResultNext).

% When N is greater than 1
gen(I, Target, Result) :-
    TargetMinusOne is Target-1,
    TargetPlusOne is Target+1,
    I < TargetPlusOne,
    I > 1,
    Next is I+1,

    findall(A, (
        % For all possible J and K
        between(0, TargetMinusOne, J),
        K is I-J-1,

        % For all possible leftsubtree with J nodes and rightsubtree with K nodes
        nth0(J, Result, Left),
        nth0(K, Result, Right),
        member(L, Left),
        member(R, Right),

        % Add leftsubtree and rightsubtree to root and store in ResultNew
        add_to_root(L, R, A)
    ), ResultNew),

    append(Result, [ResultNew], ResultNext),
    gen(Next, Target, ResultNext).

add_to_root(L, R, A) :-
    string_concat('(', L, S1),
    string_concat(S1, '.', S2),
    string_concat(S2, R, S3),
    string_concat(S3, ')', A).

print_trees(Trees) :-
    forall(member(Tree, Trees), writeln(Tree)).
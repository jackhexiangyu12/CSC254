import sys

def gen(n):
    result = []

    # When N is 0
    tmp = ['']
    result.append(tmp)

    if n > 0:
        # When N is 1
        tmp = ['(.)']
        result.append(tmp)

        # When N is greater than 1
        for i in range(2,n+1):
            tmp = []
            # For all possible leftsubtree with j nodes and rightsubtree with i-j-1 nodes
            for j in reversed(range(i)):
                for left in result[j]:
                    for right in result[i-j-1]:
                        # Add leftsubtree and rightsubtree to root and store in list
                        tmp.append('(' + left + '.' + right + ')')
            result.append(tmp)
        
    return result[-1]

def main():
    n = int(sys.argv[1])

    trees = gen(n)
    for tree in trees:
        print(tree)

if __name__ == "__main__":
    main()
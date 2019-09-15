import sys

def gen(n):
    result = []

    tmp = ['']
    result.append(tmp)

    if n > 0:
        tmp = ['(.)']
        result.append(tmp)

        for i in range(2,n+1):
            tmp = []
            for j in reversed(range(i)):
                for left in result[j]:
                    for right in result[i-j-1]:
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
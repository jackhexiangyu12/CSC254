#!/usr/bin/python3

'''
Script to check output of dining philosophers.
Must give a file with lines of the format:
Philosopher <n> <state>
where <n> is {1, 2, 3, 4, 5} and <state> is {thinking, waiting, eating}

Program will check to make sure no two adjacent philosophers are eating
at the same time and output a showing the proportion of time each
philosopher spent in each state.
'''

import sys

state_codes = {
    "thinking" : 0,
    "waiting" : 1,
    "eating" : 2
}

states = [0, 0, 0, 0, 0]

time_spent = [[0, 0, 0] for i in range(5)]



def update_state(line):
    i = int(line.split()[1])
    state = state_codes[line.split()[2]]
    states[i-1] = state
    for p, s in enumerate(states):
        time_spent[p][s] += 1

def check_states(lineno):
    for i in range(0, 4):
        if states[i] == 2 and states[i+1] == 2:
            print("Correctness error detected at line {}:".format(lineno+1))
            print("Philosophers {} and {} eating at the same time.".format(i+1, i+2))
            return False
    if states[0] == 2 and states[4] == 2:
        print("Correctness error detected at line {}:".format(lineno+1))
        print("Philosophers 1 and 5 eating at the same time.")
        return False
    return True

def print_time_spent():
    print("Fairness breakdown:")
    print("\t\tThink\tWait\tEat")
    total = sum(time_spent[0])
    for p, row in enumerate(time_spent):
        think_pct = round(100*row[0]/total, 1)
        wait_pct = round(100*row[1]/total, 1)
        eat_pct = round(100*row[2]/total, 1)
        print("Philosopher {}:\t{}%\t{}%\t{}%".format(p+1, think_pct, wait_pct, eat_pct))

if __name__ == '__main__':

    if len(sys.argv) != 2:
        print("Usage: ./check_output.py output_file")
        exit(1)

    in_filename = sys.argv[1]

    with open(in_filename, 'r') as f:
        correct = True
        for lineno, line in enumerate(f.readlines()):
            update_state(line)
            if not check_states(lineno):
                correct = False
                break
        if correct:
            print("Correctness check passed!")
        print_time_spent()
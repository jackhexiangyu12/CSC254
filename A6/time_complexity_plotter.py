import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

fig = plt.figure(figsize=(10, 4))
fig.subplots_adjust(hspace=0.4, wspace=0.4)
curr_row = 0

def plot(name):
    global curr_row
    xs = []
    ys = []
    zs = []
    ws = []
    with open(name + ".dat") as f:
        skipped = False
        for line in f.readlines():
            if skipped == False:
                skipped = True
                continue
            x,y,z,w = line.strip().split(',') 
            xs.append(int(x))
            ys.append(int(y))
            zs.append(int(z))
            ws.append(int(w))

    ax = fig.add_subplot(2,3, 3 * curr_row + 1)
    ax.set_title(name + ".(+=)")
    ax.set_ylabel("Processor Instructions")
    ax.set_xlabel("Number of Operations")
    plt.plot(xs, ys)
    plt.ylim(int(min(ys) * 0.9), int(max(ys) * 1.1))
    ax2 = fig.add_subplot(2,3,3 * curr_row + 2)
    ax2.set_title(name + ".contains")
    ax2.set_ylabel("Processor Instructions")
    ax2.set_xlabel("Number of Operations")
    plt.ylim(int(min(zs) * 0.9), int(max(zs) * 1.1))
    plt.plot(xs, zs)
    ax3 = fig.add_subplot(2,3,3 * curr_row + 3)
    ax3.set_title(name + ".(-=)")
    ax3.set_ylabel("Processor Instructions")
    ax3.set_xlabel("Number of Operations")
    plt.ylim(int(min(ws) * 0.9), int(max(ws) * 1.1))
    plt.plot(xs, ws)
    curr_row += 1


plot("std_simple_set")
# plot("std_range_set")
# plot("carray_simple_set")
# plot("hashed_simple_set")
plot("bin_search_simple_set")
# plot("carray_range_set")
# plot("hashed_range_set")
# plot("bin_search_range_set")

plt.tight_layout()
plt.savefig('time_complexity.png')
plt.show()
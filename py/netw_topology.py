#! /usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.lines import Line2D
import MySQLdb


ids = []
types = []
radii = []
xs = []
ys = []
neis = []

cxn = MySQLdb.connect(user = 'root', db = 'cpp', passwd = '0000000027')
cur = cxn.cursor()

cur.execute('SELECT id FROM graph')
for data in cur.fetchall():
    ids.append(data[0])

cur.execute('SELECT type FROM graph')
for data in cur.fetchall():
    types.append(data[0])

cur.execute('SELECT radius FROM graph')
for data in cur.fetchall():
    radii.append(data[0])

cur.execute('SELECT x FROM graph')
for data in cur.fetchall():
    xs.append(data[0])

cur.execute('SELECT y FROM graph')
for data in cur.fetchall():
    ys.append(data[0])

cur.execute('SELECT neighbors FROM graph')
for data in cur.fetchall():
    neis.append(data[0])

# convert strings to int
for i in range(0, len(neis)):
    nodes = neis[i].split(',')
    tmp = []
    if len(neis[i]) != 0:
        for n in nodes:
            tmp.append(int(n))
        neis[i] = tmp

#for i in range(0, len(ids)):
#    print str(ids[i]) + ' ' + str(types[i]) + ' ' + str(radii[i]) +\
#          ' ' + str(xs[i]) + str(ys[i]) + ' ' + neis[i]

xcoor = np.array(range(0, 101, 5))
ycoor = np.array(range(0, 101, 5))

fig, ax = plt.subplots(1, 1)
ax.set_xticks(xcoor)
ax.set_yticks(ycoor)

# draw edges
for i in range(0, len(ids)):
    for j in range(0, len(neis[i])):
        line = Line2D([xs[i], xs[neis[i][j]]], [ys[i], ys[neis[i][j]]], linewidth=1, color="b")
        ax.add_line(line)

# draw nodes
for i in range(0, len(ids)):
    node = None
    # draw sensors
    if types[i] == 0:
        node = mpatches.Polygon([[xs[i]-1, ys[i]-1], [xs[i]+1, ys[i]-1], [xs[i], ys[i]+1]], color='g', alpha=0.5)
    # draw relays
    elif types[i] == 1:
        node = mpatches.Rectangle((xs[i]-1, ys[i]-1), 2, 2, color='r', alpha=0.5)
    # draw sinks
    elif types[i] == 2:
        node = mpatches.Circle((xs[i], ys[i]), 1, color='b', alpha=0.7)
    if node != None:
        ax.add_patch(node)
        ax.text(xs[i], ys[i]+1, str(ids[i]), family='monospace', fontsize=10)

plt.show()

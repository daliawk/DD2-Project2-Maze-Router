import matplotlib.pyplot as plt
import numpy as np
import os.path
import random


if not os.path.isfile("Routed Nets.txt"):
    print('Could not find a text file with the name Routed Nets.txt. Please make sure it is in the same folder '
          'as the file')
    exit(1)
with open("Routed Nets.txt") as f:
    nets_str = f.read().splitlines()

dimensions = nets_str[0].split("x")
M1 = np.zeros((int(dimensions[0]), int(dimensions[1])), dtype=int)
M2 = np.zeros((int(dimensions[0]), int(dimensions[1])), dtype=int)
net_names = []
net_idx = 1
for str in nets_str[1:]:
    splitted_str = str.split('(')
    net_names.append(splitted_str[0])
    for i in range(1, len(splitted_str)):
        coordinates = splitted_str[i].split(",")
        coordinates[2] = coordinates[2].rstrip(")")
        if int(coordinates[0]) == 1:
            M1[int(coordinates[1])][int(coordinates[2])] = net_idx
        else:
            M2[int(coordinates[1])][int(coordinates[2])] = net_idx
    net_idx += 1

for i in range(int(dimensions[0])):
    for j in range(int(dimensions[1])):
        print(M1[i][j], end=" ")
    print()
print("---------------------------------------------------------------")
for i in range(int(dimensions[0])):
    for j in range(int(dimensions[1])):
        print(M2[i][j], end=" ")
    print()

empty_cells = [["" for c in range(int(dimensions[1]))] for r in range(int(dimensions[0]))]

colors = []
for idx in range(net_idx):
    colors.append((random.random(), random.random(), random.random()))
colors[0] = (1.0,1.0,1.0)
M1_c = np.empty((int(dimensions[0]), int(dimensions[1])), dtype=tuple)
M2_c = np.empty((int(dimensions[0]), int(dimensions[1])), dtype=tuple)
for i in range(int(dimensions[0])):
    for j in range(int(dimensions[1])):
        M1_c[i][j] = colors[M1[i][j]]
        M2_c[i][j] = colors[M2[i][j]]


fig1, ax1 = plt.subplots()
ax1.set_axis_off()
table1 = plt.table(
    cellText=empty_cells,
    cellColours=M1_c,
    loc='upper left')
table2 = plt.table(
    cellText=empty_cells,
    cellColours=M2_c,
    loc='lower left')

plt.show()
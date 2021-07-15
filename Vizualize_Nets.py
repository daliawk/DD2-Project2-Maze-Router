import matplotlib.pyplot as plt
import numpy as np
import os.path
import random

# Opening the the file with the routed nets
if not os.path.isfile("Routed Nets.txt"):
    print('Could not find a text file with the name Routed Nets.txt. Please make sure it is in the same folder '
          'as the file')
    exit(1)
with open("Routed Nets.txt") as f:
    nets_str = f.read().splitlines()

# Opening the the file with the input file
if not os.path.isfile("DEF_file.txt"):
    print('Could not find a text file with the name Routed Nets.txt. Please make sure it is in the same folder '
          'as the file')
    exit(1)
with open("DEF_file.txt") as f:
    input_file = f.read().splitlines()


dimensions = input_file[0].split("x")   # Array dimensions

# The arrays with the nets codes
M1 = np.zeros((int(dimensions[0]), int(dimensions[1])), dtype=int)
M2 = np.zeros((int(dimensions[0]), int(dimensions[1])), dtype=int)

# The arrays with the vias or pin representation
text_cells1 = [["" for c in range(int(dimensions[1]))] for r in range(int(dimensions[0]))]
text_cells2 = [["" for c in range(int(dimensions[1]))] for r in range(int(dimensions[0]))]

# Reading the input file
for str in input_file[1:]:
    # Reading Obstacles
    if str[0:3] == "OBS":
        str = str.removeprefix("OBS")
        coordinates = str.split(",")
        M1[int(coordinates[0][1:])][int(coordinates[1].rstrip(")"))] = -1
        M2[int(coordinates[0][1:])][int(coordinates[1].rstrip(")"))] = -1
    # Reading Net Pins
    else:
        splitted_str = str.split('(')
        for i in range(1, len(splitted_str)):
            coordinates = splitted_str[i].split(",")
            coordinates[2] = coordinates[2].rstrip(")")
            if int(coordinates[0]) == 1:
                if i == 1:
                    text_cells1[int(coordinates[1])][int(coordinates[2])] = "S"
                else:
                    text_cells1[int(coordinates[1])][int(coordinates[2])] = "T"
            else:
                if i == 1:
                    text_cells2[int(coordinates[1])][int(coordinates[2])] = "S"
                else:
                    text_cells2[int(coordinates[1])][int(coordinates[2])] = "T"
# Reading nets
net_idx = 1
for str in nets_str:
    splitted_str = str.split('(')
    for i in range(1, len(splitted_str)):
        coordinates = splitted_str[i].split(",")
        coordinates[2] = coordinates[2].rstrip(")")
        if int(coordinates[0]) == 1:
            M1[int(coordinates[1])][int(coordinates[2])] = net_idx
        else:
            M2[int(coordinates[1])][int(coordinates[2])] = net_idx
    net_idx += 1

# Marking vias
for i in range(int(dimensions[0])):
    for j in range(int(dimensions[1])):
        if M1[i][j] > 0:
            if M1[i][j] == M2[i][j]:
                text_cells1[i][j] += "@"
                text_cells2[i][j] += "@"
# Forming the color map
colors = []
for idx in range(net_idx):
    colors.append((random.random(), random.random(), random.random()))
colors[0] = (1.0, 1.0, 1.0)
M1_c = np.empty((int(dimensions[0]), int(dimensions[1])), dtype=tuple)
M2_c = np.empty((int(dimensions[0]), int(dimensions[1])), dtype=tuple)
for i in range(int(dimensions[0])):
    for j in range(int(dimensions[1])):
        if M1[i][j] == -1:
            M1_c[i][j] = (0, 0, 0)
            M2_c[i][j] = (0, 0, 0)
        else:
            M1_c[i][j] = colors[M1[i][j]]
            M2_c[i][j] = colors[M2[i][j]]
# Plotting the tables
fig, ax = plt.subplots()
ax.set_axis_off()
table1 = plt.table(
    cellText=text_cells1,
    cellColours=M1_c,
    loc='upper left')
table2 = plt.table(
    cellText=text_cells2,
    cellColours=M2_c,
    loc='lower left')
ax.set_title('M1 (The Upper Layer)   M2 (The lower Layer)',
             fontweight="bold")
plt.show()
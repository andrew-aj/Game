from typing import List

x = 30
y = 30
indexCount = (x - 1) * 2 * (y - 1)

assert (y > 1)

vertices = [[0.0] * 3 for i in range(x * y)]
indices = [[0] * 3 for b in range(indexCount)]

for i in range(y):
    for j in range(x):
        vertices[i * x + j] = [j, i, 0]

index = 0

for i in range(y - 1):
    for j in range((x - 1) * 2):
        if j % 2 is 0:
            indices[index] = [int(j/2),int((i+1)*x+(j/2)),int((i+1)*x+(j/2+1))]
        else:
            indices[index] = [int((j-1)/2),int((j+1)/2),int((i+1)*x+((j-1)/2+1))]
        index += 1

print(index)
with open('../bin/data/models/cubesphere.dt', 'r') as file:
    data = file.readlines()

t = ""
t += data[0]
t += data[1]
t += data[2]
t += data[3]
t += data[4]
temp = "[{"

for i in range(x*y-1):
    temp += "{" + str(vertices[i][0]) + "," + str(vertices[i][1]) + "," + str(vertices[i][2]) + "}" + ","

temp += "{" + str(vertices[x*y-1][0]) + "," + str(vertices[x*y-1][1]) + "," + str(vertices[x*y-1][2]) + "}" + "}]\n"
t += temp

temp = "[{"

for i in range(indexCount-1):
    temp += str(indices[i][0]) + "," + str(indices[i][1]) + "," + str(indices[i][2]) + ","

temp += str(indices[indexCount-1][0]) + "," + str(indices[indexCount-1][1]) + "," + str(indices[indexCount-1][2]) + "}]\n"
t += temp

with open('../bin/data/models/cubesphere.dt', 'w') as file:
    file.write(t)
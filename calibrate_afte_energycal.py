import numpy as np

data = np.loadtxt("peaks0410.data",delimiter=",")


energies = data[:,0]
peaks = data[:,1:]

print(energies,peaks)


for det in range(30):
    p = np.polyfit(y=energies,x=peaks[:,det],deg=1)
    print("{"+str(p[0])+","+str(p[1])+"},")
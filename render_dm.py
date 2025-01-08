import numpy as np
from sklearn.manifold import TSNE
from sklearn.manifold import MDS
import matplotlib.pyplot as plt
    
   
matrix0 = np.loadtxt(open("prepare-data/Cylinder2D.cdb/dm_la0.csv", "rb"), delimiter=",", skiprows=1)
matrix1 = np.loadtxt(open("prepare-data/Cylinder2D.cdb/dm_la1.csv", "rb"), delimiter=",", skiprows=1)
matrix2 = np.loadtxt(open("prepare-data/Cylinder2D.cdb/dm_la2.csv", "rb"), delimiter=",", skiprows=1)

fig, ax = plt.subplots()
ax.imshow(matrix0, cmap='coolwarm', interpolation='nearest')
ax.set_xticks(np.arange(0,301,25))
ax.set_yticks(np.arange(0,301,25))
fig.savefig("dm_la0.pdf", transparent=True, bbox_inches="tight", pad_inches=0.1)

fig, ax = plt.subplots()
ax.imshow(matrix1, cmap='coolwarm', interpolation='nearest')
ax.set_xticks(np.arange(0,301,25))
ax.set_yticks(np.arange(0,301,25))
fig.savefig("dm_la1.pdf", transparent=True, bbox_inches="tight", pad_inches=0.1)

fig, ax = plt.subplots()
ax.imshow(matrix2, cmap='coolwarm', interpolation='nearest')
ax.set_xticks(np.arange(0,301,25))
ax.set_yticks(np.arange(0,301,25))
fig.savefig("dm_la2.pdf", transparent=True, bbox_inches="tight", pad_inches=0.1)

import numpy as np
from sklearn.manifold import TSNE
from sklearn.manifold import MDS
import matplotlib.pyplot as plt
    
   
matrix0 = np.loadtxt(open("prepare-data/Cylinder2D.cdb/dm_la0.csv", "rb"), delimiter=",", skiprows=1)
matrix1 = np.loadtxt(open("prepare-data/Cylinder2D.cdb/dm_la1.csv", "rb"), delimiter=",", skiprows=1)
matrix2 = np.loadtxt(open("prepare-data/Cylinder2D.cdb/dm_la2.csv", "rb"), delimiter=",", skiprows=1)

embedding0 = TSNE(n_components=2).fit_transform(matrix0)
embedding1 = TSNE(n_components=2).fit_transform(matrix1)
embedding2 = TSNE(n_components=2).fit_transform(matrix2)

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

colarr = range(300)

fig, ax = plt.subplots()
im = ax.scatter([x[0] for x in embedding0],[x[1] for x in embedding0], label="PMD0", s=4, c=colarr)
ax.set_xticks([])
ax.set_yticks([])
fig.set_size_inches(3,3)
fig.patch.set_alpha(0.5)
ax.axes.patch.set_alpha(0.5)
plt.setp(ax.spines.values(), linewidth=3)
fig.savefig("embedding_pmd0.pdf", bbox_inches="tight")

fig, ax = plt.subplots()
im = ax.scatter([x[0] for x in embedding1],[x[1] for x in embedding1], label="PMD0", s=4, c=colarr)
ax.set_xticks([])
ax.set_yticks([])
fig.set_size_inches(3,3)
fig.patch.set_alpha(0.5)
ax.axes.patch.set_alpha(0.5)
plt.setp(ax.spines.values(), linewidth=3)
fig.savefig("embedding_pmd1.pdf", bbox_inches="tight")

fig, ax = plt.subplots()
im = ax.scatter([x[0] for x in embedding2],[x[1] for x in embedding2], label="PMD0", s=4, c=colarr)
ax.set_xticks([])
ax.set_yticks([])
fig.set_size_inches(3,3)
fig.patch.set_alpha(0.5)
ax.axes.patch.set_alpha(0.5)
plt.setp(ax.spines.values(), linewidth=3)
fig.savefig("embedding_pmd2.pdf", bbox_inches="tight")

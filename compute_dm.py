# state file generated using paraview version 5.10.1

# uncomment the following three lines to ensure this script works in future versions
#import paraview
#paraview.compatibility.major = 5
#paraview.compatibility.minor = 10

import numpy as np
from sklearn.manifold import TSNE
from sklearn.manifold import MDS
import matplotlib.pyplot as plt

#### import the simple module from the paraview
from paraview.simple import *
#### disable automatic camera reset on 'Show'
paraview.simple._DisableFirstRenderCameraReset()

# ----------------------------------------------------------------
# setup the data processing pipelines
# ----------------------------------------------------------------

# create a new 'TTK CinemaReader'
tTKCinemaReader1 = TTKCinemaReader(registrationName='TTKCinemaReader1', DatabasePath='Cylinder2D.cdb')

# create a new 'TTK CinemaProductReader'
tTKCinemaProductReader1 = TTKCinemaProductReader(registrationName='TTKCinemaProductReader1', Input=tTKCinemaReader1)

# create a new 'TTK MergeTree'
tTKMergeTree1 = TTKMergeTree(registrationName='TTKMergeTree1', Input=tTKCinemaProductReader1)
tTKMergeTree1.ScalarField = ['POINTS', 'magnitude']
tTKMergeTree1.InputOffsetField = ['POINTS', 'magnitude']
tTKMergeTree1.TreeType = 'Split Tree'

# find source
tTKMergeTree1_1 = FindSource('TTKMergeTree1')

# create a new 'TTK BlockAggregator'
tTKBlockAggregator1 = TTKBlockAggregator(registrationName='TTKBlockAggregator1', Input=[tTKMergeTree1, OutputPort(tTKMergeTree1_1,1)])
tTKBlockAggregator1.FlattenInput = 0

# create a new 'TTK MergeTreeDistanceMatrix'
tTKMergeTreeDistanceMatrix1 = TTKMergeTreeDistanceMatrix(registrationName='TTKMergeTreeDistanceMatrix1', Input=tTKBlockAggregator1,
    OptionalInput=None)
tTKMergeTreeDistanceMatrix1.Backend = 'Path Mapping Distance (TopoInVis 2022)'
tTKMergeTreeDistanceMatrix1.DistanceSquareRoot = 0
tTKMergeTreeDistanceMatrix1.Epsilon1 = 0.0
tTKMergeTreeDistanceMatrix1.Epsilon2 = 100.0
tTKMergeTreeDistanceMatrix1.Epsilon3 = 100.0

# set active source
SetActiveSource(tTKMergeTreeDistanceMatrix1)

tTKMergeTreeDistanceMatrix1.PathMappingLookahead = 0

# save data
SaveData('Cylinder2D.cdb/dm_la0.csv', proxy=tTKMergeTreeDistanceMatrix1, RowDataArrays=['Tree000', 'Tree001', 'Tree002', 'Tree003', 'Tree004', 'Tree005', 'Tree006', 'Tree007', 'Tree008', 'Tree009', 'Tree010', 'Tree011', 'Tree012', 'Tree013', 'Tree014', 'Tree015', 'Tree016', 'Tree017', 'Tree018', 'Tree019', 'Tree020', 'Tree021', 'Tree022', 'Tree023', 'Tree024', 'Tree025', 'Tree026', 'Tree027', 'Tree028', 'Tree029', 'Tree030', 'Tree031', 'Tree032', 'Tree033', 'Tree034', 'Tree035', 'Tree036', 'Tree037', 'Tree038', 'Tree039', 'Tree040', 'Tree041', 'Tree042', 'Tree043', 'Tree044', 'Tree045', 'Tree046', 'Tree047', 'Tree048', 'Tree049', 'Tree050', 'Tree051', 'Tree052', 'Tree053', 'Tree054', 'Tree055', 'Tree056', 'Tree057', 'Tree058', 'Tree059', 'Tree060', 'Tree061', 'Tree062', 'Tree063', 'Tree064', 'Tree065', 'Tree066', 'Tree067', 'Tree068', 'Tree069', 'Tree070', 'Tree071', 'Tree072', 'Tree073', 'Tree074', 'Tree075', 'Tree076', 'Tree077', 'Tree078', 'Tree079', 'Tree080', 'Tree081', 'Tree082', 'Tree083', 'Tree084', 'Tree085', 'Tree086', 'Tree087', 'Tree088', 'Tree089', 'Tree090', 'Tree091', 'Tree092', 'Tree093', 'Tree094', 'Tree095', 'Tree096', 'Tree097', 'Tree098', 'Tree099', 'Tree100', 'Tree101', 'Tree102', 'Tree103', 'Tree104', 'Tree105', 'Tree106', 'Tree107', 'Tree108', 'Tree109', 'Tree110', 'Tree111', 'Tree112', 'Tree113', 'Tree114', 'Tree115', 'Tree116', 'Tree117', 'Tree118', 'Tree119', 'Tree120', 'Tree121', 'Tree122', 'Tree123', 'Tree124', 'Tree125', 'Tree126', 'Tree127', 'Tree128', 'Tree129', 'Tree130', 'Tree131', 'Tree132', 'Tree133', 'Tree134', 'Tree135', 'Tree136', 'Tree137', 'Tree138', 'Tree139', 'Tree140', 'Tree141', 'Tree142', 'Tree143', 'Tree144', 'Tree145', 'Tree146', 'Tree147', 'Tree148', 'Tree149', 'Tree150', 'Tree151', 'Tree152', 'Tree153', 'Tree154', 'Tree155', 'Tree156', 'Tree157', 'Tree158', 'Tree159', 'Tree160', 'Tree161', 'Tree162', 'Tree163', 'Tree164', 'Tree165', 'Tree166', 'Tree167', 'Tree168', 'Tree169', 'Tree170', 'Tree171', 'Tree172', 'Tree173', 'Tree174', 'Tree175', 'Tree176', 'Tree177', 'Tree178', 'Tree179', 'Tree180', 'Tree181', 'Tree182', 'Tree183', 'Tree184', 'Tree185', 'Tree186', 'Tree187', 'Tree188', 'Tree189', 'Tree190', 'Tree191', 'Tree192', 'Tree193', 'Tree194', 'Tree195', 'Tree196', 'Tree197', 'Tree198', 'Tree199', 'Tree200', 'Tree201', 'Tree202', 'Tree203', 'Tree204', 'Tree205', 'Tree206', 'Tree207', 'Tree208', 'Tree209', 'Tree210', 'Tree211', 'Tree212', 'Tree213', 'Tree214', 'Tree215', 'Tree216', 'Tree217', 'Tree218', 'Tree219', 'Tree220', 'Tree221', 'Tree222', 'Tree223', 'Tree224', 'Tree225', 'Tree226', 'Tree227', 'Tree228', 'Tree229', 'Tree230', 'Tree231', 'Tree232', 'Tree233', 'Tree234', 'Tree235', 'Tree236', 'Tree237', 'Tree238', 'Tree239', 'Tree240', 'Tree241', 'Tree242', 'Tree243', 'Tree244', 'Tree245', 'Tree246', 'Tree247', 'Tree248', 'Tree249', 'Tree250', 'Tree251', 'Tree252', 'Tree253', 'Tree254', 'Tree255', 'Tree256', 'Tree257', 'Tree258', 'Tree259', 'Tree260', 'Tree261', 'Tree262', 'Tree263', 'Tree264', 'Tree265', 'Tree266', 'Tree267', 'Tree268', 'Tree269', 'Tree270', 'Tree271', 'Tree272', 'Tree273', 'Tree274', 'Tree275', 'Tree276', 'Tree277', 'Tree278', 'Tree279', 'Tree280', 'Tree281', 'Tree282', 'Tree283', 'Tree284', 'Tree285', 'Tree286', 'Tree287', 'Tree288', 'Tree289', 'Tree290', 'Tree291', 'Tree292', 'Tree293', 'Tree294', 'Tree295', 'Tree296', 'Tree297', 'Tree298', 'Tree299', 'Tree300', 'treeID'],
    FieldAssociation='Row Data')

tTKMergeTreeDistanceMatrix1.PathMappingLookahead = 1

# save data
SaveData('Cylinder2D.cdb/dm_la1.csv', proxy=tTKMergeTreeDistanceMatrix1, RowDataArrays=['Tree000', 'Tree001', 'Tree002', 'Tree003', 'Tree004', 'Tree005', 'Tree006', 'Tree007', 'Tree008', 'Tree009', 'Tree010', 'Tree011', 'Tree012', 'Tree013', 'Tree014', 'Tree015', 'Tree016', 'Tree017', 'Tree018', 'Tree019', 'Tree020', 'Tree021', 'Tree022', 'Tree023', 'Tree024', 'Tree025', 'Tree026', 'Tree027', 'Tree028', 'Tree029', 'Tree030', 'Tree031', 'Tree032', 'Tree033', 'Tree034', 'Tree035', 'Tree036', 'Tree037', 'Tree038', 'Tree039', 'Tree040', 'Tree041', 'Tree042', 'Tree043', 'Tree044', 'Tree045', 'Tree046', 'Tree047', 'Tree048', 'Tree049', 'Tree050', 'Tree051', 'Tree052', 'Tree053', 'Tree054', 'Tree055', 'Tree056', 'Tree057', 'Tree058', 'Tree059', 'Tree060', 'Tree061', 'Tree062', 'Tree063', 'Tree064', 'Tree065', 'Tree066', 'Tree067', 'Tree068', 'Tree069', 'Tree070', 'Tree071', 'Tree072', 'Tree073', 'Tree074', 'Tree075', 'Tree076', 'Tree077', 'Tree078', 'Tree079', 'Tree080', 'Tree081', 'Tree082', 'Tree083', 'Tree084', 'Tree085', 'Tree086', 'Tree087', 'Tree088', 'Tree089', 'Tree090', 'Tree091', 'Tree092', 'Tree093', 'Tree094', 'Tree095', 'Tree096', 'Tree097', 'Tree098', 'Tree099', 'Tree100', 'Tree101', 'Tree102', 'Tree103', 'Tree104', 'Tree105', 'Tree106', 'Tree107', 'Tree108', 'Tree109', 'Tree110', 'Tree111', 'Tree112', 'Tree113', 'Tree114', 'Tree115', 'Tree116', 'Tree117', 'Tree118', 'Tree119', 'Tree120', 'Tree121', 'Tree122', 'Tree123', 'Tree124', 'Tree125', 'Tree126', 'Tree127', 'Tree128', 'Tree129', 'Tree130', 'Tree131', 'Tree132', 'Tree133', 'Tree134', 'Tree135', 'Tree136', 'Tree137', 'Tree138', 'Tree139', 'Tree140', 'Tree141', 'Tree142', 'Tree143', 'Tree144', 'Tree145', 'Tree146', 'Tree147', 'Tree148', 'Tree149', 'Tree150', 'Tree151', 'Tree152', 'Tree153', 'Tree154', 'Tree155', 'Tree156', 'Tree157', 'Tree158', 'Tree159', 'Tree160', 'Tree161', 'Tree162', 'Tree163', 'Tree164', 'Tree165', 'Tree166', 'Tree167', 'Tree168', 'Tree169', 'Tree170', 'Tree171', 'Tree172', 'Tree173', 'Tree174', 'Tree175', 'Tree176', 'Tree177', 'Tree178', 'Tree179', 'Tree180', 'Tree181', 'Tree182', 'Tree183', 'Tree184', 'Tree185', 'Tree186', 'Tree187', 'Tree188', 'Tree189', 'Tree190', 'Tree191', 'Tree192', 'Tree193', 'Tree194', 'Tree195', 'Tree196', 'Tree197', 'Tree198', 'Tree199', 'Tree200', 'Tree201', 'Tree202', 'Tree203', 'Tree204', 'Tree205', 'Tree206', 'Tree207', 'Tree208', 'Tree209', 'Tree210', 'Tree211', 'Tree212', 'Tree213', 'Tree214', 'Tree215', 'Tree216', 'Tree217', 'Tree218', 'Tree219', 'Tree220', 'Tree221', 'Tree222', 'Tree223', 'Tree224', 'Tree225', 'Tree226', 'Tree227', 'Tree228', 'Tree229', 'Tree230', 'Tree231', 'Tree232', 'Tree233', 'Tree234', 'Tree235', 'Tree236', 'Tree237', 'Tree238', 'Tree239', 'Tree240', 'Tree241', 'Tree242', 'Tree243', 'Tree244', 'Tree245', 'Tree246', 'Tree247', 'Tree248', 'Tree249', 'Tree250', 'Tree251', 'Tree252', 'Tree253', 'Tree254', 'Tree255', 'Tree256', 'Tree257', 'Tree258', 'Tree259', 'Tree260', 'Tree261', 'Tree262', 'Tree263', 'Tree264', 'Tree265', 'Tree266', 'Tree267', 'Tree268', 'Tree269', 'Tree270', 'Tree271', 'Tree272', 'Tree273', 'Tree274', 'Tree275', 'Tree276', 'Tree277', 'Tree278', 'Tree279', 'Tree280', 'Tree281', 'Tree282', 'Tree283', 'Tree284', 'Tree285', 'Tree286', 'Tree287', 'Tree288', 'Tree289', 'Tree290', 'Tree291', 'Tree292', 'Tree293', 'Tree294', 'Tree295', 'Tree296', 'Tree297', 'Tree298', 'Tree299', 'Tree300', 'treeID'],
    FieldAssociation='Row Data')

tTKMergeTreeDistanceMatrix1.PathMappingLookahead = 2

# save data
SaveData('Cylinder2D.cdb/dm_la2.csv', proxy=tTKMergeTreeDistanceMatrix1, RowDataArrays=['Tree000', 'Tree001', 'Tree002', 'Tree003', 'Tree004', 'Tree005', 'Tree006', 'Tree007', 'Tree008', 'Tree009', 'Tree010', 'Tree011', 'Tree012', 'Tree013', 'Tree014', 'Tree015', 'Tree016', 'Tree017', 'Tree018', 'Tree019', 'Tree020', 'Tree021', 'Tree022', 'Tree023', 'Tree024', 'Tree025', 'Tree026', 'Tree027', 'Tree028', 'Tree029', 'Tree030', 'Tree031', 'Tree032', 'Tree033', 'Tree034', 'Tree035', 'Tree036', 'Tree037', 'Tree038', 'Tree039', 'Tree040', 'Tree041', 'Tree042', 'Tree043', 'Tree044', 'Tree045', 'Tree046', 'Tree047', 'Tree048', 'Tree049', 'Tree050', 'Tree051', 'Tree052', 'Tree053', 'Tree054', 'Tree055', 'Tree056', 'Tree057', 'Tree058', 'Tree059', 'Tree060', 'Tree061', 'Tree062', 'Tree063', 'Tree064', 'Tree065', 'Tree066', 'Tree067', 'Tree068', 'Tree069', 'Tree070', 'Tree071', 'Tree072', 'Tree073', 'Tree074', 'Tree075', 'Tree076', 'Tree077', 'Tree078', 'Tree079', 'Tree080', 'Tree081', 'Tree082', 'Tree083', 'Tree084', 'Tree085', 'Tree086', 'Tree087', 'Tree088', 'Tree089', 'Tree090', 'Tree091', 'Tree092', 'Tree093', 'Tree094', 'Tree095', 'Tree096', 'Tree097', 'Tree098', 'Tree099', 'Tree100', 'Tree101', 'Tree102', 'Tree103', 'Tree104', 'Tree105', 'Tree106', 'Tree107', 'Tree108', 'Tree109', 'Tree110', 'Tree111', 'Tree112', 'Tree113', 'Tree114', 'Tree115', 'Tree116', 'Tree117', 'Tree118', 'Tree119', 'Tree120', 'Tree121', 'Tree122', 'Tree123', 'Tree124', 'Tree125', 'Tree126', 'Tree127', 'Tree128', 'Tree129', 'Tree130', 'Tree131', 'Tree132', 'Tree133', 'Tree134', 'Tree135', 'Tree136', 'Tree137', 'Tree138', 'Tree139', 'Tree140', 'Tree141', 'Tree142', 'Tree143', 'Tree144', 'Tree145', 'Tree146', 'Tree147', 'Tree148', 'Tree149', 'Tree150', 'Tree151', 'Tree152', 'Tree153', 'Tree154', 'Tree155', 'Tree156', 'Tree157', 'Tree158', 'Tree159', 'Tree160', 'Tree161', 'Tree162', 'Tree163', 'Tree164', 'Tree165', 'Tree166', 'Tree167', 'Tree168', 'Tree169', 'Tree170', 'Tree171', 'Tree172', 'Tree173', 'Tree174', 'Tree175', 'Tree176', 'Tree177', 'Tree178', 'Tree179', 'Tree180', 'Tree181', 'Tree182', 'Tree183', 'Tree184', 'Tree185', 'Tree186', 'Tree187', 'Tree188', 'Tree189', 'Tree190', 'Tree191', 'Tree192', 'Tree193', 'Tree194', 'Tree195', 'Tree196', 'Tree197', 'Tree198', 'Tree199', 'Tree200', 'Tree201', 'Tree202', 'Tree203', 'Tree204', 'Tree205', 'Tree206', 'Tree207', 'Tree208', 'Tree209', 'Tree210', 'Tree211', 'Tree212', 'Tree213', 'Tree214', 'Tree215', 'Tree216', 'Tree217', 'Tree218', 'Tree219', 'Tree220', 'Tree221', 'Tree222', 'Tree223', 'Tree224', 'Tree225', 'Tree226', 'Tree227', 'Tree228', 'Tree229', 'Tree230', 'Tree231', 'Tree232', 'Tree233', 'Tree234', 'Tree235', 'Tree236', 'Tree237', 'Tree238', 'Tree239', 'Tree240', 'Tree241', 'Tree242', 'Tree243', 'Tree244', 'Tree245', 'Tree246', 'Tree247', 'Tree248', 'Tree249', 'Tree250', 'Tree251', 'Tree252', 'Tree253', 'Tree254', 'Tree255', 'Tree256', 'Tree257', 'Tree258', 'Tree259', 'Tree260', 'Tree261', 'Tree262', 'Tree263', 'Tree264', 'Tree265', 'Tree266', 'Tree267', 'Tree268', 'Tree269', 'Tree270', 'Tree271', 'Tree272', 'Tree273', 'Tree274', 'Tree275', 'Tree276', 'Tree277', 'Tree278', 'Tree279', 'Tree280', 'Tree281', 'Tree282', 'Tree283', 'Tree284', 'Tree285', 'Tree286', 'Tree287', 'Tree288', 'Tree289', 'Tree290', 'Tree291', 'Tree292', 'Tree293', 'Tree294', 'Tree295', 'Tree296', 'Tree297', 'Tree298', 'Tree299', 'Tree300', 'treeID'],
    FieldAssociation='Row Data')

tTKMergeTreeDistanceMatrix1.PathMappingLookahead = 3

# save data
SaveData('Cylinder2D.cdb/dm_la3.csv', proxy=tTKMergeTreeDistanceMatrix1, RowDataArrays=['Tree000', 'Tree001', 'Tree002', 'Tree003', 'Tree004', 'Tree005', 'Tree006', 'Tree007', 'Tree008', 'Tree009', 'Tree010', 'Tree011', 'Tree012', 'Tree013', 'Tree014', 'Tree015', 'Tree016', 'Tree017', 'Tree018', 'Tree019', 'Tree020', 'Tree021', 'Tree022', 'Tree023', 'Tree024', 'Tree025', 'Tree026', 'Tree027', 'Tree028', 'Tree029', 'Tree030', 'Tree031', 'Tree032', 'Tree033', 'Tree034', 'Tree035', 'Tree036', 'Tree037', 'Tree038', 'Tree039', 'Tree040', 'Tree041', 'Tree042', 'Tree043', 'Tree044', 'Tree045', 'Tree046', 'Tree047', 'Tree048', 'Tree049', 'Tree050', 'Tree051', 'Tree052', 'Tree053', 'Tree054', 'Tree055', 'Tree056', 'Tree057', 'Tree058', 'Tree059', 'Tree060', 'Tree061', 'Tree062', 'Tree063', 'Tree064', 'Tree065', 'Tree066', 'Tree067', 'Tree068', 'Tree069', 'Tree070', 'Tree071', 'Tree072', 'Tree073', 'Tree074', 'Tree075', 'Tree076', 'Tree077', 'Tree078', 'Tree079', 'Tree080', 'Tree081', 'Tree082', 'Tree083', 'Tree084', 'Tree085', 'Tree086', 'Tree087', 'Tree088', 'Tree089', 'Tree090', 'Tree091', 'Tree092', 'Tree093', 'Tree094', 'Tree095', 'Tree096', 'Tree097', 'Tree098', 'Tree099', 'Tree100', 'Tree101', 'Tree102', 'Tree103', 'Tree104', 'Tree105', 'Tree106', 'Tree107', 'Tree108', 'Tree109', 'Tree110', 'Tree111', 'Tree112', 'Tree113', 'Tree114', 'Tree115', 'Tree116', 'Tree117', 'Tree118', 'Tree119', 'Tree120', 'Tree121', 'Tree122', 'Tree123', 'Tree124', 'Tree125', 'Tree126', 'Tree127', 'Tree128', 'Tree129', 'Tree130', 'Tree131', 'Tree132', 'Tree133', 'Tree134', 'Tree135', 'Tree136', 'Tree137', 'Tree138', 'Tree139', 'Tree140', 'Tree141', 'Tree142', 'Tree143', 'Tree144', 'Tree145', 'Tree146', 'Tree147', 'Tree148', 'Tree149', 'Tree150', 'Tree151', 'Tree152', 'Tree153', 'Tree154', 'Tree155', 'Tree156', 'Tree157', 'Tree158', 'Tree159', 'Tree160', 'Tree161', 'Tree162', 'Tree163', 'Tree164', 'Tree165', 'Tree166', 'Tree167', 'Tree168', 'Tree169', 'Tree170', 'Tree171', 'Tree172', 'Tree173', 'Tree174', 'Tree175', 'Tree176', 'Tree177', 'Tree178', 'Tree179', 'Tree180', 'Tree181', 'Tree182', 'Tree183', 'Tree184', 'Tree185', 'Tree186', 'Tree187', 'Tree188', 'Tree189', 'Tree190', 'Tree191', 'Tree192', 'Tree193', 'Tree194', 'Tree195', 'Tree196', 'Tree197', 'Tree198', 'Tree199', 'Tree200', 'Tree201', 'Tree202', 'Tree203', 'Tree204', 'Tree205', 'Tree206', 'Tree207', 'Tree208', 'Tree209', 'Tree210', 'Tree211', 'Tree212', 'Tree213', 'Tree214', 'Tree215', 'Tree216', 'Tree217', 'Tree218', 'Tree219', 'Tree220', 'Tree221', 'Tree222', 'Tree223', 'Tree224', 'Tree225', 'Tree226', 'Tree227', 'Tree228', 'Tree229', 'Tree230', 'Tree231', 'Tree232', 'Tree233', 'Tree234', 'Tree235', 'Tree236', 'Tree237', 'Tree238', 'Tree239', 'Tree240', 'Tree241', 'Tree242', 'Tree243', 'Tree244', 'Tree245', 'Tree246', 'Tree247', 'Tree248', 'Tree249', 'Tree250', 'Tree251', 'Tree252', 'Tree253', 'Tree254', 'Tree255', 'Tree256', 'Tree257', 'Tree258', 'Tree259', 'Tree260', 'Tree261', 'Tree262', 'Tree263', 'Tree264', 'Tree265', 'Tree266', 'Tree267', 'Tree268', 'Tree269', 'Tree270', 'Tree271', 'Tree272', 'Tree273', 'Tree274', 'Tree275', 'Tree276', 'Tree277', 'Tree278', 'Tree279', 'Tree280', 'Tree281', 'Tree282', 'Tree283', 'Tree284', 'Tree285', 'Tree286', 'Tree287', 'Tree288', 'Tree289', 'Tree290', 'Tree291', 'Tree292', 'Tree293', 'Tree294', 'Tree295', 'Tree296', 'Tree297', 'Tree298', 'Tree299', 'Tree300', 'treeID'],
    FieldAssociation='Row Data')
    
   
matrix0 = np.loadtxt(open("Cylinder2D.cdb/dm_la0.csv", "rb"), delimiter=",", skiprows=1,usecols=range(0,300))
matrix1 = np.loadtxt(open("Cylinder2D.cdb/dm_la1.csv", "rb"), delimiter=",", skiprows=1,usecols=range(0,300))
matrix2 = np.loadtxt(open("Cylinder2D.cdb/dm_la2.csv", "rb"), delimiter=",", skiprows=1,usecols=range(0,300))
matrix3 = np.loadtxt(open("Cylinder2D.cdb/dm_la3.csv", "rb"), delimiter=",", skiprows=1,usecols=range(0,300))

embedding0 = TSNE(n_components=2).fit_transform(matrix0)
embedding1 = TSNE(n_components=2).fit_transform(matrix1)
embedding2 = TSNE(n_components=2).fit_transform(matrix2)
embedding3 = TSNE(n_components=2).fit_transform(matrix3)

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

fig, ax = plt.subplots()
ax.imshow(matrix2, cmap='coolwarm', interpolation='nearest')
ax.set_xticks(np.arange(0,301,25))
ax.set_yticks(np.arange(0,301,25))
fig.savefig("dm_la3.pdf", transparent=True, bbox_inches="tight", pad_inches=0.1)

colarr = range(301)

fig, ax = plt.subplots()
im = ax.scatter([x[0] for x in embedding0],[x[1] for x in embedding0], label="PMD0", s=4, c=colarr)
ax.set_xticks([])
ax.set_yticks([])
fig.set_size_inches(3,3)
fig.patch.set_alpha(0.5)
ax.axes.patch.set_alpha(0.5)
plt.setp(ax.spines.values(), linewidth=3)
fig.savefig("embedding_la0.pdf", bbox_inches="tight")

fig, ax = plt.subplots()
im = ax.scatter([x[0] for x in embedding1],[x[1] for x in embedding1], label="PMD1", s=4, c=colarr)
ax.set_xticks([])
ax.set_yticks([])
fig.set_size_inches(3,3)
fig.patch.set_alpha(0.5)
ax.axes.patch.set_alpha(0.5)
plt.setp(ax.spines.values(), linewidth=3)
fig.savefig("embedding_la1.pdf", bbox_inches="tight")

fig, ax = plt.subplots()
im = ax.scatter([x[0] for x in embedding2],[x[1] for x in embedding2], label="PMD2", s=4, c=colarr)
ax.set_xticks([])
ax.set_yticks([])
fig.set_size_inches(3,3)
fig.patch.set_alpha(0.5)
ax.axes.patch.set_alpha(0.5)
plt.setp(ax.spines.values(), linewidth=3)
fig.savefig("embedding_la2.pdf", bbox_inches="tight")

fig, ax = plt.subplots()
im = ax.scatter([x[0] for x in embedding2],[x[1] for x in embedding2], label="PMD3", s=4, c=colarr)
ax.set_xticks([])
ax.set_yticks([])
fig.set_size_inches(3,3)
fig.patch.set_alpha(0.5)
ax.axes.patch.set_alpha(0.5)
plt.setp(ax.spines.values(), linewidth=3)
fig.savefig("embedding_la3.pdf", bbox_inches="tight")
 


if __name__ == '__main__':
    # generate extracts
    SaveExtracts(ExtractsOutputDirectory='extracts')

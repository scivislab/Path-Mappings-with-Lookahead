# Path Mappings with Lookahead

This repository contains the source code for the paper presented PacificVis 2025:

"Accelerating Computation of Stable Merge Tree Edit Distances using Parameterized Heuristics"
Florian Wetzels, Heike Leitte and Christoph Garth
IEEE Transactions on Visualization and Computer Graphics, 2025.

[Link to paper](https://arxiv.org/pdf/2307.02509.pdf)

The section below contains instructions on how to compile and run the code on a vanilla Ubuntu 22.04 to reproduce the teaser figure of the paper.

## Installation Note

Tested on Ubuntu 22.04.3 LTS.

### Install the dependencies

```bash
sudo apt-get install cmake-qt-gui libboost-system-dev libpython3.10-dev libxt-dev libxcursor-dev libopengl-dev
sudo apt-get install qttools5-dev libqt5x11extras5-dev libqt5svg5-dev qtxmlpatterns5-dev-tools 
sudo apt-get install python3-sklearn 
sudo apt-get install libsqlite3-dev 
sudo apt-get install gawk
sudo apt-get install git p7zip-full wget
```

### Install Paraview

First, go in the root of this repository and run the following commands:
(replace the `4` in `make -j4` by the number of available cores on your system)

```bash
git clone https://github.com/topology-tool-kit/ttk-paraview.git
cd ttk-paraview
git checkout 5.10.1
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DPARAVIEW_USE_PYTHON=ON -DPARAVIEW_INSTALL_DEVELOPMENT_FILES=ON -DCMAKE_INSTALL_PREFIX=../install ..
make -j4
make -j4 install
```

Some warnings are expected when using the `make` command, they should not cause any problems.

Stay in the build directory and set the environment variables:
(replace `3.10` in `python3.10` by your version of python)

```bash
PV_PREFIX=`pwd`/../install
export PATH=$PATH:$PV_PREFIX/bin
export LD_LIBRARY_PATH=$PV_PREFIX/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PV_PREFIX/lib/python3.10/site-packages
```

### Install TTK

Go in the `ttk-lookahead` directory then run the following commands:
(replace the `4` in `make -j4` by the number of available cores on your system)

```bash
mkdir build && cd build
paraviewPath=`pwd`/../../ttk-paraview/install/lib/cmake/paraview-5.10
cmake -DCMAKE_INSTALL_PREFIX=../install -DParaView_DIR=$paraviewPath ..
make -j4
make -j4 install
```

Stay in the build directory and set the environment variables:
(replace `3.10` in `python3.10` by your version of python)

```bash
TTK_PREFIX=`pwd`/../install
export PV_PLUGIN_PATH=$TTK_PREFIX/bin/plugins/TopologyToolKit
export LD_LIBRARY_PATH=$TTK_PREFIX/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH:$TTK_PREFIX/lib/python3.10/site-packages
```

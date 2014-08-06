SchafKopf
========

## Build

1. Clone this repository

   ```bash
   git clone git://github.com/benni0815/SchafKopf.git
   ```

2. Install dependencies. In (K)Ubuntu run:

   ```bash
   sudo apt-get install qt4-qmake libkdegames-dev kdegames-card-data-extra libqimageblitz-dev
   ```

3. Create a build directory and run CMake
   ```bash
   cd SchafKopf
   mkdir build
   cd build
   cmake .. -DQT_QMAKE_EXECUTABLE=/usr/bin/qmake-qt4 -DINSTALL_OPEN_TAROCK=ON
   ```

3. Start the compilation

   ```bash
   cd ..
   make
   ```

4. Build a .deb package and install it

   ```bash
   make package
   sudo dpkg -i schafkopf-0.7.91-Linux.deb
   ```

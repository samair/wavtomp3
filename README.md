# wavtomp3
Lame based wav to mp3 converter in c++
# How to use
It uses cmake for cross platform availability

Steps:
1. Run cmake (in the directory which has CMake file)
2. make ( to create executable)
3. Run 
   ./mp3coverter __pathtofolder__
 
 - It converts wav files to mp3 in all folders (recursively too)
 - Implementation uses pthread(for threading) and uses a threadPool implemented (threadPool.h)
 - Lame libraries are linked statically, CMakeLists.txt has OS differentiation to achive the same for GNU based builds (windows and linux)
    For linux it links to libmp3lame.a
    For Windows it links to pthreadGC2.dll (GNU based implementation using MingW)

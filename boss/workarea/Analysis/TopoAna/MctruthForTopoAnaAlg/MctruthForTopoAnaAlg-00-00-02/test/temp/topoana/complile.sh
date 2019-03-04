#!/bin/bash
cxxfile="topoana.cpp"
exefile="topoana.exe"
ROOTCFLAGS=`root-config --cflags`
ROOTLDFLAGS=`root-config --ldflags --glibs`
g++ -g -Wall ${ROOTCFLAGS} ${ROOTLDFLAGS} -o ${exefile} ${cxxfile}
exit 0

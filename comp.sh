#how to compile EnergyCal.cpp:
g++ -fPIC `root-config --cflags` -c EnergyCal.cpp 
rootcint -f EnergyCal_dict.cpp -c EnergyCal.h
g++ -fPIC `root-config --cflags` -c EnergyCal_dict.cpp 
g++ -shared -g EnergyCal.o EnergyCal_dict.o -o libEnergyCal.so
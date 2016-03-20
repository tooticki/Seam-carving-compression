all: compress

compress: compression.cpp  bmpwrapper.cpp EasyBMP.cpp 
	g++ -g -Wall -o  compress ./compression.cpp ./bmpwrapper.cpp ./EasyBMP.cpp

clean: 
	rm -rf *.o compress

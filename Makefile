all: compress

compress: compression.cpp  bmpwrapper.cpp EasyBMP.cpp 
	g++ -o  compress ./compression.cpp ./bmpwrapper.cpp ./EasyBMP.cpp

clean: 
	rm -rf *.o compress
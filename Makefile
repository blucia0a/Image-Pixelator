all:
	g++ `pkg-config opencv --cflags --libs` imgPix.cpp -o imgPix -g -O0

CXX ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra -std=c++17

.PHONY: all windows clean

all: parcel_system

# The project includes module .cpp files from main.cpp, so compile this entry point only.
parcel_system: main.cpp
	$(CXX) $(CXXFLAGS) -I. main.cpp -o parcel_system

windows:
	x86_64-w64-mingw32-g++ -O3 -Wall -Wextra -std=c++17 -I. main.cpp -static -static-libgcc -static-libstdc++ -o main.exe

clean:
	rm -f parcel_system main.exe

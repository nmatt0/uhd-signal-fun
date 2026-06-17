
signal-rx1: signal-rx1.cpp
	g++ -std=c++23 -Wall -Wextra signal-rx1.cpp -luhd -o signal-rx1

all: signal-rx1

clean:
	rm -f hello signal-rx1

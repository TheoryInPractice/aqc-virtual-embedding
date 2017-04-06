VALGRIND = valgrind --leak-check=full --track-origins=yes
BUILD_COMMANDS = g++ -std=c++11 -O2 embedding/*/src/*.cpp\
                 embedding/driver.cpp -o embedding/driver
build:
	clear
	@echo "Creating initial folders"
	-mkdir data/input
	-mkdir data/output
	@echo "Compiling C++ code"
	$(BUILD_COMMANDS)
	@echo "Build completed!"

clean:
	clear
	@echo "Deleting input data"
	-rm -r data/input/*
	@echo "Deleting output data"
	-rm -r data/output/*
	@echo "Data clean completed!"

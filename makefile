all:
	g++ PartA/timekeeper.cpp -o PartA/timekeeper -lpthread 
	g++ PartA/generator.cpp PartA/network.cpp PartA/setEnvironment.cpp -o PartA/generator -lpthread -std=c++0x
	g++ -w PartA/main.cpp -o PartA/main
	g++ PartB/analyzer.cpp PartB/network.cpp PartB/algorithms.cpp -lpthread -o PartB/analyzer
	chmod 755 run.sh

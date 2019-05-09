.PHONY: testReflex

testReflex:
	clang++ testReflex.cpp -o testReflex -Wall -std=c++11 -g
	
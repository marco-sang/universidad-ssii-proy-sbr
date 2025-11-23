SBR: SBR.cpp main.cpp SBR.h motorDeInferencia.cpp
	g++ -g -Wall main.cpp SBR.cpp motorDeInferencia.cpp -o SBR

debug:
	gdb ./SBR

PHONY: debug


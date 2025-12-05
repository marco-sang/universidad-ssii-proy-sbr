SBR-FC: SBR.cpp main.cpp SBR.h motorDeInferencia.cpp utils.cpp
	g++ -g -Wall main.cpp SBR.cpp motorDeInferencia.cpp utils.cpp -o SBR-FC

debug:
	gdb ./SBR

PHONY: debug


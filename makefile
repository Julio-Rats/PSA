all:
	g++ -c PSA/*.hpp
	g++ -c Simulador\ CANbus/*.hpp
	g++ PSA/*.cpp Simulador\ CANbus/*.cpp -o Pareto_SA.bin -lm

#Intruction to compile the source files
CS321_Project: main.c audio_functions.c
	gcc main.c audio_functions.c -o CS321_Project -lsndfile -lm

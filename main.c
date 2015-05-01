/********************************************************
*       Music Programming I - Project Assignment        *
*                                                       *
* Created by Richard Guerci on 07/12/13.                *
* Copyright © 2013 Richard Guerci. All rights reserved. *
* see HELP file for more informations                   *
********************************************************/

#include "audio_functions.h" // My own audio library : sndFileToASCII(...), ...


// Main function
int main(int argc, char *argv[]){
	// Variables
	char c; // will contain the character of the option
	extern char *optarg;	// will contain the parameter just after the '-*' option
							// initialized by 'getopt'
	char command_play[100] = "sndfile-play "; // begining of the command to play a sound file
	char command_info[100] = "sndfile-info "; // begining of the command to get info of a sound file
	char *input_filename = NULL;
	char *output_filename = NULL;
	char *song_string = NULL;
	//flags : boolean (0 or 1)
	//flags allow to know which option is wanted
	int aflag = 0; // 1 if -a (snd to ascii option)
	int cflag = 0; // 1 if -a (snd to ascii option)
	int eflag = 0; // 1 if -e (effect option)
	int hflag = 0; // 1 if -h (help option)
	int iflag = 0; // 1 if -i (input file option)
	int Iflag = 0; // 1 if -I (info option)
	int oflag = 0; // 1 if -o (output file option)
	int pflag = 0; // 1 if -p (play option)
	int errorflag = 0;
	//Effect number (argument after -e)
	int effect_number = 0;

	// Check if there is an argument
	if(argc<2){
		printf("Error : no option\nmore information : %s -h\n", argv[0]); 
		exit(-1);//quit program
	}

	// Check if the first argument is valid (must be an option : start with '-')
	if(argv[1][0]!='-'){
		printf("Error : invalid option\nmore information : %s -h\n", argv[0]); 
		exit(-1);//quit program
	}

	// getopt(...) allow to fetch all the parameters in argv[]
	while ((c = getopt(argc , argv, "a:c:e:hi:I:o:p:")) != -1){ // While there is a parametre
		switch (c) {
			case 'a': // Render Audio file waveforms in ASCII
				input_filename = (char *) malloc(sizeof(char)*(strlen(optarg)+1));
				strcpy(input_filename,optarg);
				aflag = 1;
				break;
			case 'c': // Create music
				song_string = (char *) malloc(sizeof(char)*(strlen(optarg)+1));
				strcpy(song_string,optarg);
				cflag = 1;
				break;
			case 'e': // Select effect
				effect_number = atoi(optarg);
				if(effect_number<1||effect_number>4){
					fprintf (stderr, "Unknown effect : %d (more help : %s -h)\n", effect_number, argv[0]);
					errorflag = -1; // a problem was encountered
				}
				else{//effect ok
					eflag = 1;
				}
				break;
			case 'h': // Show help file
				hflag = 1;
				break;
			case 'i': // input filename
				//saving file name in a variable
				input_filename = (char *) malloc(sizeof(char)*(strlen(optarg)+1));
				strcpy(input_filename,optarg);
				iflag = 1;
				break;
			case 'I': // Get info
				strcat(command_info,optarg);	//concatenate command_play and the sound file name (optarg)
				Iflag = 1;
				break;
			case 'o': // output filename
				//saving file name in a variable
				output_filename = (char *) malloc(sizeof(char)*(strlen(optarg)+1));
				strcpy(output_filename,optarg);
				oflag = 1;
				break;
			case 'p': // Play a sound file
				strcat(command_play,optarg);	//concatenate command_play and the sound file name (optarg)
				pflag = 1;
				break;
			case '?': // Error case
				if (optopt=='a'||optopt=='c'||optopt=='e'||optopt=='i'||optopt=='I'||optopt=='o'||optopt=='p') fprintf (stderr, "This option -%c need an input sound file ", optopt);
				else fprintf (stderr, "Unknown option [-%c] ", optopt);
				fprintf (stderr, "(more help : %s -h)\n", argv[0]);
				//Free the dynamic memory
				if(input_filename!=NULL) free(input_filename);
				if(output_filename!=NULL) free(output_filename);
				if(song_string!=NULL) free(song_string);
				exit(-1); // a problem was encountered
				break;
		}
	}

	//Continue if there is no error
	if(errorflag==0){
		// These conditions avoid the user to do unwanted things.
		// for example : If he runs the program with option -h and -a <file>, the program will just return the help file.
		if(hflag){ // Render Audio file waveforms in ASCII
			system("cat HELP");			//system(...) allow to call a command from the console
		}
		else if(Iflag){ // Get info
			system(command_info); //system(...) allow to call a command from the console
		}
		else if(pflag){ // Play a sound file
			system(command_play); //system(...) allow to call a command from the console
		}
		else if(aflag){ // Render Audio file waveforms in ASCII
			errorflag = sndFileToASCII(input_filename);
		}
		else if(cflag){ // Create music
			errorflag = createSound(output_filename, song_string);
		}
		else if(eflag){ // Effect
			if(iflag){
				errorflag = applyEffect(input_filename, output_filename, effect_number);
			}
			else{ //no input file
				fprintf(stderr, "Error : no input file to apply an effect\n");
			}
		}
		else{
			printf("Usage error\nmore information : %s -h\n", argv[0]); 
		}
	}

	//Free the dynamic memory
	if(input_filename!=NULL) free(input_filename);
	if(output_filename!=NULL) free(output_filename);
	if(song_string!=NULL) free(song_string);

	return errorflag; // return 0 if success, else -1
}



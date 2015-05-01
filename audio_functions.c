/********************************************************
*       Music Programming I - Project Assignment        *
*                                                       *
* Created by Richard Guerci on 07/12/13.                *
* Copyright © 2013 Richard Guerci. All rights reserved. *
* see HELP file for more informations                   *
********************************************************/

#include "audio_functions.h" //header of this source file

#define BUFFER_LEN 128 // length of the buffer used to hold samples

#ifndef M_PI
#define M_PI 3.14159265358979323846264338 // PI
#endif

// Constant used in createSound(...)
#define SAMPLE_RATE 48000
#define LONG_NOTE_LENGHT (SAMPLE_RATE / 2)
#define SHORT_NOTE_LENGHT (SAMPLE_RATE / 4)
#define BREAK_LENGHT (SAMPLE_RATE / 32)
#define AMPLITUDE (1.0 * 0x7F000000)
#define A_FREQUENCY (440.0 / SAMPLE_RATE)
#define B_FREQUENCY (493.9 / SAMPLE_RATE)
#define C_FREQUENCY (261.6 / SAMPLE_RATE)
#define D_FREQUENCY (293.7 / SAMPLE_RATE)
#define E_FREQUENCY (329.6 / SAMPLE_RATE)
#define F_FREQUENCY (349.2 / SAMPLE_RATE)
#define G_FREQUENCY (392.0 / SAMPLE_RATE)



// Prints Audio file waveforms in ASCII of the sound file
int sndFileToASCII(char *filename){
	// Declare variables
	int	read_count; //Count how many sample has been read with sf_read_float(...)
	int samples_count = 0;	// Total of samples
							// Allow to calculate the time while printing ASCII waveforms
	int number_of_line = 21;// Number of line for the ASCII waveforms
	// Sound file pointer
	SNDFILE *in_snd_file;
	// Sound file infomation
	SF_INFO snd_file_info;
	// buffer
	float *buffer;


	// Open the input sound file
	if(!(in_snd_file=sf_open(filename, SFM_READ, &snd_file_info))){ 
		fprintf(stderr, "Error : fail to open input file %s\n", filename); 
		return -1;  // a problem was encountered
	}

	printf("Waveforms in ASCII of %s file :\n", filename);

	// Memory allocation of buffer
	// The lenght of the buffer is also the lenght of each line of the waveform
	// More there are channels, less there are frames, so we need to take a longer buffer depending on the number of channels
	buffer = (float *) malloc(sizeof(float)*(BUFFER_LEN*snd_file_info.channels));

	// Read frames from input file and print the waveforms in ASCII
	// BUFFER_LEN is also the lenght of a line of waveforms in ASCII
	while ((read_count = sf_read_float(in_snd_file, buffer, BUFFER_LEN*snd_file_info.channels))){
		int i,j,k;

		// Sum of samples already read
		samples_count+=read_count;
		
		// We print the 1st channel first, then the 2nd, ...
		for (j = 0; j < snd_file_info.channels; j++){
			// Print channel number and audio position (time)
			printf("\nChannel %d\t\t\t\t\t\t\t\t\t\t\t\tAudio position : %.3f s\n", j+1, 1.0/(float)snd_file_info.samplerate*(float)samples_count/(float)snd_file_info.channels);
			for(k=0;k<number_of_line;k++){//For each lines
				for (i = 0; i < read_count; i++){//For each samples of buffer
					// Selecting the sample which correspond with channel j
					if((i%snd_file_info.channels)==(j)){
						// For a given line k : if amplitude_of_bottom_line(k)) < buffer[i] <= amplitude_of_top_line(k) then write('+') else write('-')
						if( (buffer[i]+1.0)>=(2.0/(float)number_of_line*(float)(number_of_line-k-1)) && (buffer[i]+1.0)<(2.0/(float)number_of_line*(float)(number_of_line-k)) ) printf("+");
						else printf("-");
					}
				}
				printf("\n");
			}
		}
		printf("\n\n");
	}

	// Close file
	sf_close(in_snd_file);

	// Free dynamic memory
	free(buffer);

	return 0; // Return 0 for success
}

// Apply Effect function
int applyEffect(char *input_filename, char *output_filename, int effect){
	// Declare variables
	int	read_count; //Count how many sample has been read with sf_read_float(...)
	int samples_count = 0;	// Total of samples read
	int frames; //total frames
	int c = 0; //
	// Sound file pointers
	SNDFILE *in_snd_file, *out_snd_file;
	// Sound file infomation
	SF_INFO snd_file_info;
	// buffer
	float buffer[BUFFER_LEN];
	float *buffer_echo;// copy of buffer in order to apply an echo effect
	//output filename (we need tu consider the case when the user don't use -o option : no output filename)
	char new_output_filename[100];

	// Open the input sound file
	if(!(in_snd_file=sf_open(input_filename, SFM_READ, &snd_file_info))){ 
		fprintf(stderr, "Error : fail to open input file %s\n", input_filename); 
		return -1;  // a problem was encountered
	}

	//save number of frames (used with fade out effect)
	frames = snd_file_info.frames;

	// Create output file name (if not exist)
	if(output_filename==NULL){
		if(effect==1) strcpy(new_output_filename,"fade_in_");
		else if(effect==2) strcpy(new_output_filename,"fade_out_");
		else if(effect==3) strcpy(new_output_filename,"inverse_");
		else if(effect==4) strcpy(new_output_filename,"echo_");
		strcat(new_output_filename, input_filename);
	}
	else{
		strcpy(new_output_filename,output_filename);
	}

	// Open the output sound file
	if(!(out_snd_file=sf_open(new_output_filename, SFM_WRITE, &snd_file_info))){ 
		printf("Error : fail to open output file %s\n", new_output_filename); 
		sf_close(in_snd_file);
		exit(-1); 
	}

	// Memory allocation of buffer_echo (lenght is samplerate because we want an echo after 1 second)
	if (effect == 4) buffer_echo = (float *) malloc(sizeof(float)*(snd_file_info.samplerate*snd_file_info.channels));

	//Print what's happening
	printf("Applying ");
	if(effect == 1) printf("Fade In");
	else if(effect == 2) printf("Fade Out");
	else if(effect == 3) printf("Inverse");
	else if(effect == 4) printf("Echo");
	printf(" effect to %s...\n", input_filename);

	// Read frames from input file, process them and write them to the output file.
	while ((read_count = sf_read_float(in_snd_file, buffer, BUFFER_LEN))){
		int i;

		if(effect==1){// Fade In (first 3 seconds)
			for (i = 0; i < read_count; i++){
				if((1.0/(float)snd_file_info.samplerate*(float)(samples_count+i)/(float)snd_file_info.channels)<3.0){ // time < 3
					buffer[i]=buffer[i]*((float)((samples_count+i)/(float)snd_file_info.channels)/(3.0*(float)snd_file_info.samplerate));
				}
			}
		}
		else if(effect==2){// Fade Out (last 3 seconds)
			for (i = 0; i < read_count; i++){
				if((samples_count+i)>=(frames*snd_file_info.channels-3*snd_file_info.samplerate*snd_file_info.channels)){ // frame in the last 3 seconds
					float x = (float)(samples_count+i-(frames*snd_file_info.channels))/((float)3*snd_file_info.samplerate*snd_file_info.channels);
					if (x < 0) x = -x;
					buffer[i]=buffer[i]*x;
				}
			}
		}
		else if(effect==3){// Inverse
			for (i = 0; i < read_count; i++){
				buffer[i]=-buffer[i];
			}
		}
		else if(effect==4){// Echo (1 second delay; decay : 0.5)
			for (i = 0; i < read_count; i++){
				//save current sample in buffer_echo
				buffer_echo[(samples_count+i)%(snd_file_info.samplerate*snd_file_info.channels)]=buffer[i];
				if((samples_count+i)/snd_file_info.channels>snd_file_info.samplerate-snd_file_info.channels){ // One second of delay
					buffer[i]=buffer[i]+0.5*buffer_echo[(c)%(snd_file_info.samplerate*snd_file_info.channels)];
					c++;
				}
			}
		}

		// Sum of samples already read
		samples_count+=read_count;

		// Writing buffer into out sound file
		sf_write_float(out_snd_file, buffer, read_count);
	}

	// Close files
	sf_close(in_snd_file);
	sf_close(out_snd_file);

	// Free dynamic memory
	if (effect == 4) free(buffer_echo);

	// Print output file is ready
	printf("Done. %s has been successfully created\n", new_output_filename);

	return 0; // Return 0 for success
}

// This function allow to create a sound from a string (notes A, B, C, D, E, F and G) using sine function
// Each note is 1/2 second long (upper case) and 1/4 second long (lowervase); between 2 notes : 1/32 second long
// This function was inspired by make_sine.c, an example from libsndfile-1.0.25
int createSound(char *output_filename, char *song_string){
	// Declare variables
	int i,j;
	float freq; // frequency of the note
	int frames = 0; // number of frames for the full song;
	// Sound file pointer
	SNDFILE *out_snd_file;
	// Sound file infomation
	SF_INFO snd_file_info;
	// buffer
	int buffer[LONG_NOTE_LENGHT+BREAK_LENGHT]; // lenght of the buffer (longest note lenght + break lenght)
	// output filename (we need tu consider the case when the user don't use -o option : no output filename)
	char new_output_filename[100];

	// Create output file name (if not exist)
	if(output_filename==NULL){
		strcpy(new_output_filename,"my_song.wav");
	}
	else{
		strcpy(new_output_filename,output_filename);
	}

	// Counting number of frames in total
	for (i = 0; i < strlen(song_string); i++){
		char note = song_string[i]; // current note
		if(isLongNote(note)) frames += LONG_NOTE_LENGHT; // long note lenght
		else if(isShortNote(note)) frames += SHORT_NOTE_LENGHT; // short note lenght
		else frames += LONG_NOTE_LENGHT; // blank note lenght (other letters...)

		if (i!=strlen(song_string)-1) frames += BREAK_LENGHT; //if it's not the last note we add a break
	}

	// Initialize sound file info structure
	memset (&snd_file_info, 0, sizeof (snd_file_info)) ;
	snd_file_info.samplerate	= SAMPLE_RATE ;
	snd_file_info.frames		= frames;
	snd_file_info.channels		= 1;
	snd_file_info.format		= (SF_FORMAT_WAV | SF_FORMAT_PCM_24) ;

	// Open the output sound file
	if(!(out_snd_file=sf_open(new_output_filename, SFM_WRITE, &snd_file_info))){ 
		printf("Error : fail to open output file %s\n", new_output_filename); 
		exit(-1); 
	}

	// Notes creation...
	printf("Creating %s...\n", new_output_filename);
	for (i = 0; i < strlen(song_string); i++){
		char note = song_string[i]; // current note
		int lenght = 0; // lenght of the note (and the break if it's not the last note)
		
		// Lenght needed by the buffer
		if(isLongNote(note)) lenght += LONG_NOTE_LENGHT; // long note lenght
		else if(isShortNote(note)) lenght += SHORT_NOTE_LENGHT; // short note lenght
		else lenght += LONG_NOTE_LENGHT; // blank note lenght (other letters...)

		// Initialize buffer (note)
		for (j = 0; j < lenght; j++){
			buffer[j] = AMPLITUDE * sin(getNoteFrequency(note) * j * 2 * M_PI); //note
		}
		// Initialize buffer (break) if it's not the last note
		if(i!=strlen(song_string)-1){
			for (j = 0; j < BREAK_LENGHT; j++){
				buffer[lenght+j] = 0; //break
			}
			lenght += BREAK_LENGHT;
		}

		// Writing buffer into out sound file
		sf_write_int(out_snd_file, buffer, lenght);
	}

	// Close files
	sf_close(out_snd_file);

	// Print output file is ready
	printf("Done. %s has been successfully created\n", new_output_filename);

	return 0; // Return 0 for success
}

// Return the frequency of the given note (A, B, ..., G, a, b, ..., g)
float getNoteFrequency(char note){
	switch(note){
		case 'A':
			return A_FREQUENCY;
			break;
		case 'B':
			return B_FREQUENCY;
			break;
		case 'C':
			return C_FREQUENCY;
			break;
		case 'D':
			return D_FREQUENCY;
			break;
		case 'E':
			return E_FREQUENCY;
			break;
		case 'F':
			return F_FREQUENCY;
			break;
		case 'G':
			return G_FREQUENCY;
			break;
		case 'a':
			return A_FREQUENCY;
			break;
		case 'b':
			return B_FREQUENCY;
			break;
		case 'c':
			return C_FREQUENCY;
			break;
		case 'd':
			return D_FREQUENCY;
			break;
		case 'e':
			return E_FREQUENCY;
			break;
		case 'f':
			return F_FREQUENCY;
			break;
		case 'g':
			return G_FREQUENCY;
			break;
		default:
			return 0;
			break;
	}
}

// Return 1 of n is a long note (A, B, ..., G), else 0
int isLongNote(int note){
	return note=='A'||note=='B'||note=='C'||note=='D'||note=='E'||note=='F'||note=='G';
}

// Return 1 of n is a long note (a, b, ..., g), else 0
int isShortNote(int note){
	return note=='a'||note=='b'||note=='c'||note=='d'||note=='e'||note=='f'||note=='g';
}



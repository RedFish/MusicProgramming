/********************************************************
*       Music Programming I - Project Assignment        *
*                                                       *
* Created by Richard Guerci on 07/12/13.                *
* Copyright © 2013 Richard Guerci. All rights reserved. *
* see HELP file for more informations                   *
********************************************************/

#ifndef AUDIO_FUNCTION_H // guards to avoid multiple definitions of functions
#define AUDIO_FUNCTION_H

//Libararies
#include <stdio.h> // I/O : printf(...), scanf(...), ...
#include <stdlib.h> // malloc(...), free(...), exit(...), ...
#include <unistd.h> // getopt(...), ...
#include <string.h> // String library : strcat(...), strcpy(...), ...
#include <sndfile.h> // libsndfile library : sf_open(...), sf_read_float(...), ...
#include <math.h> // Math library : sin(...), ...

// Prints Audio file waveforms in ASCII of the sound file
int sndFileToASCII(char *filename);

// Apply Effect function
int applyEffect(char *input_filename, char *output_filename, int effect);

// This function allow to create a sound from a string (notes A, B, C, D, E, F and G) using sine function
// Each note is 1/2 second long (upper case) and 1/4 second long (lowervase); between 2 notes : 1/32 second long
// This function was inspired by make_sine.c, an example from libsndfile-1.0.25
int createSound(char *output_filename, char *song_string);

// Return the frequency of the given note (A, B, ..., G, a, b, ..., g)
float getNoteFrequency(char note);

// Return 1 of n is a long note (A, B, ..., G), else 0
int isLongNote(int note);

// Return 1 of n is a long note (a, b, ..., g), else 0
int isShortNote(int note);

#endif

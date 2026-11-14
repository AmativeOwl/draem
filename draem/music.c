#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>
#include "music.h"

#define NUM_NOTES 12

void init_music(void)
{
    srand(time(NULL));
}

#define PROG_LEN 4

static Progression build_progression(char *root_key, const ChordTemplate *t); // defined further down

// one progression per base quality (MAJOR..SUSP), same chords as the original switch
static const ChordTemplate QUALITY_TEMPLATES[][PROG_LEN] = {
    [MAJOR] = {{2, MINOR, 7}, {7, DOM, 9}, {0, MAJOR, 7}, {0, MAJOR, 9}}, // ii - V - I - I
    [MINOR] = {{9, MINOR, 9}, {5, MAJOR, 7}, {0, MAJOR, 7}, {7, DOM, 9}}, // vi - IV - I - V
    [DOM]   = {{0, MAJOR, 7}, {9, MINOR, 9}, {2, MINOR, 7}, {7, DOM, 9}}, // I - vi - ii - V
    [SUSP]  = {{9, MINOR, 9}, {2, MINOR, 7}, {7, DOM, 9}, {0, MAJOR, 7}}, // vi - ii - V - I
};

Progression generate_progression(char *root_key, Quality base_quality)
{
    Progression empty = {NULL, 0};

    if (map_note_to_num(root_key) == -1)
        return empty; // invalid root note

    if (base_quality < MAJOR || base_quality > SUSP)
        return empty; // only the four base qualities have templates

    return build_progression(root_key, QUALITY_TEMPLATES[base_quality]);
}

int map_note_to_num(char *note)
{
    int num = -1; // handling invalid input which defaults to -1 (not a note)
    if (strcmp(note, "C") == 0 || strcmp(note, "c") == 0)
        num = 0;
    else if (strcmp(note, "C#") == 0 || strcmp(note, "Db") == 0 || strcmp(note, "c#") == 0 || strcmp(note, "db") == 0)
        num = 1;
    else if (strcmp(note, "D") == 0 || strcmp(note, "d") == 0)
        num = 2;
    else if (strcmp(note, "D#") == 0 || strcmp(note, "Eb") == 0 || strcmp(note, "d#") == 0 || strcmp(note, "eb") == 0)
        num = 3;
    else if (strcmp(note, "E") == 0 || strcmp(note, "e") == 0)
        num = 4;
    else if (strcmp(note, "F") == 0 || strcmp(note, "f") == 0)
        num = 5;
    else if (strcmp(note, "F#") == 0 || strcmp(note, "Gb") == 0 || strcmp(note, "f#") == 0 || strcmp(note, "gb") == 0)
        num = 6;
    else if (strcmp(note, "G") == 0 || strcmp(note, "g") == 0)
        num = 7;
    else if (strcmp(note, "G#") == 0 || strcmp(note, "Ab") == 0 || strcmp(note, "g#") == 0 || strcmp(note, "ab") == 0)
        num = 8;
    else if (strcmp(note, "A") == 0 || strcmp(note, "a") == 0)
        num = 9;
    else if (strcmp(note, "A#") == 0 || strcmp(note, "Bb") == 0 || strcmp(note, "a#") == 0 || strcmp(note, "bb") == 0)
        num = 10;
    else if (strcmp(note, "B") == 0 || strcmp(note, "b") == 0)
        num = 11;

    return num;
}

char *map_num_to_note(int num)
{
    switch (num)
    {
    case 0:
        return "C";
    case 1:
        return "C#";
    case 2:
        return "D";
    case 3:
        return "D#";
    case 4:
        return "E";
    case 5:
        return "F";
    case 6:
        return "F#";
    case 7:
        return "G";
    case 8:
        return "G#";
    case 9:
        return "A";
    case 10:
        return "A#";
    case 11:
        return "B";
    default: 
        return "C";
    }
}

char *get_note(char *root_note, int semitones)
{
    int num = map_note_to_num(root_note);
    int new_num = ((num + semitones) % NUM_NOTES + NUM_NOTES) % NUM_NOTES; // ensure new_num is within [0, 11]

    return map_num_to_note(new_num);
}

char *rand_root_note()
{
    int num = rand() % 12;
    return map_num_to_note(num);
}

void free_progression(Progression *progression)
{
    free(progression->chords);
    progression->chords = NULL;
    progression->chordSize = 0;
}

int chord_notes(const Chord *chord, int *out) {
    int third = 4, fifth = 7, seventh = 11; // defaults to major

    switch (chord->quality) {
        case MAJOR: 
            break;
        case MINOR:
            third = 3; 
            seventh = 10; 
            break;
        case DOM: 
            seventh = 10; 
            break;
        case SUSP:
            third = 5; 
            seventh = 10; 
            break;
        case HALFDIM:
            third = 3; 
            fifth = 6; 
            seventh = 10; 
            break;
        case DIM: 
            third = 3; 
            fifth = 6; 
            seventh = 9; 
            break;
    }

    int n = 0; 

    out[n++] = 0; // root
    out[n++] = third; // third
    out[n++] = fifth; // fifth 

    if (chord->extension >= 7 || chord->quality == DOM) {
        out[n++] = seventh; // seventh
    }

    if (chord->extension >= 9) {
        out[n++] = 14; // ninth
    }

    if (chord->extension >= 13) {
        out[n++] = 21; // thirteenth
    }
    
    return n; // return the number of notes in the chord
}

static Progression build_progression(char *root_key, const ChordTemplate *t) {
    Progression progression; 
    progression.chordSize = 4;
    progression.chords = malloc(progression.chordSize * sizeof(Chord));

    if (progression.chords == NULL) {
        progression.chordSize = 0;
        return progression; // return an empty progression if memory allocation fails
    }

    for (int i = 0; i < progression.chordSize; i++) {
        progression.chords[i].key = get_note(root_key, t[i].semitones);
        progression.chords[i].quality = t[i].quality;
        progression.chords[i].extension = t[i].extension;
    }

    return progression;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "music.h"

#define NUM_NOTES 12

void init_music(void)
{
    srand(time(NULL));
}

Progression generate_progression(char *root_key, Quality base_quality)
{
    Progression progression;
    progression.chordSize = 4;

    if (map_note_to_num(root_key) == -1)
    {
        progression.chords = NULL;
        progression.chordSize = 0;
        return progression; // invalid root note
    }

    progression.chords = malloc(progression.chordSize * sizeof(Chord));
    
    if (progression.chords == NULL)
    {
        progression.chordSize = 0;
        return progression; // return an empty progression if memory allocation fails
    }

    switch (base_quality)
    {
    case MAJOR:
        progression.chords[0].key = get_note(root_key, 2);
        progression.chords[0].quality = MINOR;
        progression.chords[0].extension = 7;

        progression.chords[1].key = get_note(root_key, 7);
        progression.chords[1].quality = DOM;
        progression.chords[1].extension = 9;

        progression.chords[2].key = get_note(root_key, 0);
        progression.chords[2].quality = MAJOR;
        progression.chords[2].extension = 7;

        progression.chords[3].key = get_note(root_key, 0);
        progression.chords[3].quality = MAJOR;
        progression.chords[3].extension = 9;

        break;

    case MINOR:
        progression.chords[0].key = get_note(root_key, 9);
        progression.chords[0].quality = MINOR;
        progression.chords[0].extension = 9;

        progression.chords[1].key = get_note(root_key, 5);
        progression.chords[1].quality = MAJOR;
        progression.chords[1].extension = 7;

        progression.chords[2].key = get_note(root_key, 0);
        progression.chords[2].quality = MAJOR;
        progression.chords[2].extension = 7;

        progression.chords[3].key = get_note(root_key, 7);
        progression.chords[3].quality = DOM;
        progression.chords[3].extension = 9;

        break;

    case DOM:
        progression.chords[0].key = get_note(root_key, 0);
        progression.chords[0].quality = MAJOR;
        progression.chords[0].extension = 7;

        progression.chords[1].key = get_note(root_key, 9);
        progression.chords[1].quality = MINOR;
        progression.chords[1].extension = 9;

        progression.chords[2].key = get_note(root_key, 2);
        progression.chords[2].quality = MINOR;
        progression.chords[2].extension = 7;

        progression.chords[3].key = get_note(root_key, 7);
        progression.chords[3].quality = DOM;
        progression.chords[3].extension = 9;

        break;

    case SUSP:
        progression.chords[0].key = get_note(root_key, 9);
        progression.chords[0].quality = MINOR;
        progression.chords[0].extension = 9;

        progression.chords[1].key = get_note(root_key, 2);
        progression.chords[1].quality = MINOR;
        progression.chords[1].extension = 7;

        progression.chords[2].key = get_note(root_key, 7);
        progression.chords[2].quality = DOM;
        progression.chords[2].extension = 9;

        progression.chords[3].key = get_note(root_key, 0);
        progression.chords[3].quality = MAJOR;
        progression.chords[3].extension = 7;

        break;
    }

    return progression;
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
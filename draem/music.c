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

// one chord in a progression template: offset from the root, quality, extension
typedef struct
{
    int semitones;
    Quality quality;
    int extension;
} ChordTemplate;

#define PROG_LEN 4
#define TEMPLATES_PER_QUALITY 4

// [quality][template][chord]; the first template of each quality is the original one
static const ChordTemplate TEMPLATES[4][TEMPLATES_PER_QUALITY][PROG_LEN] = {
    [MAJOR] = {
        {{2, MINOR, 7}, {7, DOM, 9}, {0, MAJOR, 7}, {0, MAJOR, 9}},   // ii - V - I - I
        {{0, MAJOR, 7}, {9, MINOR, 7}, {2, MINOR, 9}, {7, DOM, 9}},   // I - vi - ii - V
        {{0, MAJOR, 9}, {7, MAJOR, 7}, {9, MINOR, 7}, {5, MAJOR, 9}}, // I - V - vi - IV
        {{5, MAJOR, 7}, {4, MINOR, 7}, {2, MINOR, 9}, {0, MAJOR, 9}}, // IV - iii - ii - I
    },
    [MINOR] = {
        {{9, MINOR, 9}, {5, MAJOR, 7}, {0, MAJOR, 7}, {7, DOM, 9}},   // vi - IV - I - V
        {{0, MINOR, 9}, {8, MAJOR, 7}, {3, MAJOR, 7}, {10, MAJOR, 9}}, // i - VI - III - VII
        {{0, MINOR, 7}, {5, MINOR, 9}, {7, MINOR, 7}, {0, MINOR, 9}}, // i - iv - v - i
        {{0, MINOR, 9}, {5, MINOR, 7}, {10, DOM, 9}, {3, MAJOR, 7}},  // i - iv - VII - III
    },
    [DOM] = {
        {{0, MAJOR, 7}, {9, MINOR, 9}, {2, MINOR, 7}, {7, DOM, 9}},   // I - vi - ii - V
        {{4, MINOR, 7}, {9, MINOR, 9}, {2, MINOR, 7}, {7, DOM, 9}},   // iii - vi - ii - V
        {{0, DOM, 7}, {5, DOM, 9}, {0, DOM, 9}, {7, DOM, 7}},         // I7 - IV7 - I7 - V7 (blues)
        {{9, DOM, 9}, {2, DOM, 7}, {7, DOM, 9}, {0, MAJOR, 7}},       // VI7 - II7 - V7 - I
    },
    [SUSP] = {
        {{9, MINOR, 9}, {2, MINOR, 7}, {7, DOM, 9}, {0, MAJOR, 7}},   // vi - ii - V - I
        {{0, SUSP, 9}, {5, MAJOR, 7}, {9, MINOR, 7}, {7, SUSP, 7}},   // Isus - IV - vi - Vsus
        {{5, MAJOR, 9}, {7, SUSP, 7}, {4, MINOR, 7}, {9, MINOR, 9}},  // IV - Vsus - iii - vi
        {{0, MAJOR, 7}, {2, SUSP, 9}, {5, MAJOR, 9}, {7, SUSP, 7}},   // I - IIsus - IV - Vsus
    },
};

Progression generate_progression(char *root_key, Quality base_quality)
{
    Progression progression;
    progression.chordSize = PROG_LEN;

    if (map_note_to_num(root_key) == -1 || base_quality < MAJOR || base_quality > SUSP)
    {
        progression.chords = NULL;
        progression.chordSize = 0;
        return progression; // invalid root note or quality
    }

    progression.chords = malloc(progression.chordSize * sizeof(Chord));

    if (progression.chords == NULL)
    {
        progression.chordSize = 0;
        return progression; // return an empty progression if memory allocation fails
    }

    // pick one of this quality's progressions at random
    const ChordTemplate *t = TEMPLATES[base_quality][rand() % TEMPLATES_PER_QUALITY];

    for (int i = 0; i < progression.chordSize; i++)
    {
        progression.chords[i].key = get_note(root_key, t[i].semitones);
        progression.chords[i].quality = t[i].quality;
        progression.chords[i].extension = t[i].extension;
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
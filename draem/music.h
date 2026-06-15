#ifndef MUSIC_H
#define MUSIC_H

typedef enum
{
    DREAMY,
    DRAMATIC,
    BALLAD,
    CINEMATIC
} Mode;
typedef enum
{
    MAJOR,
    MINOR,
    DOM,
    SUSP
} Quality;

typedef struct
{
    char *key;
    Quality quality;
    int extension;
} Chord;

typedef struct
{
    Chord *chords;
    int chordSize;
} Progression;

Progression generate_progression(char *root_key, Quality base_quality);
int map_note_to_num(char *note);
char *map_num_to_note(int num);
char *get_note(char *root_note, int semitones);
char *rand_root_note();

#endif
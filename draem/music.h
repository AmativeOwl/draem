#ifndef MUSIC_H
#define MUSIC_H
#define NUM_MODES 5 
#include <stddef.h>

typedef enum
{
    DREAMY,
    DRAMATIC,
    BALLAD,
    CINEMATIC,
    JAZZ
} Mode;

typedef enum
{
    MAJOR,
    MINOR,
    DOM,
    SUSP,
    HALFDIM,
    DIM
} Quality;

typedef struct
{
    char *key;
    Quality quality;
    int extension; // 0 = triad, 7, 9 or 13
} Chord;

typedef struct 
{
    int semitones; 
    Quality quality;
    int extension; 
} ChordTemplate;

typedef struct
{
    Chord *chords;
    int chordSize;
} Progression;

void init_music(void);
Progression generate_progression(char *root_key, Quality base_quality);
int map_note_to_num(char *note);
char *map_num_to_note(int num);
char *get_note(char *root_note, int semitones);
char *rand_root_note();
void free_progression(Progression *progression);

// for the new modes 
Progression generate_mode_progression(Mode mode, char *root_key);
const char *get_mode_name(Mode mode);
int parse_mode(const char* text);
void chord_name(const Chord *chord, char *buf, size_t size);
int chord_notes(const Chord *chord, int *out); // semitones from the chord root 

#endif
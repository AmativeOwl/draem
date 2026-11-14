#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

// ---- step 4: progressions for each mode ----
// each chord is {semitones from the key's root, quality, extension (0 = triad)}

static const ChordTemplate DREAMY_PROGS[][PROG_LEN] = {
    {{0, MAJOR, 7}, {5, MAJOR, 9}, {0, MAJOR, 9}, {5, MAJOR, 7}},  // I - IV floating
    {{0, MAJOR, 9}, {4, MINOR, 7}, {9, MINOR, 9}, {5, MAJOR, 7}},  // I - iii - vi - IV
    {{5, MAJOR, 7}, {5, MINOR, 7}, {0, MAJOR, 7}, {0, MAJOR, 9}},  // IV - iv - I (borrowed minor iv)
    {{9, MINOR, 9}, {5, MAJOR, 9}, {0, MAJOR, 7}, {7, SUSP, 9}},   // vi - IV - I - Vsus
    {{0, MINOR, 9}, {8, MAJOR, 7}, {3, MAJOR, 9}, {10, SUSP, 7}},  // i - bVI - bIII - bVIIsus
};

static const ChordTemplate DRAMATIC_PROGS[][PROG_LEN] = {
    {{0, MINOR, 0}, {8, MAJOR, 0}, {3, MAJOR, 0}, {10, MAJOR, 0}}, // i - bVI - bIII - bVII
    {{0, MINOR, 0}, {10, MAJOR, 0}, {8, MAJOR, 0}, {7, MAJOR, 0}}, // i - bVII - bVI - V (Andalusian)
    {{0, MINOR, 0}, {5, MINOR, 0}, {8, MAJOR, 0}, {7, DOM, 7}},    // i - iv - bVI - V7
    {{0, MINOR, 0}, {1, MAJOR, 0}, {0, MINOR, 0}, {10, MAJOR, 0}}, // i - bII - i - bVII (Phrygian)
    {{8, MAJOR, 0}, {10, MAJOR, 0}, {0, MINOR, 0}, {0, MINOR, 7}}, // bVI - bVII - i - i7 (build-up)
};

static const ChordTemplate BALLAD_PROGS[][PROG_LEN] = {
    {{0, MAJOR, 0}, {7, MAJOR, 0}, {9, MINOR, 7}, {5, MAJOR, 7}},  // I - V - vi - IV
    {{9, MINOR, 7}, {5, MAJOR, 9}, {0, MAJOR, 0}, {7, SUSP, 7}},   // vi - IV - I - Vsus
    {{0, MAJOR, 9}, {9, MINOR, 7}, {5, MAJOR, 7}, {7, DOM, 7}},    // I - vi - IV - V
    {{0, MAJOR, 7}, {4, MINOR, 7}, {5, MAJOR, 7}, {5, MINOR, 7}},  // I - iii - IV - iv
};

static const ChordTemplate CINEMATIC_PROGS[][PROG_LEN] = {
    {{0, MINOR, 9}, {8, MAJOR, 9}, {3, MAJOR, 0}, {10, SUSP, 9}},  // epic minor
    {{0, MINOR, 0}, {8, MAJOR, 7}, {5, MINOR, 9}, {7, SUSP, 7}},   // i - bVI - iv - Vsus
    {{0, MAJOR, 0}, {8, MAJOR, 0}, {10, MAJOR, 0}, {0, MAJOR, 9}}, // I - bVI - bVII - I (heroic)
    {{0, MINOR, 0}, {8, MAJOR, 0}, {3, MAJOR, 0}, {5, MAJOR, 0}},  // i - bVI - bIII - IV (Dorian lift)
    {{0, MAJOR, 0}, {4, MAJOR, 0}, {9, MINOR, 0}, {5, MAJOR, 9}},  // I - III - vi - IV (chromatic mediant)
};

static const ChordTemplate JAZZ_PROGS[][PROG_LEN] = {
    {{2, MINOR, 9}, {7, DOM, 13}, {0, MAJOR, 9}, {9, DOM, 9}},     // ii - V - I - VI7 (turnaround)
    {{0, MAJOR, 9}, {9, MINOR, 7}, {2, MINOR, 9}, {7, DOM, 13}},   // I - vi - ii - V
    {{4, MINOR, 7}, {9, DOM, 9}, {2, MINOR, 9}, {7, DOM, 13}},     // iii - VI7 - ii - V
    {{2, HALFDIM, 7}, {7, DOM, 7}, {0, MINOR, 9}, {8, MAJOR, 7}},  // iiø - V7 - i - bVI (minor ii-V-i)
    {{0, MAJOR, 7}, {1, DIM, 7}, {2, MINOR, 7}, {7, DOM, 9}},      // I - #idim7 - ii - V (passing dim)
    {{2, MINOR, 9}, {1, DOM, 9}, {0, MAJOR, 9}, {9, MINOR, 7}},    // ii - bII7 - I - vi (tritone sub)
};

// each mode's table plus how many progressions it has
typedef struct
{
    const ChordTemplate (*progs)[PROG_LEN];
    int count;
} ModeBank;

#define BANK(arr) {arr, (int)(sizeof(arr) / sizeof(arr[0]))}

static const ModeBank MODE_BANKS[NUM_MODES] = {
    [DREAMY] = BANK(DREAMY_PROGS),
    [DRAMATIC] = BANK(DRAMATIC_PROGS),
    [BALLAD] = BANK(BALLAD_PROGS),
    [CINEMATIC] = BANK(CINEMATIC_PROGS),
    [JAZZ] = BANK(JAZZ_PROGS),
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
    progression.chordSize = PROG_LEN;
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


// ---- step 5: pick a progression for a mode ----
Progression generate_mode_progression(Mode mode, char *root_key)
{
    Progression empty = {NULL, 0};

    if (map_note_to_num(root_key) == -1)
        return empty; // invalid root note

    if ((int)mode < 0 || (int)mode >= NUM_MODES)
        return empty; // unknown mode

    const ModeBank *bank = &MODE_BANKS[mode];

    // avoid giving the same progression twice in a row
    static int last_pick[NUM_MODES] = {-1, -1, -1, -1, -1};
    int pick = rand() % bank->count;
    if (bank->count > 1 && pick == last_pick[mode])
        pick = (pick + 1 + rand() % (bank->count - 1)) % bank->count;
    last_pick[mode] = pick;

    return build_progression(root_key, bank->progs[pick]);
}

// ---- step 6: names ----
const char *get_mode_name(Mode mode)
{
    switch (mode)
    {
    case DREAMY:
        return "Dreamy";
    case DRAMATIC:
        return "Dramatic";
    case BALLAD:
        return "Ballad";
    case CINEMATIC:
        return "Cinematic";
    case JAZZ:
        return "Jazz";
    }
    return "Unknown";
}

// accepts a number ("1".."5") or a name in any case ("dreamy", "JAZZ"); returns -1 if unknown
int parse_mode(const char *text)
{
    if (text == NULL)
        return -1;

    if (text[0] >= '1' && text[0] < '1' + NUM_MODES && text[1] == '\0')
        return text[0] - '1';

    for (int m = 0; m < NUM_MODES; m++)
    {
        const char *name = get_mode_name((Mode)m);
        int i = 0;
        while (text[i] != '\0' && tolower((unsigned char)text[i]) == tolower((unsigned char)name[i]))
            i++;
        if (text[i] == '\0' && name[i] == '\0')
            return m;
    }

    return -1;
}

// writes a readable chord name like "C#m7", "G13", "Bm7b5" into buf
void chord_name(const Chord *chord, char *buf, size_t size)
{
    int ext = chord->extension;
    const char *suffix = "";

    switch (chord->quality)
    {
    case MAJOR:
        suffix = ext >= 13 ? "maj13" : ext >= 9 ? "maj9" : ext >= 7 ? "maj7" : "";
        break;
    case MINOR:
        suffix = ext >= 13 ? "m13" : ext >= 9 ? "m9" : ext >= 7 ? "m7" : "m";
        break;
    case DOM:
        suffix = ext >= 13 ? "13" : ext >= 9 ? "9" : "7";
        break;
    case SUSP:
        suffix = ext >= 13 ? "13sus4" : ext >= 9 ? "9sus4" : ext >= 7 ? "7sus4" : "sus4";
        break;
    case HALFDIM:
        suffix = "m7b5";
        break;
    case DIM:
        suffix = ext >= 7 ? "dim7" : "dim";
        break;
    }

    snprintf(buf, size, "%s%s", chord->key, suffix);
}

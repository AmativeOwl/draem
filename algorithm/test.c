#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "music.h"
#include "audio.h"

// temporary per-mode speed until each mode gets its own sound in the rendering step
static double seconds_per_chord(Mode mode)
{
    switch (mode)
    {
    case DREAMY:
        return 3.2;
    case DRAMATIC:
        return 1.6;
    case BALLAD:
        return 2.6;
    case CINEMATIC:
        return 4.0;
    case JAZZ:
        return 1.8;
    }
    return 2.0;
}

static void print_usage(const char *program)
{
    fprintf(stderr, "usage: %s [mode] [key]\n", program);
    fprintf(stderr, "  modes: 1 dreamy, 2 dramatic, 3 ballad, 4 cinematic, 5 jazz (random if left out)\n");
    fprintf(stderr, "  key:   C, F#, Bb, ... (random if left out)\n");
}

int main(int argc, char *argv[])
{
    init_music();

    // mode: from argv[1], or random
    Mode mode = (Mode)(rand() % NUM_MODES);
    if (argc >= 2)
    {
        int parsed = parse_mode(argv[1]);
        if (parsed < 0)
        {
            fprintf(stderr, "unknown mode '%s'\n", argv[1]);
            print_usage(argv[0]);
            return 1;
        }
        mode = (Mode)parsed;
    }

    // key: from argv[2], or random
    char *key = rand_root_note();
    if (argc >= 3)
    {
        int num = map_note_to_num(argv[2]);
        if (num == -1)
        {
            fprintf(stderr, "unknown key '%s'\n", argv[2]);
            print_usage(argv[0]);
            return 1;
        }
        key = map_num_to_note(num); // normalise e.g. "bb" -> "A#"
    }

    Progression p = generate_mode_progression(mode, key);
    if (p.chords == NULL)
    {
        fprintf(stderr, "could not generate a progression\n");
        return 1;
    }

    printf("%s in %s:  ", get_mode_name(mode), key);
    for (int i = 0; i < p.chordSize; i++)
    {
        char name[16];
        chord_name(&p.chords[i], name, sizeof name);
        printf("%s  ", name);
    }
    printf("\n");
    fflush(stdout); // show the chords before the audio starts

    if (play_progression(&p, seconds_per_chord(mode)) != 0)
    {
        fprintf(stderr, "Could not play audio\n");
    }

    free_progression(&p);

    return 0;
}

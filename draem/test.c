#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "music.h"
#include "audio.h"

static const char *chord_suffix(const Chord *c)
{
    switch (c->quality)
    {
    case MAJOR:
        return c->extension == 9 ? "maj9" : "maj7";
    case MINOR:
        return c->extension == 9 ? "m9" : "m7";
    case DOM:
        return c->extension == 9 ? "9" : "7";
    case SUSP:
        return c->extension == 9 ? "9sus4" : "7sus4";
    }
    return "";
}

int main(int argc, char *argv[])
{
    init_music();
    char *note = rand_root_note();
    Quality quality = (Quality)(rand() % 4);
    Progression p = generate_progression(note, quality);

    for (int i = 0; i < p.chordSize; i++)
    {
        printf("%s%s  ", p.chords[i].key, chord_suffix(&p.chords[i]));
    }
    printf("\n");

    if (play_progression(&p, 2.0) != 0)
    {
        fprintf(stderr, "Could not play audio\n");
    }

    free_progression(&p);

    return 0;
}
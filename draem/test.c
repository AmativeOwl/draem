#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "music.h"

int main(int argc, char *argv[])
{
    char *note = rand_root_note();
    Progression p = generate_progression(note, MAJOR);

    for (int i = 0; i < p.chordSize; i++)
    {
        printf("%s %d %d\n", p.chords[i].key, p.chords[i].quality, p.chords[i].extension);
    }

    free_progression(&p);

    return 0;
}
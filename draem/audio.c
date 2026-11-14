#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#define NOGDI // windows.h's GDI declares a Chord() function that clashes with our Chord type
#include <windows.h>
#include <mmsystem.h>
#include "audio.h"

#define SAMPLE_RATE 44100
#define MAX_VOICES 8

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// semitone offsets from the chord root, based on quality and extension (7 or 9).
static int chord_intervals(const Chord *chord, int *out)
{
    int n = 0;
    out[n++] = 0;

    switch (chord->quality)
    {
    case MAJOR:
        out[n++] = 4;
        out[n++] = 7;
        if (chord->extension >= 7)
            out[n++] = 11; // major 7th
        break;
    case MINOR:
        out[n++] = 3;
        out[n++] = 7;
        if (chord->extension >= 7)
            out[n++] = 10; // minor 7th
        break;
    case DOM:
        out[n++] = 4;
        out[n++] = 7;
        out[n++] = 10; // dominant always has the flat 7th
        break;
    case SUSP:
        out[n++] = 5;
        out[n++] = 7;
        if (chord->extension >= 7)
            out[n++] = 10;
        break;
    }

    if (chord->extension >= 9)
        out[n++] = 14; // 9th

    return n;
}

static double midi_to_freq(int midi)
{
    return 440.0 * pow(2.0, (midi - 69) / 12.0);
}

static void put_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static void put_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

int play_progression(const Progression *progression, double seconds_per_chord)
{
    if (progression == NULL || progression->chords == NULL || progression->chordSize <= 0)
        return -1;

    size_t per_chord = (size_t)(seconds_per_chord * SAMPLE_RATE);
    size_t total = per_chord * (size_t)progression->chordSize;
    uint32_t data_bytes = (uint32_t)(total * 2);

    uint8_t *wav = malloc(44 + data_bytes);
    if (wav == NULL)
        return -1;

    // WAV header: 16-bit mono PCM
    memcpy(wav, "RIFF", 4);
    put_u32(wav + 4, 36 + data_bytes);
    memcpy(wav + 8, "WAVE", 4);
    memcpy(wav + 12, "fmt ", 4);
    put_u32(wav + 16, 16);
    put_u16(wav + 20, 1);               // PCM
    put_u16(wav + 22, 1);               // mono
    put_u32(wav + 24, SAMPLE_RATE);
    put_u32(wav + 28, SAMPLE_RATE * 2); // byte rate
    put_u16(wav + 32, 2);               // block align
    put_u16(wav + 34, 16);              // bits per sample
    memcpy(wav + 36, "data", 4);
    put_u32(wav + 40, data_bytes);

    uint8_t *out = wav + 44;

    for (int c = 0; c < progression->chordSize; c++)
    {
        const Chord *chord = &progression->chords[c];
        int root = map_note_to_num(chord->key);
        if (root < 0)
            root = 0;

        int intervals[MAX_VOICES];
        int n = chord_intervals(chord, intervals);

        double freqs[MAX_VOICES + 1];
        int voices = 0;
        freqs[voices++] = midi_to_freq(36 + root); // bass root (C2 octave)
        for (int k = 0; k < n; k++)
            freqs[voices++] = midi_to_freq(48 + root + intervals[k]); // chord (C3 octave)

        for (size_t s = 0; s < per_chord; s++)
        {
            double t = (double)s / SAMPLE_RATE;
            double remaining = seconds_per_chord - t;

            double attack = t < 0.02 ? t / 0.02 : 1.0;
            double release = remaining < 0.25 ? remaining / 0.25 : 1.0;
            double env = attack * release * exp(-0.7 * t);

            double x = 0.0;
            for (int v = 0; v < voices; v++)
            {
                double w = 2.0 * M_PI * freqs[v] * t;
                x += sin(w) + 0.3 * sin(2.0 * w) + 0.1 * sin(3.0 * w); // soft, slightly warm tone
            }
            x = x / (voices * 1.4) * env * 0.8;

            put_u16(out, (uint16_t)(int16_t)(x * 32767.0));
            out += 2;
        }
    }

    BOOL ok = PlaySoundA((LPCSTR)wav, NULL, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
    free(wav);

    return ok ? 0 : -1;
}

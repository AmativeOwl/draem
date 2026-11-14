#ifndef AUDIO_H
#define AUDIO_H

#include "music.h"

// synthesizes the progression and plays it through the speakers (blocks until done).
// returns 0 on success, -1 on failure.
int play_progression(const Progression *progression, double seconds_per_chord);

#endif

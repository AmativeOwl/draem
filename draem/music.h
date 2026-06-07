typedef enum
{
    MAJOR,
    MINOR,
    DOM
} Quality;

typedef struct
{
    char key;
    Quality quality;
    int extension;
} Chord;

typedef struct
{
    Chord *chords;
    int chordSize;
} Progression;
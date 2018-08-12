#ifndef MAPTOOL_STRUCTS_HPP
#define MAPTOOL_STRUCTS_HPP

#include "utils.hpp"

template<int n>
struct PACK Vector
{
    float coordinates[n];
};

/**
 * Binary structure for a track position marker
 */
typedef struct PACK _TrackPositionMarker
{
    unsigned int unknownCounters[2];
    unsigned long long hash; // ???
    Vector<3> position;
    BYTE unknown[20];
} PosMarker_t;

// chunk 57 41 03 00
typedef struct PACK _SectionFileMapping
{
    unsigned int unknownCounters[2];
    unsigned int streamChunkNumber;
    unsigned int fileHash;
} SectionFileMapping_t;

typedef struct PACK _Section
{
    char name[8];
    unsigned int streamChunkNumber;
    unsigned int blank[3];
    unsigned int streamChunkHash;
    unsigned int sectionFileID; // if 0, streamChunkNumber is used
    unsigned int unknown1;
    unsigned int streamChunkNumber2;
    Vector<3> position; // if sectionFileID != 0 then position = (0, 0, 0)
    BYTE blank2[36];
    unsigned int unknown2; // 0xFFFFFFFF
    unsigned int type; // if sectionFileID = 0 then type = 1
    unsigned int parameters[28];
} Section_t;

/**
 * This has something to do with scenery objects,
 * I haven't figured out the way it works yet
 */
typedef struct PACK _SceneryObjectPosition
{
    unsigned int unknownCounters[6];
} SceneryPos_t;

typedef struct PACK _SceneryObjectPosition2
{
    unsigned long long hash;
    unsigned long long blank;
    Vector<4> coordSet1;
    Vector<4> coordSet2;
    Vector<4> coordSet3;
} SceneryPos2_t;

typedef struct PACK _Section2
{
    unsigned int unknownCounters[2];
    char name[16];
    unsigned int unknown[2];
    unsigned int blank[10];
} Section2_t;

#endif //MAPTOOL_STRUCTS_HPP

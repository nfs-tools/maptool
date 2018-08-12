#ifndef MAPTOOL_MAPSTREAMREADER_HPP
#define MAPTOOL_MAPSTREAMREADER_HPP

#include <fstream>
#include <vector>

#include "structs.hpp"

#define CHUNK_TRACK_MARKERS 0x00034146
#define CHUNK_SCENERY_POSITIONS 0x00034152
#define CHUNK_SCENERY_POSITIONS2 0x00034156
#define CHUNK_SECTIONS 0x00034110
#define CHUNK_SECTIONS2 0x00034155
#define CHUNK_SECTION_FILE_MAPPING 0x00034157
#define CHUNK_TROUGH_BOUNDARY 0x57944655

struct MapStreamSection
{
    std::string name;
    int sectionID;
    int fileID;
    bool isPartialFile;
    float x, y, z;
    int hash;
    int type;

    int t_tpkNullOffset;
    int t_tpkDataOffset;
    int t_tpkUnknown2;
    int t_tpkUnknown3;
    int t_tpkUnknown4;
    int t_fileSize, t_fileSizePadded;
};

struct SectionFileMapping
{
    int sectionID;
    int fileID;
};

struct TroughBoundaryPoint
{
    float x, y;
};

struct TroughBoundaryDef
{
    std::string name;
    Vector<2> minPoint, maxPoint;
    std::vector<TroughBoundaryPoint> points;
};

struct TrackMarker
{
    unsigned long long hash;
    Vector<3> position;
};

/**
 * A simple chunk reader class that is used to operate on a location bundle file.
 */
class LocationBundleReader
{
public:
    /**
     * Read chunks from the given input stream.
     *
     * @param stream The stream
     * @param length The stream length
     */
    void Read(std::istream &stream, size_t length);

private:
    void ReadChunks(std::istream &stream, size_t length);

    void ReadSections(std::istream &stream, size_t length);

    void ReadSectionFileMappings(std::istream &stream, size_t length);

    void ReadMarkers(std::istream &stream, size_t length);

    void ReadTroughBoundary(std::istream &stream, size_t length);

public:
    std::vector<MapStreamSection> sections;
    std::vector<SectionFileMapping> sectionFileMappings;
    std::vector<TroughBoundaryDef> troughBoundaries;
    std::vector<TrackMarker> trackMarkers;
};


#endif //MAPTOOL_MAPSTREAMREADER_HPP

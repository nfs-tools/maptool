#include <iostream>
#include "LocationBundleReader.hpp"
#include "utils.hpp"

void LocationBundleReader::Read(std::istream &stream, size_t length)
{
    this->ReadChunks(stream, length);
}

void LocationBundleReader::ReadChunks(std::istream &stream, size_t length)
{
    auto curStreamPos = (unsigned long) stream.tellg();
    auto endOfStream = curStreamPos + length;

//    printf("Reading chunks @ %lu -> %lu\n", curStreamPos, curStreamPos + length);
    for (auto i = 0; i < 0xFFFF && stream.tellg() < endOfStream; ++i)
    {
        auto chunkId = readGeneric<unsigned int>(stream);
        auto chunkSize = readGeneric<unsigned int>(stream);

        curStreamPos = (unsigned long) stream.tellg();

        auto chunkEnd = curStreamPos + chunkSize;

        auto padding = 0u;

        while (readGeneric<BYTE>(stream) == 0x11)
        {
            padding++;
        }

        stream.seekg(-1, std::ios::cur);

        if (padding % 2 != 0)
        {
            padding--;
        }

        chunkSize -= padding;

//        printf("\tChunk #%d: 0x%08x (%d bytes @ %lu)\n", i + 1, chunkId, chunkSize, curStreamPos);

        if ((chunkId & 0x80000000) == 0x80000000)
        {
//            printf("\t* Container chunk\n");
            this->ReadChunks(stream, chunkSize);
        } else
        {
            switch (chunkId)
            {
                case CHUNK_TRACK_MARKERS:
                {
                    static_assert(sizeof(PosMarker_t) == 48);
                    assert(chunkSize % sizeof(PosMarker_t) == 0);

                    this->ReadMarkers(stream, chunkSize);

                    break;
                }
                case CHUNK_SECTIONS:
                {
                    static_assert(sizeof(Section_t) == 208);
                    assert(chunkSize % sizeof(Section_t) == 0);

                    this->ReadSections(stream, chunkSize);

                    break;
                }
                case CHUNK_SECTION_FILE_MAPPING:
                {
                    static_assert(sizeof(SectionFileMapping_t) == 16);
                    assert(chunkSize % sizeof(SectionFileMapping_t) == 0);

                    this->ReadSectionFileMappings(stream, chunkSize);

                    break;
                }
                case CHUNK_TROUGH_BOUNDARY:
                {
                    this->ReadTroughBoundary(stream, chunkSize);
                    break;
                }
                default:
                    break;
            }
        }

        stream.seekg(chunkEnd, std::istream::beg);
    }
}

void LocationBundleReader::ReadSectionFileMappings(std::istream &stream, size_t length)
{
    for (auto i = 0; i < length / sizeof(SectionFileMapping_t); i++)
    {
        auto mapping = readGeneric<SectionFileMapping_t>(stream);

//        std::cout << "Mapping #" << i + 1 << std::endl;
//        printf("\tcounters: [%d, %d]\n", mapping.unknownCounters[0], mapping.unknownCounters[1]);
//        printf("\tstream:   %d\n", mapping.streamChunkNumber);
//        printf("\tfHash:    0x%08x\n", mapping.fileHash);

        SectionFileMapping sectionFileMapping{};
        sectionFileMapping.fileID = mapping.fileHash;
        sectionFileMapping.sectionID = mapping.streamChunkNumber;

        sectionFileMappings.emplace_back(sectionFileMapping);
    }
}

void LocationBundleReader::ReadMarkers(std::istream &stream, size_t length)
{
    for (auto i = 0; i < length / sizeof(PosMarker_t); i++)
    {
        auto marker = readGeneric<PosMarker_t>(stream);

//        std::cout << "Marker #" << i + 1 << std::endl;
//        printf("\tcounters: [%d, %d]\n", marker.unknownCounters[0], marker.unknownCounters[1]);
//        printf("\thash:     0x%08llu\n", marker.hash);
//        printf("\tpos:      (%f, %f, %f)\n", marker.position.coordinates[0], marker.position.coordinates[1],
//               marker.position.coordinates[2]);

        TrackMarker trackMarker{};
        trackMarker.hash = marker.hash;
        trackMarker.position = marker.position;

        trackMarkers.emplace_back(trackMarker);
    }
}

void LocationBundleReader::ReadSections(std::istream &stream, size_t length)
{
    for (auto i = 0; i < length / sizeof(Section_t); i++)
    {
        auto section = readGeneric<Section_t>(stream);

//        std::cout << "Section " << section.name << std::endl;
//        printf("\tscn: %d/%d\n", section.streamChunkNumber, section.streamChunkNumber2);
//        printf("\tsch: 0x%08x\n", section.streamChunkHash);
//        printf("\tscf: 0x%08x\n", section.sectionFileID);
//        printf("\tsct: %d\n", section.type);
//        printf("\tpos: (%f, %f, %f)\n", section.position.coordinates[0], section.position.coordinates[1],
//               section.position.coordinates[2]);
//
//        for (auto j = 0; j < 28; ++j)
//        {
//            printf("\tparam #%d: %d (0x%08x)\n", j + 1, section.parameters[j], section.parameters[j]);
//        }

        MapStreamSection mapStreamSection{};
        mapStreamSection.type = section.type;
        mapStreamSection.name = std::string(section.name);
        mapStreamSection.x = section.position.coordinates[0];
        mapStreamSection.y = section.position.coordinates[1];
        mapStreamSection.z = section.position.coordinates[2];
        mapStreamSection.hash = section.streamChunkHash;
        mapStreamSection.sectionID = section.streamChunkNumber;

        if (section.sectionFileID != 0)
        {
            mapStreamSection.fileID = section.sectionFileID;
            mapStreamSection.isPartialFile = true;
        } else
        {
            mapStreamSection.fileID = section.streamChunkNumber;
        }

        if (section.type == 1)
        {
            mapStreamSection.t_fileSize = section.parameters[0];
            mapStreamSection.t_fileSizePadded = section.parameters[2];
        } else
        {
            mapStreamSection.t_fileSize = section.parameters[12];
            mapStreamSection.t_tpkNullOffset = section.parameters[0];
            mapStreamSection.t_tpkDataOffset = section.parameters[1];
            mapStreamSection.t_tpkUnknown2 = section.parameters[3];
            mapStreamSection.t_tpkUnknown3 = section.parameters[6];
            mapStreamSection.t_tpkUnknown4 = section.parameters[9];
        }

        sections.emplace_back(mapStreamSection);

        //        hex_dump(section.remainingData, sizeof(section.remainingData), std::cout);
    }
}

void LocationBundleReader::ReadTroughBoundary(std::istream &stream, size_t length)
{
    stream.ignore(8);
    char name[16];

    stream.read(name, 16);

    stream.ignore(72);

    float minX, minY, maxX, maxY;

    minY = readGeneric<float>(stream);
    minX = readGeneric<float>(stream);
    maxY = readGeneric<float>(stream);
    maxX = readGeneric<float>(stream);

//    printf("%s @ (%f, %f) -> (%f, %f)\n", std::string(name), minX, minY, maxX, maxY);

    auto numElements = readGeneric<int>(stream);

    length -= 116;

    assert(length % numElements == 0 && length / numElements == 8);

    TroughBoundaryDef troughBoundaryDef{};
    troughBoundaryDef.name = std::string(name);
    troughBoundaryDef.minPoint.coordinates[0] = minX;
    troughBoundaryDef.minPoint.coordinates[1] = minY;
    troughBoundaryDef.maxPoint.coordinates[0] = maxX;
    troughBoundaryDef.maxPoint.coordinates[1] = maxY;

    for (auto i = 0; i < numElements; i++)
    {
        auto y = readGeneric<float>(stream);
        auto x = readGeneric<float>(stream);

//        printf("\tPoint #%d: (%f, %f)\n", i + 1, x, y);

        TroughBoundaryPoint point{};
        point.x = x;
        point.y = y;

        troughBoundaryDef.points.emplace_back(point);
    }

    troughBoundaries.emplace_back(troughBoundaryDef);
}

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "utils.hpp"
#include "LocationBundleReader.hpp"

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: maptool <Tracks|TracksHigh> <bundle>" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "Tracks") != 0 && strcmp(argv[1], "TracksHigh") != 0)
    {
        std::cerr << "Invalid track folder name." << std::endl;
        std::cerr << "Usage: maptool <Tracks|TracksHigh> <bundle>" << std::endl;
        return 1;
    }

    boost::filesystem::path basePath = "/Users/heyitsleo/Desktop/NFSW/Need For Speed World/Data";
    boost::filesystem::path tracksPath = basePath / argv[1];
    boost::filesystem::path locationBundlePath = tracksPath / (std::string(argv[2]) + ".BUN");
    boost::filesystem::path troughPath = tracksPath / std::string(argv[2]) / "TroughBoundary.bin";

    if (!boost::filesystem::is_regular_file(locationBundlePath))
    {
        std::cerr << "Cannot find location bundle." << std::endl;
        return 1;
    }

    std::shared_ptr<LocationBundleReader> bundleReader(new LocationBundleReader);

    {
        std::ifstream bundleStream(locationBundlePath.string(), std::ios::binary);

        bundleReader->Read(bundleStream, getLength(bundleStream));
    }

    if (!boost::filesystem::is_regular_file(troughPath))
    {
        std::cerr << "Cannot find trough file." << std::endl;
        return 1;
    }

    {
        std::ifstream troughStream(troughPath.string(), std::ios::binary);

        bundleReader->Read(troughStream, getLength(troughStream));
    }

    std::cout << "Location Bundle Info:" << std::endl;
    std::cout << "\tID:       " << std::string(argv[2]) << std::endl;
    std::cout << "\tSections: " << bundleReader->sections.size() << std::endl;

    for (auto i = 0; i < bundleReader->sections.size(); ++i)
    {
        auto section = bundleReader->sections[i];

        std::cout << "\t\tSection #" << i + 1 << ":" << std::endl;
        printf("\t\t\tID:        %d\n", section.sectionID);
        printf("\t\t\tName:      %s\n", section.name.c_str());
        printf("\t\t\tHash:      0x%08x\n", section.hash);
        printf("\t\t\tType:      0x%08x\n", section.type);
        printf("\t\t\tFile Size: %d/%d\n", section.t_fileSize, section.t_fileSizePadded);
        printf("\t\t\tFile ID:   ");

        if (section.isPartialFile)
        {
            printf("0x%08x\n", section.fileID);
        } else
        {
            printf("%d\n", section.fileID);
        }

        printf("\t\t\tPosition: (%f, %f, %f)\n", section.x, section.y, section.z);
    }

    std::cout << "\tMarkers:            " << bundleReader->trackMarkers.size() << std::endl;

    for (auto i = 0; i < bundleReader->trackMarkers.size(); ++i)
    {
        auto marker = bundleReader->trackMarkers[i];

        std::cout << "\t\tMarker #" << i + 1 << ":" << std::endl;
        printf("\t\t\tHash:      0x%08llu\n", marker.hash);
        printf("\t\t\tPosition: (%f, %f, %f)\n", marker.position.coordinates[0], marker.position.coordinates[1],
               marker.position.coordinates[2]);
    }

    std::cout << "\tTrough Boundaries:  " << bundleReader->troughBoundaries.size() << std::endl;

    for (auto i = 0; i < bundleReader->troughBoundaries.size(); ++i)
    {
        auto troughBoundary = bundleReader->troughBoundaries[i];

        std::cout << "\t\tTroughBoundary #" << i + 1 << ":" << std::endl;
        printf("\t\t\tName:     %s\n", troughBoundary.name.c_str());
        printf("\t\t\tBounds:   (%f, %f) -> (%f, %f)\n",
               troughBoundary.minPoint.coordinates[0],
               troughBoundary.minPoint.coordinates[1],
               troughBoundary.maxPoint.coordinates[0],
               troughBoundary.maxPoint.coordinates[1]);
        printf("\t\t\tPoints:   %zu\n", troughBoundary.points.size());

        for (auto j = 0; j < troughBoundary.points.size(); ++j)
        {
            printf("\t\t\tPoint #%d: (%f, %f)\n", j + 1, troughBoundary.points[j].x, troughBoundary.points[j].y);
        }
    }

    return 0;
}
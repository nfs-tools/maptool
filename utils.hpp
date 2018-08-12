#ifndef MAPTOOL_UTILS_HPP
#define MAPTOOL_UTILS_HPP

#include <fstream>
#include <ostream>
#include <string>
#include <boost/variant.hpp>

typedef unsigned char BYTE;

#define PACK __attribute__((__packed__))

size_t getLength(std::istream &stream);

template<typename T>
T readGeneric(std::istream &stream, size_t size = sizeof(T))
{
    T result;
    stream.read((char *) &result, size);

    return result;
}

template<typename T>
void writeGeneric(std::ostream &stream, T data, size_t size = sizeof(T))
{
    stream.write((const char *) &data, size);
}

template<typename T>
void fill(std::ostream &stream, T data, int count)
{
    for (auto i = 0; i < count; i++)
    {
        writeGeneric<T>(stream, data);
    }
}

void printStreamPosition(std::istream &stream);

template<class Elem, class Traits>
inline void
hex_dump(const void *aData, std::size_t aLength, std::basic_ostream<Elem, Traits> &aStream, std::size_t aWidth = 16)
{
    const auto *const start = static_cast<const char *>(aData);
    const char *const end = start + aLength;
    const char *line = start;
    while (line != end)
    {
        aStream.width(4);
        aStream.fill('0');
        aStream << std::hex << line - start << " : ";
        std::size_t lineLength = std::min(aWidth, static_cast<std::size_t>(end - line));
        for (std::size_t pass = 1; pass <= 2; ++pass)
        {
            for (const char *next = line; next != end && next != line + aWidth; ++next)
            {
                char ch = *next;
                switch (pass)
                {
                    case 1:
                        aStream << (ch < 32 ? '.' : ch);
                        break;
                    case 2:
                        if (next != line)
                        {
                            aStream << " ";
                        }
                        aStream.width(2);
                        aStream.fill('0');
                        aStream << std::hex << std::uppercase << static_cast<int>(static_cast<unsigned char>(ch));
                        break;
                    default:
                        break;
                }
            }
            if (pass == 1 && lineLength != aWidth)
            {
                aStream << std::string(aWidth - lineLength, ' ');
            }
            aStream << " ";
        }
        aStream << std::endl;
        line = line + lineLength;
    }
}

#endif //TPKTOOL_UTILS_HPP

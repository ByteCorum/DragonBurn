#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <ctime>
#include <filesystem>
#include <string>
#include <fstream>

std::vector<uint8_t> GenKey(size_t length)
{
    std::mt19937 gen(static_cast<unsigned int>(std::time(0)));
    std::uniform_int_distribution<int> distrib(0, 255);

    std::vector<uint8_t> key(length);
    for (size_t i = 0; i < length; ++i) 
        key[i] = static_cast<uint8_t>(distrib(gen));
    return key;
}

void Crypt(std::vector<uint8_t>& data, const std::vector<uint8_t>& key)
{
    if (key.empty())
    {
        std::cerr << "Error: Encryption key cannot be empty." << std::endl;
        return;
    }

    for (size_t i = 0; i < data.size(); ++i)
        data[i] ^= key[i % key.size()];
}

void Help() 
{
    std::cout << "Usage:\n\ncrypto crypt \"path/to/key\" \"path/to/file\" --> encrypt/decrypt file\ncrypto gen \"output/path\" --> gen key\ncrypto print \"path/to/file\" --> print bytes";
}

bool WriteFile(const std::string& filename, const std::vector<uint8_t>& bin)
{
    std::ofstream outfile(filename, std::ios::out | std::ios::binary);
    if (!outfile)
    {
        std::cerr << "Error: Could not open file for writing: " + filename << std::endl;
        return false;
    }

    outfile.write(reinterpret_cast<const char*>(bin.data()), bin.size());
    outfile.close();
    return true;
}

bool ReadFile(const std::string& filename, std::vector<uint8_t>& bin)
{
    std::ifstream infile(filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!infile)
    {
        std::cerr << "Error: Could not open file for reading: " + filename << std::endl;
        return false;
    }

    std::streamsize size = infile.tellg();
    infile.seekg(0, std::ios::beg);

    bin.resize(size);
    infile.read(reinterpret_cast<char*>(bin.data()), size);
    infile.close();
    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        Help();
        return 1;
    }

    std::string command = argv[1];
    if (command == "crypt")
    {
        if (argc < 4 || !std::filesystem::exists(argv[2]) || !std::filesystem::exists(argv[3]))
        {
            Help();
            return 1;
        }

        std::vector<uint8_t> key;
        std::vector<uint8_t> image;

        if (!ReadFile(argv[2], key))
            return 1;
        if (!ReadFile(argv[3], image))
            return 1;

        Crypt(image, key);

        std::string outname = argv[3];
        if (!WriteFile(outname + ".enc", image))
            return 1;

        std::cout << "File encrypted/decrypted successfully." << std::endl;
    }
    else if (command == "gen")
    {
        if (argc < 3)
        {
            Help();
            return 1;
        }

        std::vector<uint8_t> key = GenKey(32);
        if (!WriteFile(argv[2], key))
            return 1;
        std::cout << "Key saved to " << argv[2] << std::endl;
    }
    else if (command == "print")
    {
        if (argc < 3 || !std::filesystem::exists(argv[2]))
        {
            Help();
            return 1;
        }

        std::vector<uint8_t> data;
        if (!ReadFile(argv[2], data))
            return 1;

        std::cout << "File contents:\n";
        for (size_t i = 0; i < data.size(); ++i)
        {
            printf("0x%02X", data[i]);
            if (i < data.size() - 1)
                std::cout << ", ";
        }
        std::cout << "\n";
    }
    else
    {
        Help();
        return 1;
    }
    return 0;
}

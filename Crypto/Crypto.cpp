#include <iostream>
#include <vector>
#include <cstdint>
#include <random>
#include <ctime>

std::vector<uint8_t> GenKey(size_t length)
{
    std::mt19937 gen(static_cast<unsigned int>(std::time(0)));
    std::uniform_int_distribution<int> distrib(0, 255);

    std::vector<uint8_t> key(length);
    for (size_t i = 0; i < length; ++i) 
        key[i] = static_cast<uint8_t>(distrib(gen));
    return key;
}

void Crypto(std::vector<uint8_t>& data, const std::vector<uint8_t>& key)
{
    if (key.empty())
    {
        std::cerr << "Error: Encryption key cannot be empty." << std::endl;
        return;
    }

    for (size_t i = 0; i < data.size(); ++i)
        data[i] ^= key[i % key.size()];
}

std::vector<uint8_t> image = 
{
    0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64
};

int main()
{
    const std::vector<uint8_t> key = GenKey(32);
    std::cout << "Key:\n{\n";
    for (size_t i = 0; i < key.size(); ++i)
    {
        printf("0x%02X", key[i]);
        if (i < key.size() - 1)
            std::cout << ", ";
    }
    std::cout << "\n}\n\n";

    Crypto(image, key);
    std::cout << "Encrypted image:\n{\n";
    for (size_t i = 0; i < image.size(); ++i)
    {
        printf("0x%02X", image[i]);
        if (i < image.size() - 1)
            std::cout << ", ";
    }
    std::cout << "\n}\n\n";
}

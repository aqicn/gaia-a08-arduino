#include "main.hpp"
#include "utils.hpp"

// read Mac, and reverse to ID
uint32_t MacId()
{

    uint32_t id = 0;

    for (int i = 0; i < 17; i = i + 8)
    {
        id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    return id >> 2; // low two bits are unused.
}

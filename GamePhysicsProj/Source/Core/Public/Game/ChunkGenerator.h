#pragma once
#include <random>

#include "Math/Vector2.h"

class Rectangle;

class ChunkGenerator
{

private:

    Vector2 mChunkSize = {};

    int mChunkGenerationHeight = 0;
    uint32_t mPlatformPerChunkCount = 8;

    struct
    {
        std::mt19937 mRandomNumberGenerator;
        std::uniform_int_distribution<> mWidthDistribution = {};
        std::uniform_int_distribution<> mHeightDistribution = {};
    };
    
public:

    ChunkGenerator(const Vector2& inChunkSize, uint32_t inPlatformPerChunkCount);

    void generateChunk(int chunkHeight, std::vector<std::unique_ptr<Rectangle>>& outPlatforms);
    int getChunkGenerationHeight() const { return mChunkGenerationHeight; }

};

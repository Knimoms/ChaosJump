#include "Game/ChunkGenerator.h"

#include <random>

#include "Objects/Platform.h"
#include "Objects/Rectangle.h"

ChunkGenerator::ChunkGenerator(const Vector2& inChunkSize, const uint32_t inPlatformPerChunkCount) : mChunkSize(inChunkSize), mPlatformPerChunkCount(inPlatformPerChunkCount), mRandomNumberGenerator(std::random_device()())
{
    const int halfChunkWidth = static_cast<int>(inChunkSize.x) / 2;
    const int halfChunkHeight = static_cast<int>(inChunkSize.y) / 2;
    mWidthDistribution = std::uniform_int_distribution<>(-halfChunkWidth,halfChunkWidth);
    mHeightDistribution = std::uniform_int_distribution<>(-halfChunkHeight, halfChunkHeight);
}

void ChunkGenerator::generateChunk(int chunkHeight, std::vector<std::unique_ptr<Rectangle>>& outPlatforms)
{
    mChunkGenerationHeight = std::max(mChunkGenerationHeight, chunkHeight);

    for (uint32_t i = 0; i < mPlatformPerChunkCount; ++i)
    {
        const int xCoord = mWidthDistribution(mRandomNumberGenerator);
        const int yCoord = chunkHeight * -mChunkSize.y + mHeightDistribution(mRandomNumberGenerator);

        Vector2 position = {.x = static_cast<float>(xCoord), .y = static_cast<float>(yCoord)};
        
        std::unique_ptr<Platform> platform = std::make_unique<Platform>();
        platform->setLocation(position);
        outPlatforms.push_back(std::move(platform));
    }
}

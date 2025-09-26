#pragma once
#include <random>

#include "Math/Vector2.h"

class Platform;
class CollisionObject;
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
        std::uniform_real_distribution<float> mColorDistribution = {};
        std::uniform_int_distribution<> mObstacleTypeDistribution = {};
    };

protected:

    void spawnRandomObstacle(int chunkHeight, std::vector<std::unique_ptr<CollisionObject>>& outObstacles);

    template<class ObjectType>
    std::unique_ptr<ObjectType> createRandomCollisionObject(float chunkHeight, const Vector2& chunkSize);
    
public:

    ChunkGenerator(const Vector2& inChunkSize, uint32_t inPlatformPerChunkCount, uint32_t inSeed);

    void generateChunk(int chunkHeight, std::vector<std::unique_ptr<Platform>>& outPlatforms, std::vector<std::unique_ptr<CollisionObject>>& outObstacles);
    int getChunkGenerationHeight() const { return mChunkGenerationHeight; }

};

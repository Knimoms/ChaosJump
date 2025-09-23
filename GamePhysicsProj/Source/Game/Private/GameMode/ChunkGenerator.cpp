#include "GameMode/ChunkGenerator.h"

#include <random>

#include "Application.h"
#include "Objects/Circle.h"
#include "Objects/Platform.h"
#include "Objects/Polygon.h"
#include "Objects/Rectangle.h"
#include "SDL3/SDL_stdinc.h"

template<>
std::unique_ptr<Circle> ChunkGenerator::createRandomCollisionObject(float chunkHeight, const Vector2& chunkSize)
{
    std::uniform_int_distribution<> sizeDist(25, 75);
    std::uniform_int_distribution<> colorDist(0,SDL_MAX_UINT8);

    unsigned int size = sizeDist(mRandomNumberGenerator);
    std::unique_ptr<Circle> circle = std::make_unique<Circle>(size);

    const float r = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    const float g = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    const float b = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    circle->setColor({.r = r, .g = g, .b = b});
    circle->setCanCollideWithWindowBorder(true, true);
    circle->setGravity({});
    std::uniform_int_distribution<> locHeightDist(size + (chunkHeight - 1) * chunkSize.y,chunkHeight * chunkSize.y - size);
    std::uniform_int_distribution<> locWidthDist(size,chunkSize.x - size);

    const Vector2 location = {static_cast<float>(locWidthDist(mRandomNumberGenerator)), -static_cast<float>(locHeightDist(mRandomNumberGenerator))};
    circle->setLocation(location);

    std::uniform_int_distribution<> velocityDist(-1000,1000);
    circle->setVelocity({static_cast<float>(velocityDist(mRandomNumberGenerator)), static_cast<float>(velocityDist(mRandomNumberGenerator))});

    return circle;
}

template<>
std::unique_ptr<Rectangle> ChunkGenerator::createRandomCollisionObject(float chunkHeight, const Vector2& chunkSize)
{
    std::uniform_int_distribution<> sizeDist(50, 200);
    std::uniform_int_distribution<> colorDist(0,SDL_MAX_UINT8);

    unsigned int width = sizeDist(mRandomNumberGenerator);
    unsigned int height = sizeDist(mRandomNumberGenerator);
    std::unique_ptr<Rectangle> rectangle = std::make_unique<Rectangle>(width, height);

    const float r = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    const float g = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    const float b = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    rectangle->setColor({.r = r, .g = g, .b = b});
    rectangle->setCanCollideWithWindowBorder(true, true);
    rectangle->setGravity({});
    std::uniform_int_distribution<> locHeightDist(height + (chunkHeight - 1) * chunkSize.y,chunkHeight * chunkSize.y - height);
    std::uniform_int_distribution<> locWidthDist(width,chunkSize.x - width);

    const Vector2 location = {static_cast<float>(locWidthDist(mRandomNumberGenerator)), -static_cast<float>(locHeightDist(mRandomNumberGenerator))};
    rectangle->setLocation(location);

    std::uniform_int_distribution<> velocityDist(-1000,1000);
    rectangle->setVelocity({static_cast<float>(velocityDist(mRandomNumberGenerator)), static_cast<float>(velocityDist(mRandomNumberGenerator))});

    return rectangle;
}

static Vector2 getBiggestValue(const std::vector<Vector2>& vertices)
{
    Vector2 biggest = {};

    for (const Vector2& vertex : vertices)
    {
        if (biggest.squaredSize() < vertex.squaredSize())
        {
            biggest = vertex;
        }
    }

    return biggest;
}

template<>
std::unique_ptr<Polygon> ChunkGenerator::createRandomCollisionObject(float chunkHeight, const Vector2& chunkSize)
{
    std::uniform_int_distribution<> polygonIndexDist(0, 4);
    std::uniform_int_distribution<> colorDist(0,SDL_MAX_UINT8);

    const std::vector<Vector2> vertices = [](int index)
    {
        const std::vector<Vector2> preDefinedVertices[] = {
            {{43.2635f,6.8604f}, {55.0929f,-9.3619f}, {8.2399f,-52.0017f}, {-48.6525f,4.1456f}},
            {{-18.3162f,45.2245f}, {52.7055f,-5.7648f}, {29.5635f,-44.2779f}, {-29.4444f,39.4642f}},
            {{-41.6380f,14.0692f}, {37.0215f,23.5764f}, {39.4071f,-26.6079f}, {2.3210f,-56.8108f}, {-51.8513f,-18.9697f}},
            {{-12.3984f,53.5238f}, {35.1011f,36.6958f}, {-9.3180f,-54.6367f}, {-51.3585f,6.5760f}},
            {{-40.5856f,25.3067f}, {30.7374f,32.7841f}, {51.4345f,-19.5136f}, {20.8666f,-44.6675f}, {-4.0400f,-43.3833f}, {-39.2438f,-35.2537f}, {-41.8348f,-32.5844f}, {-52.0016f,-7.5905f}}
        };

        return preDefinedVertices[index];
    }(polygonIndexDist(mRandomNumberGenerator));
    
    std::unique_ptr<Polygon> polygon = std::make_unique<Polygon>(vertices);

    const float r = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    const float g = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    const float b = static_cast<float>(colorDist(mRandomNumberGenerator)) / SDL_MAX_UINT8;
    polygon->setColor({.r = r, .g = g, .b = b});
    polygon->setCanCollideWithWindowBorder(true, true);
    polygon->setGravity({});

    const Vector2 biggestVertex = getBiggestValue(vertices);
    const float biggestVertexSize = biggestVertex.size();
    
    std::uniform_int_distribution<> locHeightDist(biggestVertexSize + (chunkHeight - 1) * chunkSize.y,chunkHeight * chunkSize.y - biggestVertexSize);
    std::uniform_int_distribution<> locWidthDist(biggestVertexSize,chunkSize.x - biggestVertexSize);

    const Vector2 location = {static_cast<float>(locWidthDist(mRandomNumberGenerator)), -static_cast<float>(locHeightDist(mRandomNumberGenerator))};
    polygon->setLocation(location);

    std::uniform_int_distribution<> velocityDist(-1000,1000);
    polygon->setVelocity({static_cast<float>(velocityDist(mRandomNumberGenerator)), static_cast<float>(velocityDist(mRandomNumberGenerator))});

    return polygon;
}

void ChunkGenerator::spawnRandomObstacle(int chunkHeight, std::vector<std::unique_ptr<CollisionObject>>& outObstacles)
{
    for (uint32_t i = 0; i < 1; ++i)
    {
        const int ObstacleType = mObstacleTypeDistribution(mRandomNumberGenerator);

        switch (ObstacleType)
        {
        case 0:
            outObstacles.push_back(std::move(createRandomCollisionObject<Circle>(chunkHeight, mChunkSize)));
            break;
        case 1:
            outObstacles.push_back(std::move(createRandomCollisionObject<Rectangle>(chunkHeight, mChunkSize)));
            break;
        case 2:
            outObstacles.push_back(std::move(createRandomCollisionObject<Polygon>(chunkHeight, mChunkSize)));
            break;
        default:
            ;
        }
    }
}

ChunkGenerator::ChunkGenerator(const Vector2& inChunkSize, const uint32_t inPlatformPerChunkCount) : mChunkSize(inChunkSize), mPlatformPerChunkCount(inPlatformPerChunkCount), mRandomNumberGenerator(std::random_device()())
{
    const int halfChunkWidth = static_cast<int>(inChunkSize.x) / 2;
    const int halfChunkHeight = static_cast<int>(inChunkSize.y) / 2;
    mWidthDistribution = std::uniform_int_distribution<>(-halfChunkWidth,halfChunkWidth);
    mHeightDistribution = std::uniform_int_distribution<>(-halfChunkHeight, halfChunkHeight);
    mColorDistribution = std::uniform_real_distribution<float>(0.f, 1.f);
    mObstacleTypeDistribution = std::uniform_int_distribution<>(0, 2);
}

void ChunkGenerator::generateChunk(int chunkHeight, std::vector<std::unique_ptr<Platform>>& outPlatforms, std::vector<std::unique_ptr<CollisionObject>>& outObstacles)
{
    mChunkGenerationHeight = std::max(mChunkGenerationHeight, chunkHeight);

    for (uint32_t i = 0; i < mPlatformPerChunkCount; ++i)
    {
        const int xCoord = mWidthDistribution(mRandomNumberGenerator);
        const int yCoord = chunkHeight * -mChunkSize.y + mHeightDistribution(mRandomNumberGenerator);

        Vector2 position = {.x = static_cast<float>(xCoord), .y = static_cast<float>(yCoord)};

        const float r = mColorDistribution(mRandomNumberGenerator);
        const float g = mColorDistribution(mRandomNumberGenerator);
        const float b = mColorDistribution(mRandomNumberGenerator);
        
        std::unique_ptr<Platform> platform = std::make_unique<Platform>();
        platform->setLocation(position);
        platform->setColor({r, g, b});
        outPlatforms.push_back(std::move(platform));
    }

    spawnRandomObstacle(chunkHeight, outObstacles);
}

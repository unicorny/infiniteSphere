#ifndef __SC_RENDER_NOISE_PLANET_TO_TEXTURE__
#define __SC_RENDER_NOISE_PLANET_TO_TEXTURE__


class DRGeometrieHeightfield;
struct PlanetNoiseParameter;

enum PlanetNoiseParameterNames
{
    CONTINENT_FREQUENCY = 0,
    CONTINENT_LACUNARITY = 1,
    MOUNTAIN_LACUNARITY = 2,
    HILLS_LACUNARITY = 3,
    PLAINS_LACUNARITY = 4,
    BADLANDS_LACUNARITY = 5,
    MOUNTAIN_GLACIATION = 6,
    SEA_LEVEL = 7,
    SHELF_LEVEL = 8,
    MOUNTAINS_AMOUNT = 9,
    HILLS_AMOUNT = 10,
    BADLANDS_AMOUNT = 11,
    RIVER_DEPTH = 12,
    CONTINENT_HEIGHT_SCALE = 13,
    SEA_LEVEL_IN_METRES = 14,
    MAX_HEIGHT_IN_PERCENT = 15,
    MIN_HEIGHT_IN_PERCENT = 16,
    TERRAIN_OFFSET = 17,
    MOUNTAINS_TWIST = 18,
    HILLS_TWIST = 19,
    BADLANDS_TWIST = 20

};

struct PlanetNoiseParameter
{
    PlanetNoiseParameter()
    {memset(values, 0, sizeof(float)*15); }
    void print(bool toLog = false);
    const char* getFieldName(PlanetNoiseParameterNames feldName);
    union
    {
        struct
        {
            float continentFrequenzy;
            float continentLacunarity;
            float mountainLacunarity;
            float hillsLacunarity;
            float plainsLacunarity;
            float badlandsLacunarity;
            float mountainGlaciation;
            float seaLevel;
            float shelfLevel;
            float mountainAmount;
            float hillsAmount;
            float badlandsAmount;
            float riverDeapth;
            float continentHeightScale;
            float seaLevelInMetres;
            float maxHeightInPercent;
            float minHeightInPercent;
            float terrainOffset;
            float mountainsTwist;
            float hillsTwist;
            float badlandsTwist;
        };
        float values[21];
    };
};

class RenderNoisePlanetToTexture : public RenderInStepsToTexture
{
public:
    RenderNoisePlanetToTexture(const char* vertexShaderName, const char* fragmentShaderName, const PlanetNoiseParameter* noiseParameter);
    ~RenderNoisePlanetToTexture();

    //__inline__ void update(float patchScaling) {mPatchScaling = patchScaling;}
    DRReturn init(int stepSize, DRVector3 translate, float patchScaling, DRTexturePtr texture, const DRMatrix& rotation = DRMatrix::identity());

    virtual DRReturn renderStuff();

    __inline__ const DRMatrix& getRotationsMatrix() {return mRotation;}
    __inline__ float getPatchScaling() {return mPatchScaling;}

protected:
private:
    ShaderProgram* mShader;
    DRGeometrieHeightfield* mRenderSphere;
    const PlanetNoiseParameter*      mNoiseParameter;
    float         mPatchScaling;
    DRVector3     mTranslate;
    DRMatrix      mRotation;


};


#endif //__SC_RENDER_NOISE_PLANET_TO_TEXTURE__

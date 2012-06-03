#include "main.h"

RenderNoisePlanetToTexture::RenderNoisePlanetToTexture(const char* vertexShaderName, const char* fragmentShaderName, const PlanetNoiseParameter* noiseParameter)
: RenderInStepsToTexture(), mShader(NULL), mRenderSphere(NULL), mNoiseParameter(noiseParameter)
{
    if(!vertexShaderName || !fragmentShaderName)
        LOG_ERROR_VOID("Fehler kein Shader");

    mShader = ShaderManager::Instance().getShader(vertexShaderName, fragmentShaderName);
    mRenderSphere = DRGeometrieManager::Instance().getGrid(50, GEO_FULL, GEO_VERTEX_TRIANGLE_STRIP);

}

RenderNoisePlanetToTexture::~RenderNoisePlanetToTexture()
{
    ShaderManager::Instance().releaseShader(mShader);
    DRGeometrieManager::Instance().freeGrid(50, GEO_FULL, GEO_VERTEX_TRIANGLE_STRIP);
}

DRReturn RenderNoisePlanetToTexture::renderStuff()
{
    if(!mNoiseParameter) LOG_ERROR("no noise parameter", DR_ERROR);
    mShader->bind();

    //mShader->setUniform3fv("SphericalCenter", DRVector3(0.0f, 0.0f, -1.0f*(1.0f-mH)));
	mShader->setUniform1f("patchScaling", mPatchScaling);
    mShader->setUniform3fv("translate", mTranslate);

    //printf("theta:%f\n", 90.0*GRADTORAD);

    // Frequency of the planet's continents.  Higher frequency produces smaller,
    // more numerous continents.  This value is measured in radians.
	mShader->setUniform1f("CONTINENT_FREQUENCY", mNoiseParameter->continentFrequenzy);

    // Lacunarity of the planet's continents.  Changing this value produces
    // slightly different continents.  For the best results, this value should
    // be random, but close to 2.0.
	mShader->setUniform1f("CONTINENT_LACUNARITY", mNoiseParameter->continentLacunarity);

    // Lacunarity of the planet's mountains.  Changing this value produces
    // slightly different mountains.  For the best results, this value should
    // be random, but close to 2.0.
	mShader->setUniform1f("MOUNTAIN_LACUNARITY", mNoiseParameter->mountainLacunarity);

    // Lacunarity of the planet's hills.  Changing this value produces slightly
    // different hills.  For the best results, this value should be random, but
    // close to 2.0.
	mShader->setUniform1f("HILLS_LACUNARITY", mNoiseParameter->hillsLacunarity);

    // Lacunarity of the planet's plains.  Changing this value produces slightly
    // different plains.  For the best results, this value should be random, but
    // close to 2.0.
	mShader->setUniform1f("PLAINS_LACUNARITY", mNoiseParameter->plainsLacunarity);

    // Lacunarity of the planet's badlands.  Changing this value produces
    // slightly different badlands.  For the best results, this value should be
    // random, but close to 2.0.
	mShader->setUniform1f("BADLANDS_LACUNARITY", mNoiseParameter->badlandsLacunarity);

    // Specifies the amount of "glaciation" on the mountains.  This value
    // should be close to 1.0 and greater than 1.0.
	mShader->setUniform1f("MOUNTAIN_GLACIATION", mNoiseParameter->mountainGlaciation);

    // Specifies the planet's sea level.  This value must be between -1.0
    // (minimum planet elevation) and +1.0 (maximum planet elevation.)
	mShader->setUniform1f("SEA_LEVEL", mNoiseParameter->seaLevel);

    // Specifies the level on the planet in which continental shelves appear.
    // This value must be between -1.0 (minimum planet elevation) and +1.0
    // (maximum planet elevation), and must be less than SEA_LEVEL.
	mShader->setUniform1f("SHELF_LEVEL", mNoiseParameter->shelfLevel);

    // Determines the amount of mountainous terrain that appears on the
    // planet.  Values range from 0.0 (no mountains) to 1.0 (all terrain is
    // covered in mountains).  Mountainous terrain will overlap hilly terrain.
    // Because the badlands terrain may overlap parts of the mountainous
    // terrain, setting MOUNTAINS_AMOUNT to 1.0 may not completely cover the
    // terrain in mountains.
	mShader->setUniform1f("MOUNTAINS_AMOUNT", mNoiseParameter->mountainAmount);

    // Determines the amount of hilly terrain that appears on the planet.
    // Values range from 0.0 (no hills) to 1.0 (all terrain is covered in
    // hills).  This value must be less than MOUNTAINS_AMOUNT.  Because the
    // mountainous terrain will overlap parts of the hilly terrain, and
    // the badlands terrain may overlap parts of the hilly terrain, setting
    // HILLS_AMOUNT to 1.0 may not completely cover the terrain in hills.
	mShader->setUniform1f("HILLS_AMOUNT", mNoiseParameter->hillsAmount);

    // Determines the amount of badlands terrain that covers the planet.
    // Values range from 0.0 (no badlands) to 1.0 (all terrain is covered in
    // badlands.)  Badlands terrain will overlap any other type of terrain.
	mShader->setUniform1f("BADLANDS_AMOUNT", mNoiseParameter->badlandsAmount);

    // Maximum depth of the rivers, in planetary elevation units.
	mShader->setUniform1f("RIVER_DEPTH", mNoiseParameter->riverDeapth);

    // Scaling to apply to the base continent elevations, in planetary elevation
    // units.
	mShader->setUniform1f("CONTINENT_HEIGHT_SCALE", mNoiseParameter->continentHeightScale);

    // Calculate the sea level, in meters.
	mShader->setUniform1f("SEA_LEVEL_IN_METRES", mNoiseParameter->seaLevelInMetres);
    //printf("seaLevelInMeters: %f\n", seaLevelInMeters);

    // Offset to apply to the terrain type definition.  Low values (< 1.0) cause
    // the rough areas to appear only at high elevations.  High values (> 2.0)
    // cause the rough areas to appear at any elevation.  The percentage of
    // rough areas on the planet are independent of this value.
    mShader->setUniform1f("TERRAIN_OFFSET", mNoiseParameter->terrainOffset);

    // Specifies the "twistiness" of the mountains.
    mShader->setUniform1f("MOUNTAINS_TWIST", mNoiseParameter->mountainsTwist);

    // Specifies the "twistiness" of the hills.
    mShader->setUniform1f("HILLS_TWIST", mNoiseParameter->hillsTwist);

    // Specifies the "twistiness" of the badlands.
    mShader->setUniform1f("BADLANDS_TWIST", mNoiseParameter->badlandsTwist);

    mShader->setUniformMatrix("projection", mProjectionMatrix);
	mShader->setUniformMatrix("texture", mRotation);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mRenderSphere->render();

    mShader->unbind();
    return DR_OK;
}

DRReturn RenderNoisePlanetToTexture::init(int stepSize, DRVector3 translate, float patchScaling, DRTexturePtr texture, const DRMatrix& rotation /*= DRMatrix::Identity()*/)
{
    mPatchScaling = patchScaling;
    mTranslate = translate;
    mRotation = rotation;
    float clippingPlanes[4] = {1.0f, -1.0f, 1.0f, -1.0f};

    return RenderInStepsToTexture::init(stepSize, clippingPlanes, texture);
}

//*************************************************************************************************
// PlanetNoiseParameter
//*************************************************************************************************
void PlanetNoiseParameter::print(bool toLog /*= false*/)
{
    if(toLog)
    {
        DREngineLog.writeToLog("----- PlanetNoiseParameter Begin ----");
        for(int i = 0; i < 21; i++)
        {
            DREngineLog.writeToLog("%s: %.4f", getFieldName(static_cast<PlanetNoiseParameterNames>(i)), values[i]);
        }
        DREngineLog.writeToLog("----- PlanetNoiseParameter Ende ----");
    }
    else
    {
        printf("----- PlanetNoiseParameter Begin ----");
        for(int i = 0; i < 21; i++)
        {
            printf("%s: %.4f", getFieldName(static_cast<PlanetNoiseParameterNames>(i)), values[i]);
        }
        printf("----- PlanetNoiseParameter Ende ----");
    }
}

const char* PlanetNoiseParameter::getFieldName(PlanetNoiseParameterNames feldName)
{
    switch(feldName)
    {
        case CONTINENT_FREQUENCY: return "CONTINENT_FREQUENCY";
        case CONTINENT_LACUNARITY: return "CONTINENT_LACUNARITY";
        case MOUNTAIN_LACUNARITY: return "MOUNTAIN_LACUNARITY";
        case HILLS_LACUNARITY: return "HILLS_LACUNARITY";
        case PLAINS_LACUNARITY: return "PLAINS_LACUNARITY";
        case BADLANDS_LACUNARITY: return "BADLANDS_LACUNARITY";
        case MOUNTAIN_GLACIATION: return "MOUNTAIN_GLACIATION";
        case SEA_LEVEL: return "SEA_LEVEL";
        case SHELF_LEVEL: return "SHELF_LEVEL";
        case MOUNTAINS_AMOUNT: return "MOUNTAINS_AMOUNT";
        case HILLS_AMOUNT: return "HILLS_AMOUNT";
        case BADLANDS_AMOUNT: return "BADLANDS_AMOUNT";
        case RIVER_DEPTH: return "RIVER_DEPTH";
        case CONTINENT_HEIGHT_SCALE: return "CONTINENT_HEIGHT_SCALE";
        case SEA_LEVEL_IN_METRES: return "SEA_LEVEL_IN_METRES";
        case MAX_HEIGHT_IN_PERCENT: return "MAX_HEIGHT_IN_PERCENT";
        case MIN_HEIGHT_IN_PERCENT: return "MIN_HEIGHT_IN_PERCENT";
        case TERRAIN_OFFSET: return "TERRAIN_OFFSET";
        case MOUNTAINS_TWIST: return "MOUNTAINS_TWIST";
        case HILLS_TWIST: return "HILLS_TWIST";
        case BADLANDS_TWIST: return "BADLANDS_TWIST";
        default: return "-- invalid --";
    }

    return "-- error --";
}

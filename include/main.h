#include "Engine2Main.h"

#include "DRCubeTexture.h"

#include <vector>
#include <sstream>
#include <iostream>
#include "Eigen/Dense"
#include "Eigen/Geometry"

//extern DRMatrix rotationMatrix;
#include "GlobalRenderer.h"
#include "DRGeometrieHeightfield.h"
#include "DRGeometrieManager.h"
#include "ShaderProgram.h"
#include "ShaderManager.h"
#include "RenderInStepsToTexture.h"
#include "MipMapTexture.h"
#include "MipMapCubeTexture.h"

#include "RenderNoisePlanetToTexture.h"



#include "Unit.h"
#include "Vector3Unit.h"
#include "Camera.h"



struct ControlMode
{
    ControlMode() {}
    ControlMode(Unit value) : mValue(value) {}
    ControlMode(double value, UnitTypes type) : mValue(value, type) {}
    Unit mValue;
};

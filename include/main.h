#include "Engine2Main.h"

#include "DRCubeTexture.h"

#include <vector>
#include <sstream>
#include <iostream>
#include "Eigen/Dense"
#include "Eigen/Geometry"

extern DRMatrix rotationMatrix;

#include "InfiniteSphere.h"
#include "ShaderProgram.h"
#include "RenderToTexture.h"
#include "RenderInStepsToTexture.h"
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
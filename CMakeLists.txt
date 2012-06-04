PROJECT(infiniteSphere CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(include import/include)

SET(infiniteSphereSrc src/main src/ShaderProgram src/Camera src/Unit
	src/RenderNoisePlanetToTexture src/RenderInStepsToTexture  src/ShaderManager src/DRGeometrieManager src/DRGeometrieHeightfield
	src/GlobalRenderer src/MipMapCubeTexture src/MipMapTexture src/RenderMipMapToTexture)
IF(WIN32)
SET(infiniteSphereSrc ${infiniteSphereSrc} include/main.h include/ShaderProgram.h include/Camera.h include/Unit.h
	include/RenderNoisePlanetToTexture.h include/RenderInStepsToTexture.h include/ShaderManager.h
	include/DRGeometrieManager.h include/DRGeometrieHeightfield.h include/GlobalRenderer.h include/MipMapTexture.h
	include/MipMapCubeTexture.h include/RenderMipMapToTexture.h)
ENDIF(WIN32)


SET(PA import/bin import/lib)
find_library(SDL_LIB SDL PATHS ${PA})
find_library(DR_ENGINE_LIB DREngine2 PATHS ${PA} REQUIRED)
find_library(DR_CORE_LIB DRCore2 PATHS ${PA} REQUIRED)
if(WIN32)
SET(infiniteSphereLibs ${SDL_LIB} ${DR_ENGINE_LIB} ${DR_CORE_LIB} glu32 opengl32)
else()
SET(infiniteSphereLibs ${SDL_LIB} DREngine2 DRCore2 glut)
endif()

if (MSVC)
ADD_DEFINITIONS(-DNODEFAULTLIB:library)
endif(MSVC)

ADD_EXECUTABLE(infiniteSphere ${infiniteSphereSrc})

TARGET_LINK_LIBRARIES(infiniteSphere ${infiniteSphereLibs})




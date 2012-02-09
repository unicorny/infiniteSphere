PROJECT(infiniteSphere CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(include import/include)

SET(infiniteSphereSrc src/main src/ShaderProgram src/InfiniteSphere src/Camera src/Unit src/RenderToTexture src/RenderNoisePlanetToTexture src/RenderInStepsToTexture  src/DRCubeTexture)
#IF(WIN32)
#SET(infiniteSphereSrc ${infiniteSphereSrc} ShaderProgram.h main.h InfiniteSphere.h)
#ENDIF(WIN32)


SET(PA import/bin import/lib)
find_library(SDL_LIB SDL PATHS ${PA})
find_library(DR_ENGINE2_LIB Engine2 PATHS ${PA} REQUIRED)
find_library(DR_CORE2_LIB Core2 PATHS ${PA} REQUIRED)
if(WIN32)
SET(infiniteSphereLibs ${SDL_LIB} ${DR_ENGINE2_LIB} ${DR_CORE2_LIB} glu32 opengl32)
else()
SET(infiniteSphereLibs ${SDL_LIB} Engine2 Core2 glut)
endif()

if (MSVC)
ADD_DEFINITIONS(-DNODEFAULTLIB:library)
endif(MSVC)

ADD_EXECUTABLE(infiniteSphere ${infiniteSphereSrc})

TARGET_LINK_LIBRARIES(infiniteSphere ${infiniteSphereLibs})




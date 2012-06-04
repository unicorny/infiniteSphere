#include "main.h"

Camera          camera(DRVector3(0.0f, 0.0f, 20.0f));
const Unit      planetRadius = Unit(6378, KM); //Erde
//const Unit      planetRadius = Unit(695999.25, KM); // Sonne
//const Unit      planetRadius = Unit(1066.1559366853, AE).convertTo(KM); // VY Canis Majoris

ShaderProgram*   shader;
DRFont* g_Font = NULL;
GLUquadricObj*  quadratic = NULL;
DRTexturePtr    texture;;
bool            wireframe = false;

Eigen::Affine3f EigenAffine;
Eigen::Quaternionf  rotationQ;
Eigen::Quaternionf  rotationQOld;

MipMapCubeTexture* mipmap;

float stepSize = 32.0f;
int  renderMode = 0;


#define MAX_CONTROL_MODES 9
ControlMode gControlModes[MAX_CONTROL_MODES];
int gCurrentControlMode = 0;

//DRReturn renderToTexture(double theta, float h);


#ifdef _WIN32
#undef main
#endif

using Eigen::MatrixXd;

DRReturn load()
{
    if(EnInit_OpenGL(1.0f, DRVideoConfig(800, 600), "Infinite Sphere"))
    {
        printf("Fehler bei Engine Init\n");
        return DR_ERROR;
    }


    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);

    ShaderManager::Instance().init();
    DRGeometrieManager::Instance().init();

    DRIni ini("data/config.ini");
    if(!ini.isValid()) LOG_ERROR("non valid config", DR_ERROR);


    printf("stepSize: %f, renderMode: %d\n", stepSize, renderMode);

	//check maximal multitextures
	int textureCount = 0;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &textureCount);
	DREngineLog.writeToLog("Multitexturing supported up to: %d textures\n", textureCount);
	if(textureCount < 4) LOG_ERROR("Multitexturing mit mindestens 4 Texturen wird nicht unterstuetzt!", DR_ERROR);	

    g_Font = new DRFont();
    g_Font->init("data/MalgunGothic.tga", "data/MalgunGothic.tbf");

    //Steuerung
    gControlModes[0].mValue = Unit(20, M);
    gControlModes[1].mValue = Unit(0.100, KM);
    gControlModes[2].mValue = Unit(10, KM);
    gControlModes[3].mValue = Unit(1000, KM);
    gControlModes[4].mValue = Unit(20000, KM);
    gControlModes[5].mValue = Unit(400000, KM);
    gControlModes[6].mValue = Unit(0.1, AE);
    gControlModes[7].mValue = Unit(10, AE);
    gControlModes[8].mValue = Unit(500, AE);

    camera.setSektorPosition(Vector3Unit(0.0, 0.0, 10000.0, KM));
    //camera.setSektorPosition(Vector3Unit(0.0, 0.0, 700000.0, KM));
    //camera.setSektorPosition(Vector3Unit(0.0, 0.0, 1070.20, AE));

    quadratic = gluNewQuadric(); // erzeugt einen Zeiger auf das Quadric Obkect ( NEU )
    if(!quadratic) LOG_ERROR("error by call gluNewQuadric", DR_ERROR);
    gluQuadricNormals(quadratic, GLU_SMOOTH); // erzeugt Normalen ( NEU )
    gluQuadricTexture(quadratic, GL_TRUE); // erzeugt Textur Koordinaten ( NEU )

    texture = DRTextureManager::Instance().getTexture("data/test.tga");

	mipmap = new MipMapCubeTexture;

    shader = ShaderManager::Instance().getShader("data/shader/sphere.vert", "data/shader/sphere.frag");
    if(!shader)
        LOG_ERROR("Fehler bei load shader", DR_ERROR);

    Uint32 start = SDL_GetTicks();
    //renderToTexture(1.0f);
    DRLog.writeToLog("zeit fuer renderToTexture: %f Sekunden", static_cast<double>(SDL_GetTicks()-start)/1000.0f);
    

    camera.setProjectionMatrix(45.0f, (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 10000.0f);


    return DR_OK;
}

void exit()
{
    if(quadratic)
        gluDeleteQuadric(quadratic);
    ShaderManager::Instance().exit();
    DRGeometrieManager::Instance().exit();
    DR_SAVE_DELETE(g_Font);
    //DR_SAVE_DELETE(texture);
	texture.release();
	DR_SAVE_DELETE(mipmap);

    EnExit();
}



DRReturn render(float fTime)
{
    //berechnugnen
    // Werte für Krümmung
    Vector3Unit cameraPlanet = -camera.getSektorPosition();
    Unit l = cameraPlanet.length();
    double theta = acos(planetRadius/l); // if theta < 0.5 Grad, using ebene
    float spherePartH = 1.0-planetRadius/l;

    //printf("\rtheta: %f (%f Grad)", theta, theta*RADTOGRAD);

    DRVector3 cameraIntersectionPlanet = camera.getSektorPosition().getVector3().normalize();

    DRVector3 startAxis(0.0001f, 0.0001f, 1.0000f);
  //  if(rotation_ZAxis.z < 0.0f)
    //    startAxis.z *= -1.0f;

    DRVector3 newAxis = startAxis.cross(cameraIntersectionPlanet).normalize();
    float newAngle = startAxis.dot(cameraIntersectionPlanet);

    //Eigen::Matrix3f m2;
    //Eigen::Affine3f prev = EigenAffine;
    Eigen::Quaternionf q(Eigen::AngleAxisf(acosf(newAngle), Eigen::Vector3f(newAxis.x, newAxis.y, newAxis.z)));
    EigenAffine = Eigen::AngleAxisf(acosf(newAngle), Eigen::Vector3f(newAxis.x, newAxis.y, newAxis.z));
    //EigenAffine = q;
//    rotationMatrix = DRMatrix(EigenAffine.data());


	glClearColor(0.1, 0.2, 0.0, 0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);

    glDisable(GL_TEXTURE_2D);
    //glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);


    //Reseten der Matrixen
    glViewport(0, 0, g_pSDLWindow->w, g_pSDLWindow->h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluPerspective(g_Player.getCameraFOV(), (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 2000.0f);
    glMultMatrixf(camera.getProjectionMatrix());
    glMatrixMode(GL_MODELVIEW);          // Select the modelview matrix

    glLoadIdentity();                    // Reset (init) the modelview matrix
    camera.setCameraMatrix();

    glEnable(GL_DEPTH_TEST);             // Enables depth test
   /*
    //Boden
    //glColor3f(0.2f, 0.5f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, -10.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(1.0, 0.0f);
        glVertex3f( 50.0f, 0.0f, -50.0f);

        glColor3f(0.0f, 1.0f, 1.0f);
        glTexCoord2f(0.0, 0.0f);
        glVertex3f(-50.0f, 0.0f, -50.0f);

        glColor3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0, 1.0f);
        glVertex3f(-50.0f, 0.0f,  50.0f);

        glColor3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0, 1.0f);
        glVertex3f( 50.0f, 0.0f,  50.0f);
    glEnd();
    glPopMatrix();
    //*/
     if(wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //Kugel


    glLoadIdentity();
    
    /*
    DRMatrix projBack = camera.getProjectionMatrix().invert();
    glScalef(0.15f, 0.1f, 1.0f);
    glBegin(GL_QUADS);
        glVertex3fv(DRVector3(-1.0f, 1.0f, 0.0f).transformCoords(projBack));
        glVertex3fv(DRVector3(-1.0f, -1.0f, 0.0f).transformCoords(projBack));
        glVertex3fv(DRVector3(1.0f, -1.0f, 0.0f).transformCoords(projBack));
        glVertex3fv(DRVector3(1.0f, 1.0f, 0.0f).transformCoords(projBack));
    glEnd();
    glLoadIdentity();*/
    //glTranslatef(0.0f, 0.0f, 0.0f);
    camera.setCameraMatrixRotation();



    // skalierung
    Unit distance1 = (-camera.getSektorPosition()).length();
	distance1 = distance1.convertTo(planetRadius.getType());
	double distance2 = 200.0f;
	Unit radius1 = planetRadius;
	double radius2 = ((radius1 * distance2) / distance1);

    Vector3Unit pos = -camera.getSektorPosition().normalize();
    pos *= (distance2);

    if(theta*RADTOGRAD > 0.35)
    {

        //kugel mittelpunkt bewegen und auf sichtbare größe skalieren
        glTranslated(static_cast<double>(pos.x), static_cast<double>(pos.y), static_cast<double>(pos.z));

    // glTranslatef(pos.x, pos.y, pos.z);
        //radius2 *= 0.5f;
        glScaled(radius2, radius2, radius2);

		

        //Camera test(DRVector3(0.0f, 0.0f, 20.0f));
        //test.lookAt(DRVector3(0.0f));
        //test.setProjectionMatrix(45.0f, (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 10000.0f);
/*		DRVector3 points[2][4];
		int border = 5;
		GLfloat modelview[16];
		glGetFloatv( GL_MODELVIEW_MATRIX, modelview );
		DRMatrix model(modelview);

		unproject(border, YHEIGHT-border, model, points[1][0], points[0][0]);
		unproject(border, border, model, points[1][1], points[0][1]);
		unproject(XWIDTH-border, border, model, points[1][2], points[0][2]);
		unproject(XWIDTH-border, YHEIGHT-border, model, points[1][3], points[0][3]);
	
		glBegin(GL_LINES);
		for(int i = 0; i<4; i++)
		{
			points[0][i] -= points[1][i];
			glVertex3fv(points[1][i]);
			glVertex3fv(points[1][i] + points[0][i]);
		}
		glEnd();
		glBegin(GL_QUADS);
		for(int i = 0; i<4; i++)
		{
			glVertex3fv(points[1][i] + points[0][i]);
		}
		glEnd();
*/
        glDisable(GL_TEXTURE_2D);
        //texture->bind();
        //renderTarget->bindTexture();
        //mRender2->bindTexture();
        //glBindTexture(GL_TEXTURE_2D, tempTexture2);
        glColor3f(1.0f, 1.0f, 1.0f);
        //gluSphere(quadratic, 1.0f, 128, 64);
        glDisable(GL_TEXTURE_2D);



    //*/

        glColor3f(1.0f, 1.0f, 1.0f);

        cameraIntersectionPlanet -= cameraIntersectionPlanet*spherePartH;
    }
    else
    {
        //cameraIntersectionPlanet -= cameraIntersectionPlanet*spherePartH;
        cameraIntersectionPlanet *= -1.0f;
    }
/*
    //achsen
    glBegin(GL_LINES);
    // x-Achse
        glColor3f(0.5f, 0.0f, 0.0f);
        glVertex3f(-2.0f, 0.0f, 0.0f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(2.0f, 0.0f, 0.0f);
    // y-Achse
        glColor3f(0.0f, 0.0f, 0.5f);
        glVertex3f(0.0f, -2.0f, 0.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 2.0f, 0.0f);
    // z-Achse
        glColor3f(0.0f, 0.5f, 0.0f);
        glVertex3f(0.0f, 0.0f, -2.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 2.0f);
    glEnd();

//*/

	DRMatrix lastTransformation = DRMatrix::identity();

	//lastTransformation *= DRMatrix::translation(DRVector3(0.0f, 0.0f, 1.0f-spherePartH));
	lastTransformation *= DRMatrix(EigenAffine.data());


	//glMultMatrixf(DRMatrix(mTest->getRotationsMatrix()/rotationMatrix));
	mipmap->updateTexture(pos.getVector3().normalize(), &camera, lastTransformation, theta);

   glMultMatrixf(EigenAffine.data());

   glTranslatef(0.0f, 0.0f, 1.0f-spherePartH);

    //glMultMatrixf(mTest->getRotationsMatrix());

   

    glEnable(GL_TEXTURE_2D);
    //texture->bind();
/*
    if(!renderMode)
        renderTarget->bindTexture();
    else
        mTest->bindTexture();
        */


    shader->bind();

    shader->setUniform1f("theta", theta);
    shader->setUniform3fv("SphericalCenter", DRVector3(0.0f, 0.0f, 1.0f-spherePartH));
//    glUniform1f(theta2Location, static_cast<float>(mTest->getTheta()));
	
	if(wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDisable(GL_TEXTURE_2D);
    if(radius2 <= 200.0f)
        //sphere->render();
        DRGeometrieManager::Instance().getGrid(100, GEO_FULL, GEO_VERTEX_TRIANGLE_STRIP)->render();

    shader->unbind();

    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //render text
    //FPS
    g_Font->begin();

    DRText text(g_Font);
    text.setFlags(DR_FF_RELATIVE | DR_FF_RELATIVSCALING);
    text.setText("FPS: %.0f", 1.0f/fTime);
    text.setPosition(DRVector2(0.0f, 0.0f));
    text.setColor12(DRColor(0.8f, 0.5f, 0.1f));
    text.setScaling(DRVector2(1.0f));
    text.drawText();

    text.setText("Distance: %s", Unit(distance1-planetRadius).print().data());
    text.setPosition(DRVector2(0.0f, 0.04f));
    text.drawText();

    text.setText("Steuerung: %d - %s/s", gCurrentControlMode+1, gControlModes[gCurrentControlMode].mValue.print().data());
    text.setPosition(DRVector2(0.0f, 0.08f));
    text.drawText();

    g_Font->end();

	if(GlobalRenderer::Instance().renderTasks())
		LOG_ERROR("Fehler bei calling GlobalRenderer::renderTasks", DR_ERROR);


    return DR_OK;
}


DRReturn move(float fTime)
{
    float fRotSpeed = 2.0f;
    float fSpeed = 20.0f;
	int numKeys = 0;
    //Kamera
#if SDL_VERSION_ATLEAST(1,3,0)
	Uint8 *keystate = SDL_GetKeyboardState(&numKeys);
#else
	Uint8 *keystate = SDL_GetKeyState(NULL);
#endif

    int mouseMove_x = 0, mouseMove_y = 0;
    // holen der Maus bewegung seit letztem frame und der bitmaks welche Tasten gedrückt sind
    Uint8 mousePressed = SDL_GetRelativeMouseState(&mouseMove_x, &mouseMove_y);


     // die Kamera wird rotiert, gesteuert durch die Tasten w, s (x Achse, hoch/runter), <-, -> (y Achse links/rechts), e und q (z Achse seitlich)
    camera.rotateRel(DRVector3(keystate[SDLK_s]-keystate[SDLK_w], keystate[SDLK_RIGHT]-keystate[SDLK_LEFT], keystate[SDLK_q]-keystate[SDLK_e])*fTime);
    // wenn die rechte maustaste gedrückt ist
    if((mousePressed & 4) == 4)
        // wird die Kamera auch abhängig von der Mausposition gedreht
    camera.rotateRel(DRVector3(mouseMove_y, mouseMove_x, 0.0f)*fTime*fRotSpeed);


//    if(gControlModes[gCurrentControlMode].mValue.getType() == M)
  //      camera.translateRel(DRVector3(keystate[SDLK_d]-keystate[SDLK_a], keystate[SDLK_PAGEUP]-keystate[SDLK_PAGEDOWN], keystate[SDLK_DOWN]-keystate[SDLK_UP])*fTime*gControlModes[gCurrentControlMode].mValue);
    //else
        camera.translateRel_SektorPosition(DRVector3(keystate[SDLK_d]-keystate[SDLK_a], keystate[SDLK_PAGEUP]-keystate[SDLK_PAGEDOWN], keystate[SDLK_DOWN]-keystate[SDLK_UP])*fTime*gControlModes[gCurrentControlMode].mValue, gControlModes[gCurrentControlMode].mValue.getType());

    //camera.lookAt_SektorPosition(Vector3Unit(0.0f, KM), camera.getYAxis());

    //set control mode
    if(EnIsButtonPressed(SDLK_1)) gCurrentControlMode = 0;
    else if(EnIsButtonPressed(SDLK_2)) gCurrentControlMode = 1;
    else if(EnIsButtonPressed(SDLK_3)) gCurrentControlMode = 2;
    else if(EnIsButtonPressed(SDLK_4)) gCurrentControlMode = 3;
    else if(EnIsButtonPressed(SDLK_5)) gCurrentControlMode = 4;
    else if(EnIsButtonPressed(SDLK_6)) gCurrentControlMode = 5;
    else if(EnIsButtonPressed(SDLK_7)) gCurrentControlMode = 6;
    else if(EnIsButtonPressed(SDLK_8)) gCurrentControlMode = 7;
    else if(EnIsButtonPressed(SDLK_9)) gCurrentControlMode = 8;


    // R-Taste
    if(EnIsButtonPressed(SDLK_r)) wireframe = !wireframe;

    return DR_OK;
}


int main()
{
    if(load())
    {
        printf("Fehler bei load\n");
        exit();
        return -1;
    }
    EnGameLoop(move, render);
    exit();

    return 42;
}

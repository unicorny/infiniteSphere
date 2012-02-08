#include "main.h"

Camera          camera(DRVector3(0.0f, 0.0f, 20.0f));
const Unit      planetRadius = Unit(6378, KM); //Erde
//const Unit      planetRadius = Unit(695999.25, KM); // Sonne
//const Unit      planetRadius = Unit(1066.1559366853, AE).convertTo(KM); // VY Canis Majoris

ShaderProgram   shader;
ShaderProgram   shader2;
DRFont* g_Font = NULL;
GLUquadricObj*  quadratic = NULL; 
InfiniteSphere* sphere = NULL;
DRTextur*       texture = NULL;
bool            wireframe = false;

#define MAX_CONTROL_MODES 9
ControlMode gControlModes[MAX_CONTROL_MODES];
int gCurrentControlMode = 0;

#ifdef _WIN32
#undef main
#endif

DRReturn load()
{
    if(EnInit_OpenGL(1.0f, DRVideoConfig(800, 600), "Infinite Sphere"))
    {
        printf("Fehler bei Engine Init\n");
        return DR_ERROR;
    }
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glClearColor(0.1, 0.2, 0.0, 0);
    
    g_Font = new DRFont();
    g_Font->init("MalgunGothic.tga", "MalgunGothic.tbf");
    
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
    
    texture = new DRTextur("earth.jpg");
    
    sphere = new InfiniteSphere();
    DRVector3 edges[4];
    edges[0] = DRVector3(1.0f, 1.0f, 0.0f);
    edges[1] = DRVector3(-1.0f, 1.0f, 0.0f);
    edges[2] = DRVector3(1.0f, -1.0f, 0.0f);
    edges[3] = DRVector3(-1.0f, -1.0f, 0.0f);

    sphere->init(25, edges);
    
    if(shader.init("sphere.vert", "sphere.frag"))
        LOG_ERROR("Fehler bei load shader", DR_ERROR);
    if(shader2.init("noise.vert", "noise.frag"))
        LOG_ERROR("Fehler bei load shader2", DR_ERROR);
    
    return DR_OK;    
}

void exit()
{
    if(quadratic)
        gluDeleteQuadric(quadratic); 
    DR_SAVE_DELETE(g_Font);
    DR_SAVE_DELETE(texture);
    EnExit();
}

DRReturn render(float fTime)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);    
    
    
    glDisable(GL_TEXTURE_2D);
    //glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    
        
    //Reseten der Matrixen
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluPerspective(g_Player.getCameraFOV(), (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.1f, 2000.0f);
    glMultMatrixf(DRMatrix::view_frustum(45.0f, (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.001f, 1000.0f));
    glMatrixMode(GL_MODELVIEW);          // Select the modelview matrix

    glLoadIdentity();                    // Reset (init) the modelview matrix
    camera.setKameraMatrix();
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
    //glTranslatef(0.0f, 0.0f, 0.0f);
    camera.setKameraMatrixRotation();    
    
    //berechnugnen
    float radius = 10.0f;
    //DRVector3 cameraPlanet = -camera.getPosition();
    Vector3Unit cameraPlanet = -camera.getSektorPosition();
    //DRVector3 cameraIntersectionPlanet = camera.getPosition().normalize();
    Unit l = cameraPlanet.length();
    double theta = acos(planetRadius/l); // if theta < 0.5 Grad, using ebene
    float spherePartH = 1.0-planetRadius/l;
    double quadSize = sin(theta);//*2.0f;

    DRVector3 cameraIntersectionPlanet = camera.getSektorPosition().getVector3().normalize();
    
    
    //DRMatrix m1 = DRMatrix::rotationAxis(camera.getYAxis(), theta);
    //DRMatrix m2 = DRMatrix::rotationAxis(camera.getYAxis(), -theta);
    //DRVector3 v1 = cameraIntersectionPlanet.transformCoords(m1);
    //DRVector3 v2 = cameraIntersectionPlanet.transformCoords(m2);
    //v1 + (v2-v1)/2.0f
    
    Unit distance1 = (-camera.getSektorPosition()).length();    
	distance1 = distance1.convertTo(planetRadius.getType());
	double distance2 = 200.0f;
	Unit radius1 = planetRadius;
	double radius2 = ((radius1 * distance2) / distance1);
    
    //DRVector3 pos = (-camera.getSektorPosition()).getVector3().normalize();
    Vector3Unit pos = -camera.getSektorPosition().normalize();
	//DRVector3 relCameraPos = -pos*distance1/planetRadius;
	//pos *= (distance2+0.001f);
    pos *= (distance2);
/*   printf("\r %f %f %f, %.8f, %s  x:%s y:%s z:%s (%f %f %f)", pos.x, pos.y, pos.z, radius2, distance1.print().data(),
									   absCameraPosition.x.print().data(), absCameraPosition.y.print().data(),
									   absCameraPosition.z.print().data(), diff.x, diff.y, diff.z);
										   //*/
  
    //printf("\r radius2: %f", radius2);
    
   //printf("\r theta: %f, quadSize: %f, winkel: %f, h: %.8f, radius2: %.8f, camerapunkt: %f, %f, %f", theta*RADTOGRAD, quadSize, angle, spherePartH, radius2,
     //   cameraIntersectionPlanet.x, cameraIntersectionPlanet.y, cameraIntersectionPlanet.z);    
    float len = cameraIntersectionPlanet.length();
    float ta = atanf(cameraIntersectionPlanet.z/cameraIntersectionPlanet.x);
    float sa = asinf(cameraIntersectionPlanet.z/len);
    float ca = acosf(cameraIntersectionPlanet.x/len);
    
    DRVector2 uv;        
    DRVector3 vp(cameraIntersectionPlanet);
    uv.y = asinf(vp.y);
    uv.x = acosf(vp.x/cosf(uv.y));
    DRVector2 xy(vp.x-vp.z,
                 vp.y-vp.z);
    
    //uv /= (2.0f*PI);
    
    DRVector2 uv2;
    uv2.y = atan2f(vp.y, vp.x)/PI;
    uv2.x = acosf(vp.z / vp.length())/PI;//*0.5;
    
    if(cameraIntersectionPlanet.z < 0.0f)
    {
        //uv.x = 0.5f-uv.x + 0.5f;
        /*if(uv.y < 0.0f)
            uv.y = -0.25f-uv.y - 0.25f;
        else
            uv.y = 0.25f-uv.y + 0.25f;
       //*/
    }
    //uv.y = atan2f(vp.y, vp.x)/PI;
    //uv.x = sqrt(vp.x*vp.x+vp.y*vp.y);
    
    
    //if(cameraIntersectionPlanet.z < 0.0f)
        //uv.y = 0.5f-uv.y + 0.5f;
    
    
    DRVector3 point = DRVector3(0.0f, 1.0f, 0.0f);
	float angle = acosf(point.dot(DRVector3(0.0f, 0.0f, 1.0f)));
	DRVector3 axis = point.cross(cameraIntersectionPlanet);
	DRMatrix m = DRMatrix::rotationAxis(axis, -angle);
    //DRMatrix m = DRMatrix::rotationX(-angle);
    DRVector3 trans = cameraIntersectionPlanet.transformCoords(m);
    
    byte largest = 1; // +x
    if(fabs(vp.z) > fabs(vp.x) && fabs(vp.z) > fabs(vp.y))
        largest = 0; //+z
    else if(fabs(vp.y) > fabs(vp.x))
        largest = 4;//+y
    if     (1 == largest && vp.x < 0.0f) largest = 3; //-x
    else if(0 == largest && vp.z < 0.0f) largest = 2; //-z
    else if(4 == largest && vp.y < 0.0f) largest = 5; //-y
    DRVector3 sektorID;
    switch(largest)
    {
        case 0: sektorID = DRVector3( 0.0f, 0.0f, 1.0f); break;
        case 1: sektorID = DRVector3( 1.0f, 0.0f, 0.0f); break;
        case 2: sektorID = DRVector3( 0.0f, 0.0f,-1.0f); break;
        case 3: sektorID = DRVector3(-1.0f, 0.0f, 0.0f); break;
        case 4: sektorID = DRVector3( 0.0f, 1.0f, 0.0f); break;
        case 5: sektorID = DRVector3( 0.0f,-1.0f, 0.0f); break;
        default: LOG_ERROR("Fehler", DR_ERROR); 
    }

 //   printf("\r z/x: %f, atan(z/x): %f (%f Grad), asin(z/r): %f (%f Grad), acos(x/r):  %f (%f Grad), uv: (%f, %f) camerapunkt: %f, %f, %f", cameraIntersectionPlanet.z/cameraIntersectionPlanet.x,
   //     ta, ta*RADTOGRAD, sa, sa*RADTOGRAD, ca, ca*RADTOGRAD, uv.u, uv.v,
     //   cameraIntersectionPlanet.x, cameraIntersectionPlanet.y, cameraIntersectionPlanet.z);    
    //printf("\r uv: (%f, %f), uv2: (%f, %f) camerapunkt: %f, %f, %f, cos(theta): %f, theta: %f, trans: %f, %f, %f (%f), %d", uv.u, uv.v, uv2.u, uv2.v,
      //  cameraIntersectionPlanet.x, cameraIntersectionPlanet.y, cameraIntersectionPlanet.z,
        //cosf(theta), theta* RADTOGRAD, trans.x, trans.y, trans.z, angle*RADTOGRAD, largest);  
  //  printf("\r camerapunkt: %f, %f, %f, %d",
    //    cameraIntersectionPlanet.x, cameraIntersectionPlanet.y, cameraIntersectionPlanet.z,
      //  largest); 
    
    printf("\r camerapunkt: %f, %f, %f, theta: %f (%f Grad), cos(theta): %f, kruemmung: %f", 
        cameraIntersectionPlanet.x, cameraIntersectionPlanet.y, cameraIntersectionPlanet.z,
        theta, theta*RADTOGRAD, cos(theta), 1.0f-spherePartH);    
    
    
        
    if(theta*RADTOGRAD > 0.35)
    {
        //kugel mittelpunkt bewegen und auf sichtbare größe skalieren    
        glTranslated(static_cast<double>(pos.x), static_cast<double>(pos.y), static_cast<double>(pos.z));
    // glTranslatef(pos.x, pos.y, pos.z);
        //radius2 *= 0.5f;
        glScaled(radius2, radius2, radius2);
    
        glEnable(GL_TEXTURE_2D);
        texture->bind();
        glColor3f(1.0f, 1.0f, 1.0f);
        //gluSphere(quadratic, 1.0f, 128, 64);
        glDisable(GL_TEXTURE_2D);
    //*/
    
        glColor3f(1.0f, 1.0f, 1.0f);
        
        cameraIntersectionPlanet -= cameraIntersectionPlanet*spherePartH;
    }
    else
    {
        //cameraIntersectionPlanet *= 20.0f;
        quadSize = 50.0f;
        //cameraIntersectionPlanet -= cameraIntersectionPlanet*spherePartH;
        cameraIntersectionPlanet *= -1.0f;
    }
    
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
    
    // verschieben der plane, auf dem verbindungsvektor zwischen Planet und Kamera, abhängig vom Winkel         
    //glTranslatef(cameraIntersectionPlanet.x, cameraIntersectionPlanet.y, cameraIntersectionPlanet.z);
    //glMultMatrixf(planet.getMatrix());
        
    //glRotatef(-angle, rotateAxis.x, rotateAxis.y, rotateAxis.z);
    DRVector3 mZAxis = cameraIntersectionPlanet.normalize();
    DRVector3 mXAxis = camera.getYAxis().cross(mZAxis).normalize();
    //DRVector3 mXAxis = DRVector3(0.0f, 1.0f, 0.0f).cross(mZAxis).normalize();
    DRVector3 mYAxis = mZAxis.cross(mXAxis).normalize();
    glMultMatrixf(DRMatrix::axis(mXAxis, mYAxis, mZAxis).invert());
    //glMultMatrixf(camera.getMatrix().invert());
    //glMultMatrixf(DRMatrix::axis(camera.getXAxis(), camera.getYAxis(), camera.getZAxis()).invert());
    
    glTranslatef(0.0f, 0.0f, 1.0f-spherePartH);
    
    
    //DRLog.writeMatrixToLog(DRMatrix::axis(mXAxis, mYAxis, mZAxis), "rotation");
    
    // skalieren der Plane, abhängig vom Winkel
    //glScalef(quadSize, quadSize, quadSize);
    //glScaled(sin(theta), sin(theta), sin(theta));
    //glScaled(1.0, 1.0, 0.0);
    
    // Texture Matrix
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    //glTranslatef(cameraIntersectionPlanet.x, cameraIntersectionPlanet.y, cameraIntersectionPlanet.z);
    //glMultMatrixf(DRMatrix::axis(mXAxis, mYAxis, mZAxis));
    glMultMatrixf(DRMatrix::axis(mXAxis, mYAxis, mZAxis).invert());
    //glMultMatrixf(DRMatrix::axis(camera.getXAxis(), camera.getYAxis(), camera.getZAxis()).invert());
    //glScalef(quadSize, quadSize, 0.0f);
    double textureScale = (1.0-cos(theta));
    //glScaled(textureScale, textureScale, 1.0);
    
    glEnable(GL_TEXTURE_2D);
    texture->bind();
    
    shader.bind();
    
    int sphereCenterLocation = glGetUniformLocation(shader.getProgram(), "SphericalCenter");
    int sphereHLocation = glGetUniformLocation(shader.getProgram(), "sphereH");
    int cameraIntersectionPointLocation = glGetUniformLocation(shader.getProgram(), "CameraIntersectionPoint");
    int thetaLocation = glGetUniformLocation(shader.getProgram(), "theta");
    int camerauvLocation = glGetUniformLocation(shader.getProgram(), "camUV");
    int sektorIDLocation = glGetUniformLocation(shader.getProgram(), "sektorID");

    glUniform3fv(sphereCenterLocation, 1, static_cast<float*>(DRVector3(0.0f, 0.0f, -1.0f*(1.0f-spherePartH))));
    glUniform3fv(cameraIntersectionPointLocation, 1, static_cast<float*>(cameraIntersectionPlanet));
    glUniform3fv(sektorIDLocation, 1, static_cast<float*>(sektorID));
    glUniform1f(sphereHLocation, spherePartH);
    glUniform1f(thetaLocation, static_cast<float>(theta));
    glUniform2fv(camerauvLocation, 1, static_cast<float*>(uv));
        
    if(radius2 <= 200.0f)
        sphere->render();
    glDisable(GL_TEXTURE_2D);

    shader.unbind();
    
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
    camera.rotateRel(DRVector3(-mouseMove_y, -mouseMove_x, 0.0f)*fTime*fRotSpeed);


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
    if(EnIsButtonPressed(21)) wireframe = !wireframe;

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

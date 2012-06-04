#include "main.h"

Camera          camera(DRVector3(0.0f, 0.0f, 20.0f));
const Unit      planetRadius = Unit(6378, KM); //Erde
//const Unit      planetRadius = Unit(695999.25, KM); // Sonne
//const Unit      planetRadius = Unit(1066.1559366853, AE).convertTo(KM); // VY Canis Majoris

ShaderProgram   shader;
ShaderProgram   shader2;
RenderToTexture* renderTarget;
DRFont* g_Font = NULL;
GLUquadricObj*  quadratic = NULL; 
InfiniteSphere* sphere = NULL;
InfiniteSphere* renderSphere = NULL;
DRTextur*       texture = NULL;
GLuint          tempTexture = 0;
bool            wireframe = false;
DRMatrix rotationMatrix;
Eigen::Affine3f EigenAffine;
Eigen::Quaternionf  rotationQ;
Eigen::Quaternionf  rotationQOld;

float stepSize = 32.0f;
int  renderMode = 0;

RenderNoisePlanetToTexture* mTest = NULL;

#define MAX_CONTROL_MODES 9
ControlMode gControlModes[MAX_CONTROL_MODES];
int gCurrentControlMode = 0;

DRReturn renderToTexture(double theta, float h);


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
    
    DRIni ini("data/config.ini");
    if(!ini.isValid()) LOG_ERROR("non valid config", DR_ERROR);
    
    DRVector2 size = DRVector2(ini.getInt("RenderToTexte", "Resolution"));
    glGenTextures(1, &tempTexture);    
    glBindTexture(GL_TEXTURE_2D, tempTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            
    glTexImage2D(GL_TEXTURE_2D, 0, 4, size.x, size.y, 0,
		GL_RGBA, GL_FLOAT, 0);
    
    stepSize = static_cast<float>(ini.getInt("RenderToTexte", "StepSize"));
    renderMode = ini.getInt("RenderToTexte", "RenderMode");
    
    printf("stepSize: %f, renderMode: %d\n", stepSize, renderMode);
    
    
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
    
    texture = new DRTextur("data/texture.jpg");
    
    sphere = new InfiniteSphere();
    renderSphere = new InfiniteSphere();
    DRVector3 edges[4];
    edges[0] = DRVector3(1.0f, 1.0f, 0.0f);
    edges[1] = DRVector3(-1.0f, 1.0f, 0.0f);
    edges[2] = DRVector3(1.0f, -1.0f, 0.0f);
    edges[3] = DRVector3(-1.0f, -1.0f, 0.0f);

    sphere->init(100, edges);
    renderSphere->init(10, edges);
    
    if(shader.init("data/shader/sphere.vert", "data/shader/sphere.frag"))
        LOG_ERROR("Fehler bei load shader", DR_ERROR);
    if(shader2.init("data/shader/noise_little.vert", "data/shader/noise_little.frag"))
        LOG_ERROR("Fehler bei load shader2", DR_ERROR);
    
    renderTarget = new RenderToTexture;
    renderTarget->setup(DRVector2(DRVector2(ini.getInt("RenderToTexte", "Resolution2"))));
    
    mTest = new RenderNoisePlanetToTexture("data/shader/noise_little.vert", "data/shader/noise_little.frag", DRVector2(ini.getInt("RenderToTexte", "Resolution")));
    mTest->init(stepSize, PI/2.0f, 0.0f, DRMatrix::identity(), tempTexture);
    
    Uint32 start = SDL_GetTicks();
    //renderToTexture(1.0f);
    DRLog.writeToLog("zeit fuer renderToTexture: %f Sekunden", static_cast<double>(SDL_GetTicks()-start)/1000.0f);
    glClearColor(0.1, 0.2, 0.0, 0);
    
    return DR_OK;    
}

void exit()
{
    if(quadratic)
        gluDeleteQuadric(quadratic); 
    if(tempTexture)
        glDeleteTextures(1, &tempTexture);
    tempTexture = 0;
    DR_SAVE_DELETE(g_Font);
    DR_SAVE_DELETE(texture);
    DR_SAVE_DELETE(renderTarget);
    DR_SAVE_DELETE(sphere);
    DR_SAVE_DELETE(renderSphere);    
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
    rotationMatrix = DRMatrix(EigenAffine.data());        
    
    static int turn = 0;
    
    if(turn >= 1)
    {
        if(!renderMode || renderMode == 2)
                renderToTexture(theta, spherePartH);//spherePartH);
        //renderToTexture(90.0f*GRADTORAD, 1.0f);
        turn = 0;
    }
    turn++;
    //*/
    if(renderMode)
    {
        if(mTest->step() == DR_NOT_ERROR)
        {
            //bool test = true;
            //mTest->getFrameBuffer()->saveToImage("data/texture.jpg");
            mTest->init(stepSize, theta, spherePartH, rotationMatrix, tempTexture);
            rotationQOld = rotationQ;
            rotationQ = q;
        }
    }
   //**/ 
    
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
    glMultMatrixf(DRMatrix::view_frustum(45.0f, (GLfloat)XWIDTH/(GLfloat)YHEIGHT, 0.001f, 1000.0f));
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
    
        glEnable(GL_TEXTURE_2D);
        texture->bind();
        glColor3f(1.0f, 1.0f, 1.0f);
        //gluSphere(quadratic, 1.0f, 64, 32);
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
    
        
    glMultMatrixf(EigenAffine.data());
    
    glTranslatef(0.0f, 0.0f, 1.0f-spherePartH);
    
    
    // Texture Matrix
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    
    DRMatrix a = EigenAffine.data();
    DRMatrix b = Eigen::Affine3f(rotationQ).data();
    
    glMultMatrixf(b.invert());
    glMultMatrixf(a);    
    
    glActiveTexture (GL_TEXTURE1);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    b = Eigen::Affine3f(rotationQOld).data();
    
    glMultMatrixf(b.invert());
    glMultMatrixf(a);
    glActiveTexture (GL_TEXTURE0);
    
    
    //glMultMatrixf(mTest->getRotationsMatrix());    
    
    
    //glMultMatrixf(DRMatrix(mTest->getRotationsMatrix()/rotationMatrix));
       
    glEnable(GL_TEXTURE_2D);
    //texture->bind();
    
    if(!renderMode)
        renderTarget->bindTexture();
    else
        mTest->bindTexture();
    glActiveTexture (GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tempTexture);
    glActiveTexture (GL_TEXTURE0);  
    if(renderMode == 2)
    {
        glActiveTexture (GL_TEXTURE1);
        renderTarget->bindTexture();
        glActiveTexture (GL_TEXTURE0);  
        mTest->bindTexture();
    }
    
    shader.bind();
    
    int sphereCenterLocation = glGetUniformLocation(shader.getProgram(), "SphericalCenter");
    int thetaLocation = glGetUniformLocation(shader.getProgram(), "theta");
    int textureLocation  = glGetUniformLocation(shader.getProgram(), "texture");
    int texture2Location = glGetUniformLocation(shader.getProgram(), "texture2");
    
    glUniform3fv(sphereCenterLocation, 1, static_cast<float*>(DRVector3(0.0f, 0.0f, -1.0f*(1.0f-spherePartH))));
    glUniform1f(thetaLocation, static_cast<float>(theta));
        
    glUniform1i(textureLocation, 0);
    glUniform1i(texture2Location, 1);
         
    if(radius2 <= 200.0f)
        sphere->render();
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

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


DRReturn renderToTexture(double theta, float h)
{
    DRVector2 size = renderTarget->getSize();
    
    //DRLog.writeToLog("generate noisemap with %.0fx%.0f (=%.0f pixel)", size.x, size.y, size.x*size.y);

    
    renderTarget->bindToRender();
    
    //renderTarget.unbind();
    Uint32 start = SDL_GetTicks();
    //render stuff
   // glClearColor(0.0, 1.0, 1.0, 0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0f, 0.0f, 0.0f);    
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
           
    //Reseten der Matrixen
    glViewport(0, 0, size.x, size.y);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(1.0, -1.0, 1.0, -1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);          // Select the modelview matrix
    glLoadIdentity();                    // Reset (init) the modelview matrix
    
    shader2.bind();
    int sphereCenterLocation = glGetUniformLocation(shader2.getProgram(), "SphericalCenter");
    int thetaLocation = glGetUniformLocation(shader2.getProgram(), "theta");
    int continent_frequenzy_Location = glGetUniformLocation(shader2.getProgram(), "CONTINENT_FREQUENCY");
    int continent_lacunarity_Location = glGetUniformLocation(shader2.getProgram(), "CONTINENT_LACUNARITY");
    int mountain_lacunarity_Location = glGetUniformLocation(shader2.getProgram(), "MOUNTAIN_LACUNARITY");
    int hills_lacunarity_Location = glGetUniformLocation(shader2.getProgram(), "HILLS_LACUNARITY");
    int plains_lacunarity_Location = glGetUniformLocation(shader2.getProgram(), "PLAINS_LACUNARITY");
    int badlands_lacunarity_Location = glGetUniformLocation(shader2.getProgram(), "BADLANDS_LACUNARITY");
    int mountain_glaciation_Location = glGetUniformLocation(shader2.getProgram(), "MOUNTAIN_GLACIATION");
    int sea_level_Location = glGetUniformLocation(shader2.getProgram(), "SEA_LEVEL");
    int shelf_level_Location = glGetUniformLocation(shader2.getProgram(), "SHELF_LEVEL");
    int hills_amount_Location = glGetUniformLocation(shader2.getProgram(), "HILLS_AMOUNT");
    int mountains_amount_Location = glGetUniformLocation(shader2.getProgram(), "MOUNTAINS_AMOUNT");
    int badlands_amount_Location = glGetUniformLocation(shader2.getProgram(), "BADLANDS_AMOUNT");
    int river_depth_Location = glGetUniformLocation(shader2.getProgram(), "RIVER_DEPTH");
    int continent_height_scale_Location = glGetUniformLocation(shader2.getProgram(), "CONTINENT_HEIGHT_SCALE");
    int sea_level_in_metres_Location = glGetUniformLocation(shader2.getProgram(), "SEA_LEVEL_IN_METRES");

    glUniform3fv(sphereCenterLocation, 1, static_cast<float*>(DRVector3(0.0f, 0.0f, -1.0f*(1.0f-h))));
    glUniform1f(thetaLocation, static_cast<float>(theta));
    
    //printf("theta:%f\n", 90.0*GRADTORAD);
    
    // Frequency of the planet's continents.  Higher frequency produces smaller,
    // more numerous continents.  This value is measured in radians.
    glUniform1f(continent_frequenzy_Location, 1.0f);
    
    // Lacunarity of the planet's continents.  Changing this value produces
    // slightly different continents.  For the best results, this value should
    // be random, but close to 2.0.
    glUniform1f(continent_lacunarity_Location, 2.208984375f);
    
    // Lacunarity of the planet's mountains.  Changing this value produces
    // slightly different mountains.  For the best results, this value should
    // be random, but close to 2.0.
    glUniform1f(mountain_lacunarity_Location, 2.142578125f);
    
    // Lacunarity of the planet's hills.  Changing this value produces slightly
    // different hills.  For the best results, this value should be random, but
    // close to 2.0.
    glUniform1f(hills_lacunarity_Location, 2.162109375f);
    
    // Lacunarity of the planet's plains.  Changing this value produces slightly
    // different plains.  For the best results, this value should be random, but
    // close to 2.0.
    glUniform1f(plains_lacunarity_Location, 2.314453125f);
    
    // Lacunarity of the planet's badlands.  Changing this value produces
    // slightly different badlands.  For the best results, this value should be
    // random, but close to 2.0.
    glUniform1f(badlands_lacunarity_Location, 2.212890625f);
    
    // Specifies the amount of "glaciation" on the mountains.  This value
    // should be close to 1.0 and greater than 1.0.
    glUniform1f(mountain_glaciation_Location, 1.375);
 
    // Specifies the planet's sea level.  This value must be between -1.0
    // (minimum planet elevation) and +1.0 (maximum planet elevation.)
    const float SEA_LEVEL = 0.0f;
    glUniform1f(sea_level_Location, SEA_LEVEL);
    
    // Specifies the level on the planet in which continental shelves appear.
    // This value must be between -1.0 (minimum planet elevation) and +1.0
    // (maximum planet elevation), and must be less than SEA_LEVEL.
    glUniform1f(shelf_level_Location, -0.375f);
    
    // Determines the amount of mountainous terrain that appears on the
    // planet.  Values range from 0.0 (no mountains) to 1.0 (all terrain is
    // covered in mountains).  Mountainous terrain will overlap hilly terrain.
    // Because the badlands terrain may overlap parts of the mountainous
    // terrain, setting MOUNTAINS_AMOUNT to 1.0 may not completely cover the
    // terrain in mountains.
    const float MOUNTAINS_AMOUNT = 0.5f;
    glUniform1f(mountains_amount_Location, MOUNTAINS_AMOUNT);
  
    // Determines the amount of hilly terrain that appears on the planet.
    // Values range from 0.0 (no hills) to 1.0 (all terrain is covered in
    // hills).  This value must be less than MOUNTAINS_AMOUNT.  Because the
    // mountainous terrain will overlap parts of the hilly terrain, and
    // the badlands terrain may overlap parts of the hilly terrain, setting
    // HILLS_AMOUNT to 1.0 may not completely cover the terrain in hills.
    glUniform1f(hills_amount_Location, (1.0f + MOUNTAINS_AMOUNT) / 2.0f);
    
    // Determines the amount of badlands terrain that covers the planet.
    // Values range from 0.0 (no badlands) to 1.0 (all terrain is covered in
    // badlands.)  Badlands terrain will overlap any other type of terrain.
    glUniform1f(badlands_amount_Location, 0.03125f);
    
    // Maximum depth of the rivers, in planetary elevation units.
    glUniform1f(river_depth_Location, 0.0234375f);
    
    // Scaling to apply to the base continent elevations, in planetary elevation
    // units.
    glUniform1f(continent_height_scale_Location, (1.0f - SEA_LEVEL) / 4.0f);    
    
    // Calculate the sea level, in meters.
    float MAX_ELEV = 1.0f; float MIN_ELEV = -1.0f;
    float seaLevelInMeters = (((SEA_LEVEL + 1.0) / 2.0)
    * (MAX_ELEV - MIN_ELEV)) + MIN_ELEV;
    glUniform1f(sea_level_in_metres_Location, seaLevelInMeters);    
    //printf("seaLevelInMeters: %f\n", seaLevelInMeters);

        
    DRVector3 rotation_ZAxis = camera.getSektorPosition().getVector3().normalize();
    DRVector3 rotation_XAxis = camera.getYAxis().cross(rotation_ZAxis).normalize();
    DRVector3 rotation_YAxis = rotation_ZAxis.cross(rotation_XAxis).normalize();
    //rotationMatrix = DRMatrix::axis(rotation_XAxis, rotation_YAxis, rotation_ZAxis).invert();
   // glMultMatrixf(rotationMatrix);
    
    // Texture Matrix
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
   
    glMultMatrixf(rotationMatrix);
    
    renderSphere->render();
    shader2.unbind();
    
   
    
    //DRLog.writeToLog("zeit fuer renderToTexture: %f Sekunden", static_cast<double>(SDL_GetTicks()-start)/1000.0f);
        
    //save into texture
   // renderTarget->saveToImage("data/test.jpg");
    
    
    //clean up
    renderTarget->unbind();    
    
    DRGrafikError("Fehler bei render to texture");
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

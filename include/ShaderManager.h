#ifndef __SC_SHADER_MANAGER__
#define __SC_SHADER_MANAGER__


class ShaderManager
{
public:
    ~ShaderManager() {if(isInitialized()) exit();};

    // Singleton-Daten
    static ShaderManager&		Instance();
    inline static ShaderManager& getSingleton() {return Instance();};
    inline static ShaderManager* getSingletonPtr() {return &Instance();};
    static bool	isInitialized()	{return Instance().mInitalized;};

    DRReturn init();

    void exit();

    //! lädt oder return instance auf Shader
    ShaderProgram* getShader(const char* vertexShader, const char* fragmentShader);
    //! reduziert reference, bei null wird Shader gelöscht und OpenGL Shader in liste eingetragen
    void      releaseShader(const char* vertexShader, const char* fragmentShader);
    void      releaseShader(ShaderProgram* shader);
    void      releaseShader(DHASH id);


private:
    ShaderManager();

    struct ShaderEntry
    {
        ShaderProgram* shader;
        int referenzCounter;
    };

    DHASH makeShaderHash(const char* vertexShader, const char* fragmentShader);

    DRHashList mShaderEntrys;
    bool mInitalized;
};

#endif //__SC_SHADER_MANAGER__

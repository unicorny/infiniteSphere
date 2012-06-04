#ifndef _SC_RENDER_MIP_MAP_TO_TEXTURE
#define _SC_RENDER_MIP_MAP_TO_TEXTURE

class RenderMipMapToTexture : public RenderInStepsToTexture
{
public:
	RenderMipMapToTexture(const char* textureName);
	virtual ~RenderMipMapToTexture();

	virtual DRReturn renderStuff();

private:
	DRTexturePtr mTexture;
};

#endif // _SC_RENDER_MIP_MAP_TO_TEXTURE
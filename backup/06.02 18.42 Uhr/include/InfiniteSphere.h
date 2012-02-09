#ifndef __INFINITE_SPHERE__
#define __INFINITE_SPHERE__

class InfiniteSphere : public DRGeometrie
{
public:
    InfiniteSphere();
    ~InfiniteSphere();
    
    DRReturn init(int gridSize, DRVector3 edgePoints[4]);
};


#endif
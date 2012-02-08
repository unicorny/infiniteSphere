/* 
 * File:   DRIndexReferenzHolder.h
 * Author: Dario
 *
 * Created on 8. November 2011, 11:56
 */

#ifndef __DR_INDEX_REFERENZ_HOLDER_H
#define	__DR_INDEX_REFERENZ_HOLDER_H

class CORE2_API DRIndexReferenzHolder
{
public:
    DRIndexReferenzHolder(uint maxIndexCount);
    ~DRIndexReferenzHolder();
    
    void add(uint index);
    
    void remove(uint index);
    
    uint getFree();
    
private:
    
    uint*       mReferenzCounter;
    uint*       mFreePlaces;  
    uint        mFreePlaceCursor;
    uint        mMaxIndexCount;
    
};

#endif	/* __DR_INDEX_REFERENZ_HOLDER_H */


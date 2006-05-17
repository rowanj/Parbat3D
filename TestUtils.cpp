#include "TestUtils.h"

void attemptRasterCall(ImageFile* theFile)
{
    char* theBuffer=new char[256*256*3];
    theFile->getRasterData(256, 256, 0, 0,
    						theBuffer, 256, 256);
}


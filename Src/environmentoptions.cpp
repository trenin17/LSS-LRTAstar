#include "environmentoptions.h"

EnvironmentOptions::EnvironmentOptions()
{
    metrictype = CN_SP_MT_EUCL;
    allowsqueeze = false;
    allowdiagonal = true;
    cutcorners = false;
    algorithm = CN_SP_ST_ASTAR;
    hweight = 1;

}

EnvironmentOptions::EnvironmentOptions(bool AS, bool AD, bool CC, int MT, int AL, double HW)
{
    metrictype = MT;
    allowsqueeze = AS;
    allowdiagonal = AD;
    cutcorners = CC;
    algorithm = AL;
    hweight = HW;
}


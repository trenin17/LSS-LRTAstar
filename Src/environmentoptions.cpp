#include "environmentoptions.h"

EnvironmentOptions::EnvironmentOptions()
{
    metrictype = CN_SP_MT_EUCL;
    allowsqueeze = false;
    allowdiagonal = true;
    cutcorners = false;
    visibility = 1;
    lookahead = 1;

}

EnvironmentOptions::EnvironmentOptions(bool AS, bool AD, bool CC, int MT, int VI, int LA)
{
    metrictype = MT;
    allowsqueeze = AS;
    allowdiagonal = AD;
    cutcorners = CC;
    visibility = VI;
    lookahead = LA;
}


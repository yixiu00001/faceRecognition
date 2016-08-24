// checkfacedet.cpp:
//
// Copyright (C) 2005-2013, Stephen Milborrow

#include "stasm.h"

namespace stasm
{
#define YAW00 0
#define YAW22 1
#define YAW_22 0

static int TabPoint(    // return first used point in tab, -1 if none
    const int*   tab,   // in
    int          ntab,  // in
    const Shape& shape) // in
{
    for (int i = 0; i < ntab; i++)
        if (PointUsed(shape, tab[i]))
            return tab[i]; // note return

    return -1;
}

static void CenterOfEyeMouthTriangle(
    double&      x,                  // out
    double&      y,                  // out
    const Shape& shape)              // in
{
    static const int leyes[] =
    {
        L17_LPupil,
        L17_LEyeInner,
        L17_LEyeOuter,
        L17_LEyebrowInner,
        L17_LEyebrowOuter,
        L17_REyeInner,
        L17_RPupil,
        L17_REyeOuter,
        L17_REyebrowInner,
        L17_REyebrowOuter,
        L17_CNoseTip
    };
    static const int reyes[] =
    {
        L17_RPupil,
        L17_REyeInner,
        L17_REyeOuter,
        L17_REyebrowInner,
        L17_REyebrowOuter,
        L17_LEyeInner,
        L17_LPupil,
        L17_LEyeOuter,
        L17_LEyebrowInner,
        L17_LEyebrowOuter,
        L17_CNoseTip
    };
    static const int mouths[] =
    {
        L17_CBotOfBotLip,
        L17_CTopOfTopLip,
        L17_LMouthCorner,
        L17_RMouthCorner,
        L17_CNoseTip,
        L17_LNostril,
        L17_RNostril
    };
    const Shape shape17(Shape17(shape));

    const int leye  = TabPoint(leyes,  NELEMS(leyes),  shape17);
    const int reye  = TabPoint(reyes,  NELEMS(reyes),  shape17);
    const int mouth = TabPoint(mouths, NELEMS(mouths), shape17);

    CV_Assert(PointUsed(shape17, leye));
    CV_Assert(PointUsed(shape17, reye));
    CV_Assert(PointUsed(shape17, mouth));

    x = (shape17(leye, IX) + shape17(reye, IX) + shape17(mouth, IX)) / 3;
    y = (shape17(leye, IY) + shape17(reye, IY) + shape17(mouth, IY)) / 3;
}

// The face detector is considered to be on the face if (i) the center of
// the face det rectangle must be near center of the eye-mouth triangle
// (ii) the face det rectangle must be neither too big nor too small
// relative to the size of the face.

bool DetOnFace(              // true if the face det rect is on the face
    const DetPar& detpar,    // in
    const Shape&  refshape)  // in
{
#if YAW00
    const Shape shape17(Shape17(refshape));
    double x, y; CenterOfEyeMouthTriangle(x, y, refshape);
    const double eyemouth = EyeMouthDist(refshape);
    // TODO reduce these constants to say 2.4 or 2.5 gen false pos for faces that are too big
    const bool ok1 = PointDist(detpar.x, detpar.y, x, y) < .36 * eyemouth;
    const bool ok2 = detpar.width  < 3.0 * eyemouth; // TODO lax, allows big face rects, needed for haarcascade_frontalface_alt2.xml
    const bool ok3 = detpar.height < 3.0 * eyemouth;
    // lprintf("[%d%d%d %.2f %.2f %.2f] ", ok1, ok2, ok3,
    //         PointDist(detpar.x, detpar.y, x, y) / eyemouth,
    //         detpar.width / eyemouth,
    //         detpar.height / eyemouth);
    return ok1 && ok2 && ok3;

#elif YAW22
    const Shape shape17(Shape17(refshape, true));
    double x, y; CenterOfEyeMouthTriangle(x, y, refshape);
    const double eyemouth = EyeMouthDist(refshape);
    // first check is that right boundary of face rect is to right of right eye (detect a common yaw22 false pos)
    const bool ok1 = !PointUsed(shape17, L17_REyeOuter) ||
                       detpar.x + detpar.width/2 > shape17(L17_REyeOuter, IX);
    const bool ok2 = PointDist(detpar.x, detpar.y, x, y) < .7 * eyemouth;
    // magic numbers from eyeballing def_tr_Y-35-15P-1515R-1515.shape results
    const bool ok3 = detpar.width / eyemouth > 1.15 ;
    const bool ok4 = detpar.width / eyemouth < 2.35;
    // lprintf("\n[%d%d%d%d]\n", ok1, ok2, ok3, ok4);
    return ok1 && ok2 && ok3 && ok4;

#elif YAW_22
    const Shape shape17(Shape17(refshape));

    double x, y; CenterOfEyeMouthTriangle(x, y, refshape);

    const double eyemouth = EyeMouthDist(refshape);

    // first check is that left boundary of face rect is to left of left eye
    const bool ok1 = !PointUsed(shape17, L17_LEyeOuter) ||
                       detpar.x - detpar.width/2 < shape17(L17_LEyeOuter, IX);
    const bool ok2 = PointDist(detpar.x, detpar.y, x, y) < .7 * eyemouth;
    const bool ok3 = detpar.width + detpar.width/2 / eyemouth > 1.15;
    const bool ok4 = detpar.width / eyemouth < 2.35;
    // lprintf("\n[%d%d%d%d]\n", ok1, ok2, ok3, ok4);
    return ok1 && ok2 && ok3 && ok4;

#else
    Err("Must #define YAW00 or YAW22 or YAW_22");
#endif
}

} // namespace stasm

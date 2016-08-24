// fitmeas.cpp: calculate fitness measures
//
// Copyright (C) 2005-2013, Stephen Milborrow

#include "stasm.h"
#include "fitmeas.h"

namespace stasm
{
// Like MeanFitOverInterEye but assumes shape and refshape have same nbr or points

static double MeanFitOverInterEye1( // mean landmark distance divided by intereye
    int&         iworst,            // out: index of point with worse fit
    const Shape& shape,             // in
    const Shape& refshape)          // in
{
    CV_Assert(shape.rows == refshape.rows);
    iworst = -1;
    double fit = 0, worst = -1;
    int npoints = 0;
    for (int i = 0; i < shape.rows; i++)
    {
        if (PointUsed(refshape, i) && PointUsed(shape, i))
        {
            const double pointfit = PointDist(shape, refshape, i);
            fit += pointfit;
            if (pointfit > worst)
            {
                worst = pointfit;
                iworst = i;
            }
            npoints++;
        }
    }
    // 3 is arb, but need a decent number of points
    CV_Assert(npoints > MIN(1, shape.rows / 3));
    return fit /= (npoints * InterEyeDist(refshape));
}

// Convert shape to same number of points as refshape and return meanfit
// over the inter eye distance.
// Return NOFIT (currently .99999) if cannot convert shape.

double MeanFitOverInterEye( // mean landmark distance divided by intereye
    int&         iworst,    // out: index of point with worse fit
    const Shape& shape,     // in
    const Shape& refshape)  // in: shape will be converted to same nbr of points as refshape
{
    Shape newshape = ConvertShape(shape, refshape.rows);
    if (newshape.rows != refshape.rows) // cannot convert number of points?
    {
        iworst = -1;
        return NOFIT;
    }
    return MeanFitOverInterEye1(iworst, newshape, refshape);
}

double Me17(               // me17 fitness measure, return NOFIT if can't get it
    const Shape& shape,    // in
    const Shape& refshape) // in
{
    const Shape shape17(Shape17OrEmpty(shape));
    const Shape refshape17(Shape17OrEmpty(refshape));
    if (shape17.rows == 0 || refshape17.rows == 0)
        return NOFIT;
    int idummy;
    return MeanFitOverInterEye1(idummy, shape17, refshape17);
}

} // namespace stasm

// linsolve.cpp: routines to solve linear systems

#include "stasm.h"

namespace stasm
{
// Solve for x in A x = b using the SVD decompostion.
// This is much slower than using the LU decomposition but can handle
// underdetermined systems.
//
// In the Tasm context, the system will be undetermined when there are not
// enough shapes.  More precisely, when the number of positive and negative
// cases is less than the descriptor length, or there is collinearity in
// the descriptors.

static const VEC LinSolveSvd(
    MAT&        A,   // in, but gets destroyed: nbr of cases x intercept plus nbr of vars
    const VEC&  b,   // in: nbr of cases (row or column vector)
    const char* msg) // in: appended to err msg if any e.g. "(level 1 point 2)"
{
    MAT U, D, Vt;
    cv::SVD::compute(A, D, U, Vt, cv::SVD::MODIFY_A); // MODIFY_A for speed
    const double max = D(0) / 1e8;  // condition number 1e8 is arb
    MAT Di(NSIZE(D), NSIZE(D), 0.); // inverted vector D as a diagonal matrix
    for (int i = 0; i < NSIZE(D); i++)
    {
        if (IsZero(D(i), max))
        {
            // system is undetermined, not a show-stopper, but let the user know
            static int printed;
            PrintOnce(printed,
                      "\n    number of columns is %d but rank is %d%s%s...\n",
                      A.cols, i, msg && msg[0]? " ": "", msg);
            break; // note break
        }
        Di(i, i) = 1 / D(i);
    }
    const MAT Ai(U * Di * Vt);                 // pseudo inverse
    return ((b.cols == 1? b.t(): b) * Ai).t(); // return a column vector
}

// Solves for x in A x = b.  Returns x as a column vector.
// This can deal with determined, over- or under-determined systems.
// For speed it uses the LU decomp where possible.  Else it uses the SVD.
// The b argument can be a row or column vector

const VEC LinSolve(  // solve for x in A x = b, returns x as a column vector
    MAT&        A,   // in, but gets destroyed: nbr of cases x intercept plus nbr of vars
    VEC&        b,   // in, but gets destroyed: 1 x nbr of cases
    const char* msg) // in: appended to err msg if any e.g. "(level 1 point 2)"
{
    CV_Assert(A.isContinuous() && b.isContinuous());
    if (b.rows == 1)
        b = b.t();
    CV_Assert(b.cols == 1);
    CV_Assert(A.rows == NSIZE(b));
    if (A.rows < A.cols) // cv::solve won't accept rows < cols
        return LinSolveSvd(A, b, msg);  // note return
    VEC coeffs(1, A.rows);
    if (solve(A, b, coeffs, cv::DECOMP_LU|cv::DECOMP_NORMAL))
        return coeffs;                  // note return
    static int printed;
    PrintOnce(printed,
              "\nSolve with LU failed, solving with SVD instead%s%s...\n",
              msg && msg[0]? " ": "", msg);
    return LinSolveSvd(A, b, msg);
}

} // namespace stasm

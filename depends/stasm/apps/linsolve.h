// linsolve.h: routines to solve linear systems

#ifndef stasm_linsolve_hpp
#define stasm_linsolve_hpp

namespace stasm
{
const VEC LinSolve(   // solve for x in A x = b, returns x as a column vector
    MAT&        A,    // in, but gets destroyed: nbr of cases x intercept plus nbr of vars
    VEC&        b,    // in, but gets destroyed: 1 x nbr of cases
    const char* msg); // in: appended to err msg if any e.g. "(level 1 point 2)"

} // namespace stasm
#endif // stasm_linsolve_hpp

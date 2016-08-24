// appmisc.h: miscellaneous defs for apps but not needed by the Stasm library itself
//
// Copyright (C) 2005-2013, Stephen Milborrow

#ifndef STASM_APPMISC_H
#define STASM_APPMISC_H

#if _MSC_VER // microsoft compiler
#pragma warning(disable:4996) // sprintf: This function may be unsafe
#endif

namespace stasm
{
typedef vector<Shape>    vec_Shape;
typedef vector<string>   vec_string;
typedef vector<VEC>      vec_VEC;

inline const char* plural(int n)
{
    return n == 1? "": "s";
}

class Pacifier // print pacifier when i is at every 10% of n
{
public:
    Pacifier(int n, int nmin=50) // constructor
    {
        n_ = n;
        nmin_ = nmin; // n must be at least nmin to print anything
        boundary_ = 0;
    }
    void Print_(int i) // print pacifier if i has reached next 10% boundary
    {
        CV_Assert(i >= 0 && i <= n_);
        if (n_ > nmin_ && i >= boundary_)
        {
            const int n10 = ((n_ + 10) / 10) * 10;
            printf("%d_", (boundary_ * 10) / n10);
            fflush(stdout);
            boundary_ += n10 / 10;
        }
    }
    void End_(void) // print final 0
    {
        if (n_ > nmin_)
        {
            printf("0");
            fflush(stdout);
        }
    }
private:
    int n_, nmin_, boundary_;
    DISALLOW_COPY_AND_ASSIGN(Pacifier);
};

const Shape LandmarksAsShape(   // convert array of landmarks to a Shape
    const float* landmarks);    // in

void Fprintf(                   // like fprintf but issue err if can't write
    FILE*       file,           // in
    const char* format, ...);   // in

void LogShapeAsCsv(             // print shape in CSV format to log file
    const MAT&  mat,            // in
    const char* path);          // in

void AdjustCropSoDivBy4(
    int& leftcrop,              // io
    int& rightcrop,             // io
    int  width,                 // in
    bool allow_overcrop=true);  // in

void CropCimgToShapeWithMargin( // crop the image so the shape fills the image
    CImage& img,                // io
    const  Shape& Shape,        // in
    double xmargin=-1,          // in: -1 (default) means auto choose margin
    double ymargin=-1);         // in: -1 (default) means auto choose margin

char* PathGivenDirs(            // return the full path of the image with basename base
    const char* base,           // in
    const char* dirs,           // in: dir names separated by semicolons
    const char* shapepath);     // in: path of shape file holding dirs, for err msgs

char* PathGivenDirs(            // like above but base is given as a string
    const string& base,         // in
    const char*   dirs,         // in: dir names separated by semicolons
    const char*   shapepath);   // in: path of shape file holding dirs, for err msgs

void ToUpper(                   // convert string to upper case
    char s[]);                  // io

Image AsImage(const CImage& cimg); // convert a CImage to an Image

bool DirWriteable(              // true if directory is writeable
    const char* path,           // in
    bool        verbose);       // in: true to print messages

void MkDir(                     // create the given directory if it doesn't exist
    const char* path);          // in: the directory

void System(
    const char* cmd);           // in

void CmdLineAsSingleString(
    char*       cmdline,        // out
    int         argc,           // in
    const char* argv[]);        // in

void WriteMatAsArray(
    const MAT&  mat,            // in
    const char* name,           // in
    const char* comment,        // in
    FILE*       file);          // in

void AttrBitsToString(
    char*       s,              // io: bits will be appended to this string
    unsigned    bits,           // in: info bits e.g. AT_Glasses
    unsigned    filter=-1,      // in: only bits in filter will be included
    const char* space=" ");     // in

double MaxAbsElem(              // maximum absolute value of matrix elements
    const MAT& mat);

double MaxElem(                 // maximum matrix element
    const MAT& mat);

} // namespace stasm
#endif // STASM_APPMISC_H

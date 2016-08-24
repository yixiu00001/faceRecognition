// appmisc.h: miscellaneous defs for apps but not needed by the Stasm library itself
//
// Copyright (C) 2005-2013, Stephen Milborrow

#include <sys/types.h>
#include <sys/stat.h>
#if _MSC_VER // microsoft
#include <direct.h>
#endif
#include "stasm.h"
#include "opencv/highgui.h"
#include "appmisc.h"

namespace stasm
{
const Shape LandmarksAsShape( // convert an array of landmarks to a shape
    const float* landmarks)   // in: will be converted to a shape
{
    Shape shape(stasm_NLANDMARKS, 2);
    for (int i = 0; i < stasm_NLANDMARKS; i++)
    {
        shape(i, IX) = landmarks[i*2];
        shape(i, IY) = landmarks[i*2+1];
    }
    return shape;
}

void Fprintf(                 // like fprintf but issue err if can't write
    FILE*               file, // in
    const char* format, ...)  // in
{
    char s[SBIG];
    va_list args;
    va_start(args, format);
    VSPRINTF(s, format, args);
    va_end(args);
    for (int i = 0; s[i]; i++)
    {
        if (EOF == fputc(s[i], file))
            Err("Cannot write to file (%-.100s)", format);
        if (i > SBIG-1)
            Err("Fprintf: string is too long (%-.100s)", format);
    }
    fflush(file);
}

void LogShapeAsCsv(   // print shape in CSV format to log file
    const MAT&  mat,  // in
    const char* path) // in
{
    CV_Assert(mat.cols == 2);
    static bool firsttime = true;
    if (firsttime)
    {
        // print heading
        firsttime = false;
        logprintf("\n# name,nrows,ncols,");
        for (int row = 0; row < mat.rows; row++)
            logprintf("x%d,y%d%s", row, row, row < mat.rows-1? ",":"");
    }
    logprintf("\n# %s,%d,%d,", Base(path), mat.rows, mat.cols);
    for (int row = 0; row < mat.rows; row++)
    {
        for (int col = 0; col < mat.cols; col++)
        {
            if (int(mat(row, col)) == mat(row, col))
                logprintf("%.0f", mat(row, col));
            else
                logprintf("%.1f", mat(row, col));
            if (row != mat.rows-1 || col != mat.cols-1)
                logprintf(",");
        }
    }
    logprintf("\n");
}

// Adjust x crop so image width after cropping is divisible by 4.
//
// Images with width divisible by 4 can be handled much more efficiently
// under certain circumstances in Windows (e.g. StretchDIBits requires that
// the image width is divisible by 4).
//
// We keep cropping symmetrical, as much as is possible.
// Also if crops are big enough we reduce rather than
// increase them so we undercrop rather than overcrop.
//
// If allow_overcrop is false the final crop may not be div-by-4
// if the passed in crop is too small to allow us to decrease them.
// If true, allow increase in left or right crop to enable div-by-4.

void AdjustCropSoDivBy4(
    int& leftcrop,              // io
    int& rightcrop,             // io
    int  width,                 // in
    bool allow_overcrop)        // in
{
    int rem = (width - leftcrop - rightcrop) % 4;
    switch (rem)
    {
    case 0:
        break; // no cropping needed

    case 1:
        if (leftcrop >= 1 && rightcrop >= 2)
        {
            leftcrop -= 1;
            rightcrop -= 2;
        }
        else if (leftcrop >= 2 && rightcrop >= 1)
        {
            leftcrop -= 2;
            rightcrop -= 1;
        }
        else if (leftcrop >= 3)
            leftcrop -= 3;
        else if (rightcrop >= 3)
            rightcrop -= 3;
        else if (allow_overcrop)
            leftcrop += 1;
        break;

    case 2:
        if (leftcrop >= 1 && rightcrop >= 1)
        {
            leftcrop -= 1;
            rightcrop -= 1;
        }
        else if (rightcrop >= 2)
            rightcrop -= 2;
        else if (leftcrop >= 2)
            leftcrop -= 2;
        else if (allow_overcrop)
        {
            leftcrop += 1;
            rightcrop += 1;
        }
        break;

    case 3:
        if (leftcrop >= 1)
            leftcrop -= 1;
        else if (rightcrop >= 1)
            rightcrop -= 1;
        else if (allow_overcrop)
        {
            leftcrop += 1;
            rightcrop += 2;
        }
        break;

    default:
        Err("Invalid rem %d", rem);
        break;
    }
    CV_Assert(!allow_overcrop || (width - leftcrop - rightcrop) % 4 == 0);
}

static void CropCimg(  // crop the given amounts off the edges of a color image
    CImage& img,       // io
    int     topcrop,   // in
    int     botcrop,   // in
    int     leftcrop,  // in
    int     rightcrop, // in
    bool    div4=true) // in: want width and height div by 4? default true
{
    if (div4)
    {
        AdjustCropSoDivBy4(leftcrop, rightcrop, img.cols);
        AdjustCropSoDivBy4(topcrop,  botcrop,   img.rows);
    }
    int new_width  = img.cols - leftcrop - rightcrop;
    int new_height = img.rows - topcrop  - botcrop;
    CV_Assert(topcrop >= 0   && botcrop >= 0);
    CV_Assert(leftcrop >= 0  && rightcrop >= 0);
    CV_Assert(new_width >= 0  && new_width <= img.cols);
    CV_Assert(new_height >= 0 && new_height <= img.rows);
    CImage outimg(new_height, new_width);
    for (int iy = 0; iy < new_height; iy++)
        for (int ix = 0; ix < new_width; ix++)
            outimg(iy, ix) = img(iy + topcrop, ix + leftcrop);
    img = outimg;
}

void CropCimgToShapeWithMargin( // crop the image so the shape fills the image
    CImage&       img,          // io
    const  Shape& shape,        // in
    double        xmargin,      // in: -1 (default) means auto choose margin
    double        ymargin)      // in: -1 (default) means auto choose margin
{
    double xmin, xmax, ymin, ymax; ShapeMinMax(xmin, xmax, ymin, ymax, shape);

    // Note: Increase the default margins below if you want more room around the
    // shape.  These margins work well with faces where we generally need
    // more space around the top (for the hair) than around the other sides.
    //
    // The MIN MAX stuff below is to handle one-point shapes and similar.

    const double height  = MIN(MAX(100, ymax - ymin), .7 * img.rows);
    xmargin = (xmargin < 0)? height / 6: xmargin;
    double ymargin_top = (ymargin < 0)? height / 3:  ymargin;
    double ymargin_bot = (ymargin < 0)? height / 5:  ymargin;

    CropCimg(img,
             cvRound(MAX(0, ymin - ymargin_top)),             // top
             cvRound(MAX(0, img.rows - ymax - ymargin_bot)),  // bottom
             cvRound(MAX(0, xmin - xmargin)),                 // left
             cvRound(MAX(0, img.cols - xmax - xmargin)));     // right
}

// Return the string at index n in strings,
// where strings has the form dir1;dir2;dir3

static const char* NthString(
    const char* strings,      // in
    int         n)            // in
{
    // following def needed because strtok replaces ; with a 0
    char strings1[SBIG]; STRCPY(strings1, strings);
    int i = 0;
    char* dir = strtok(strings1, ";");
    while (i++ < n && dir != NULL)
        dir = strtok(NULL, ";");
    return dir;
}

// Look for a readable image file with base name base and with an
// image extension (like .jpg) and in a directory listed in dirs.
// For efficiency we first try the directory and extension used in the
// previous call to this function because that is most likely to be correct.

char* PathGivenDirs(         // return the full path of the image with basename base
    const char* base,        // in
    const char* dirs,        // in: dir names separated by semicolons
    const char* shapepath)   // in: path of shape file holding dirs, for err msgs
{
    static const char* const exts = "jpg;bmp;pgm;png"; // extensions we search for

    static char path[SLEN];
    static char lastdir[SLEN];
    static char lastext[SLEN];

    char drive[_MAX_DRIVE], dir[_MAX_DIR], base1[_MAX_FNAME], ext[_MAX_EXT];
    splitpath(base, drive, dir, base1, ext);
    // sanity checks, make sure base is indeed a basename, no dir or extension)
    if (drive[0] || dir[0])
        Err("%s in %s includes a directory name\n"
            "(filenames in shape files should not contain directories)",
             base, shapepath);
    else if (STRNLEN(ext, _MAX_EXT) > 0)
        Err("File name %s in %s has an extension?\n"
            "(file names in shape files should not have extensions)",
            base, shapepath);

    // first try the same dir and ext we used last time (for efficiency)
    bool tried_open = false;
    FILE* file = NULL;
    if (lastdir[0] && lastext[0])
    {
        sprintf(path, "%s/%s.%s", lastdir, base1, lastext);
        logprintf("Open %s\n", path);
        file = fopen(path, "rb");
        tried_open = true;
    }
    if (!file) // no success?
    {
        // try each ext and dir in turn
        int iext = 0; const char* ext1;
        while (!file && NULL != (ext1 = NthString(exts, iext++)))
        {
            STRCPY(ext, ext1);
            int idir = 0; const char* dir1 = "";
            while (!file && NULL != (dir1 = NthString(dirs, idir++)))
            {
                STRCPY(dir, dir1);
                sprintf(path, "%s/%s.%s", dir, base1, ext);
                logprintf("%sOpen %s ",
                          (tried_open? "unsuccessful, trying next\n": ""), path);
                file = fopen(path, "rb");
                tried_open = true;
            }
            if (file)           // success?
            {
                STRCPY(lastdir, dir);
                STRCPY(lastext, ext);
            }
        }
    }
    if (!file)
    {
        logprintf("unsuccessful\n");
        Err("Cannot open %s.*\n"
            "       Searched in directories %s\n"
            "       With extensions %s\n"
            "       Check definition of \"Directories\" in %s?",
            base1, dirs, exts, shapepath);
    }
    fclose(file);
    return path;
}

char* PathGivenDirs(         // like above but base is given as a string
    const string& base,      // in
    const char*   dirs,      // in: dir names separated by semicolons
    const char*   shapepath) // in: path of shape file holding dirs, for err msgs
{
    return PathGivenDirs(base.c_str(), dirs, shapepath);
}

void ToUpper(char s[])
{
    int n = STRNLEN(s, SLEN);
    for (int i = 0; i < n; i++)
        s[i] = toupper(s[i]);
}

// TODO: If you pass an Image (not a CImage) to this function you will get:
// OpenCV Error: The function/feature is not implemented in matrix.cpp.
// So in that case we never even get to the CV_Assert below.

Image AsImage(const CImage& cimg) // convert a CImage to an Image
{
    CV_Assert(cimg.depth() == CV_8U && cimg.channels() == 3);
    Image img; cvtColor(cimg, img, CV_BGR2GRAY);
    return img;
}

bool DirWriteable(       // true if directory is writeable
    const char* path,    // in
    bool        verbose) // in: true to print messages
{
    CV_Assert(path != NULL && STRNLEN(path, SLEN) > 0);
#if _MSC_VER // microsoft
    struct _stat st;
    if (_stat(path, &st))
#else
    struct stat st;
    if (stat(path, &st))
#endif
    {
        if (verbose)
            lprintf("Directory %s does not exist\n", path);
        return false;
    }
    else if ((st.st_mode & S_IFDIR) == 0)
    {
        if (verbose)
            lprintf("%s is not a directory\n", path);
        return false;
    }
    else if ((st.st_mode & S_IWRITE) == 0)
    {
        if (verbose)
            lprintf("Directory %s is not writeable\n", path);
        return false;
    }
    return true;
}

void MkDir(           // create the given directory if it doesn't exist
    const char* path) // in: the directory
{
    if (!DirWriteable(path, false))
    {
        lprintf("mkdir %s\n", path);
        const int err =
#if _MSC_VER // microsoft
            _mkdir(path);
// Added by Yuji Oyamada, 2013.02.01
// from here
#elif __GNUC__
            mkdir(path, 0777);
// to here
#else
            mkdir(path); // TODO some systems want: mkdir(path, 0777);
#endif
        if (err)
            Err("Cannot create directory %s (error code %d)", path, err);
    }
}

void System(
    const char* cmd) // in
{
    lprintf("%s\n", cmd); // as a side effect also flushes stdout and logfile
    const int err = system(cmd);
    if (err)
        Err("%s failed (errno %d)", cmd, err);
}

void CmdLineAsSingleString(
    char*       cmdline, // out
    int         argc,    // in
    const char* argv[])  // in
{
    CV_Assert(argc >= 1);
    sprintf(cmdline, "%s", Base(argv[0]));
    const int n = argc < 32? argc: 32;
    for (int i = 1 ; i < n; i++)
        strcat(cmdline, ssprintf(" %s", argv[i]));
    if (n != argc)
        strcat(cmdline, " ...");
}

void WriteMatAsArray(
    const MAT&  mat,     // in
    const char* name,    // in
    const char* comment, // in
    FILE*       file)    // in
{
    char comment1[SLEN]; comment1[0] = 0;
    if (comment && comment[0])
        sprintf(comment1, " // %s", comment);
    if (mat.rows == 1)
        Fprintf(file, "static const double %s[%d] =%s\n{\n",
                name, mat.cols, comment1);
    else
        Fprintf(file, "static const double %s[%d*%d] =%s\n{\n",
                name, mat.rows, mat.cols, comment1);
    const char* spacer =
        mat.rows == 1 || mat.cols == 1? "\n    ": "\n        ";
    for (int row = 0; row < mat.rows; row++)
    {
        Fprintf(file, "    ");
        int i = 0; // number of elements printed in curren line
        for (int col = 0; col < mat.cols; col++)
        {
            i++;
            Fprintf(file, "%g%s%s",
                mat(row, col),
                col < mat.cols-1 || row < mat.rows-1? ",": "",
                col >= mat.cols-1? "": (i > 10? spacer: " "));
            if (i > 10)
                i = 0;
        }
        Fprintf(file, "\n");
    }
    Fprintf(file, "};\n");
}

void AttrBitsToString(
    char*       s,      // io: bits will be appended to this string
    unsigned    bits,   // in: info bits e.g. AT_Glasses
    unsigned    filter, // in: only bits in filter will be included
    const char* space)  // in
{
    unsigned bit = 1;
    for (int i = 0; i < 16; i++)
    {
        if ((bit & filter) && (bits & bit))
            strcat(s, ssprintf("%s%s", space, AT_NAMES[i]));
        bit <<= 1;
    }
}

double MaxAbsElem( // maximum absolute value of matrix elements
    const MAT& mat)
{
    CV_Assert(mat.isContinuous());
    double max = 0;
    double* data = Buf(mat);
    for (int i = 0; i < NSIZE(mat); i++)
        if (ABS(data[i]) > max)
            max = ABS(data[i]);
    return max;
}

double MaxElem(      // maximum matrix element
    const MAT& mat)
{
    CV_Assert(mat.isContinuous());
    double max = -FLT_MAX;
    double* data = Buf(mat);
    for (int i = 0; i < NSIZE(mat); i++)
        if (data[i] > max)
            max = data[i];
    return max;
}

} // namespace stasm

// shapefile.cpp: read and use shape files
//
// Copyright (C) 2005-2013, Stephen Milborrow

#include "stasm.h"
#include "opencv/highgui.h"
#include "../appmisc.h"
#include "shapefile.h"
#include "stasm_regex.h"

namespace stasm
{
// Return the current line number in the given file.
// This function is very slow but is used only for reporting errors.

static int LineNbr(FILE* file)
{
    CV_Assert(file);
    int n = 0;                        // line number
    const long filepos = ftell(file); // original position in file
    fseek(file, 0, SEEK_SET);         // rewind to start of file
    while (ftell(file) < filepos)     // read lines until back at original posn
    {
        char line[SBIG];
        if (!fgets(line, SBIG-1, file))
            break;                    // should never get here
        n++;
    }
    fseek(file, filepos, SEEK_SET);   // restore original position
    return n;
}

// Like fgets but
//    o no final \n or \r
//    o skips comments (# and anything after it)
//    o skips white space lines or all comment lines
//    o skips space at start of line
//
// TODO Fgets, ReadMatData, ReadMat could be simplified

static const char* Fgets(char* s, int n, FILE* file)
{
    if (s == NULL || n <= 1 || file == NULL) // compat with fgets
        return NULL;
    bool incomment = false, inleadingspace = true;
    int i = 0, c = 0;
    while (1)
    {
        c = fgetc(file);
        if (c == EOF)
        {
            if (i > 0)
                i--;
            break;
        }
        if (c == '\r')
            ;               // ignore CR
        else if (c == '\n')
        {
            if (inleadingspace)
            {
                // start from scratch on the next line in file
                incomment = false;
                i = c = 0;
            }
            else
                break;
        }
        else if (c == '#')
        {
            incomment = true;
            while (i > 0 && (s[i-1] == ' ' || s[i-1] == '\t'))
                i--;        // backspace over preceding blanks
        }
        else if (!incomment)
        {
            if (c != ' ' && c != '\t')
                inleadingspace = false;
            if (!inleadingspace)
            {
                s[i++] = char(c);
                if (i >= n - 1)
                    break;
            }
        }
    }
    s[i] = 0;
    return i == 0? NULL: s;
}

static void Header(         // read and check first line of shape file
    bool&       oldformat,  // out: true if old format shape file
    const char* shapepath,  // in: shape file path
    FILE*       file)       // in
{
    oldformat = false;

    char s[SLEN];
    if (!fgets(s, SLEN-1, file))
        Err("Cannot read %s", shapepath);

    static const char* const whitespace = " \t\n\r";
    char* token = strtok(s, whitespace);
    if (strcmp("ss", token) == 0)
    {
        oldformat = true;
        lprintf("old format shapefile ");
    }
    else if (strcmp("shape", token) != 0)
        Err("%s: bad header (expected \"shape\" or \"ss\")", shapepath);
}

// On return dirs will be a string holding the semi-colon separated list
// of image directories in the given shape file, like "/dir1;/dir2".
//
// On entry, we assume file points to the string, or to comments
// preceding it, which will be ignored.

static void ImgDirs(
    char*       dirs,       // out
    const char* shapepath,  // in: shape file path
    FILE*       file)       // in
{
    char s[SLEN];
    Fgets(s, SLEN-1, file); // will skip blank lines and comments, if any

    static const char* const whitespace = " \t\n\r";
    char* token = strtok(s, whitespace);
    if (!token || 0 != strcmp(token, "Directories"))
        Err("Expected \"Directories\" in line %d of %s",
            LineNbr(file), shapepath);

    token = strtok(NULL, whitespace);
    if (!token)
        Err("Cannot read image directories in line %d of %s",
            LineNbr(file), shapepath);

    strncpy_(dirs, token, SLEN);
    ConvertBackslashesToForwardAndStripFinalSlash(dirs);
}

static void PrematureEndOfFile(FILE* file, const char* path)
{
    long n = LineNbr(file);
    if (n > 0)
        Err("%s(%d): premature end of file", path, n);
    else
        Err("Cannot read from %s", path);
}

static void SkipToEndOfLine(FILE* file, const char* path)
{
    int c = ' ';
    while (c != '\n' && c != EOF)
        c = fgetc(file);
    if (c == EOF)
        PrematureEndOfFile(file, path);
}

// Read the data fields of a matrix
// This assumes we have already read the header "{ nrows ncols".
// It reads up to but not including the final "}".
//
// Known issue: comments not at the start of a line must be preceded by a space.

static void ReadMatData(
    Shape&      mat,    // out
    int         nrows,   // in
    int         ncols,   // in
    FILE*       file,    // in
    const char* path)    // in: for error reporting
{
    double* data = Buf(mat);
    for (int i = 0; i < ncols * nrows; i++)
    {
        // skip comments and white space
        int c = ' ';
        while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            c = fgetc(file);
            if (c == '#') // comment
            {
                SkipToEndOfLine(file, path);
                c = fgetc(file);
            }
        }
        if (c == EOF)
            PrematureEndOfFile(file, path);
        else
        {
            ungetc(c, file);
            float temp; // microsoft compiler can't sscanf doubles so use float
            if (!fscanf(file, "%g", &temp))
                Err("%s(%d): Cannot read %dx%d matrix",
                    path, LineNbr(file), nrows, ncols);
            data[i] = temp;
        }
    }
}

static bool ReadMat(  // true if read the mat, false if no (more) mats in file
    char*       base, // out: basename in tag
    unsigned&   bits, // out: hex bits in tag
    Shape&      mat,  // out: the matrix
    FILE*       file, // in:  pointer to the shape file
    const char* path) // in:  for error messages
{
    char s[SLEN];     // the string tag before the matrix
    while (1)
    {
        int c = fgetc(file);
        if (c == EOF)
            return false;   // note return
        if (c == '{')
            break;          // note break
        else if (c == '#')
            SkipToEndOfLine(file, path);
        else if (c == '\n' || c == '\r' || c == '\t' || c == ' ') // white space
            ;
        else if (c == '"') // old format tag (enclosed in quotes)
            ;
        else    // any other char, assume it is the start of the tag
        {
            s[0] = char(c);
            if (!Fgets(s+1, SLEN-1, file))
                Err("%s(%d): Read failed (premature EOF)",
                    path, LineNbr(file));
            // remove trailing white space and final quote if any
            int i = STRNLEN(s, SLEN) - 1;
            CV_Assert(i >= 4);
            while (s[i] == ' ' || s[i] == '\t' || s[i] == '"')
                i--;
            s[i+1] = 0;
        }
    }
    if (!s[0])
        Err("%s(%d): Empty tag", path, LineNbr(file));
    if (s[4] != ' ' && s[8] != ' ') // hex string must be 4 or 8 chars
        Err("%s(%d): Malformed tag", path, LineNbr(file));
    if (2 != sscanf(s, "%x %s", &bits, base))
        Err("%s(%d): Malformed tag", path, LineNbr(file));

    int nrows, ncols; int c;
    if (2 != fscanf(file, "%d %d", &nrows, &ncols))
        Err("%s(%d): Cannot read matrix size", path, LineNbr(file));
    if (ncols < 1 || nrows > MAX_MAT_DIM)
        Err("%s(%d): Invalid number of rows %d", path, LineNbr(file), nrows);
    if (ncols < 1 || ncols > MAX_MAT_DIM)
        Err("%s(%d): Invalid number of columns %d", path, LineNbr(file), ncols);

    mat.create(nrows, ncols);
    ReadMatData(mat, nrows, ncols, file, path);

    // make sure that next non-white char is matrix terminator '}'

    c = ' ';
    while (c == ' ' || c == '\t' || c == '\n' || c == '\r') // skip white space
        if (EOF == (c = fgetc(file))) // assignment is intentional
            Err("%s(%d): Cannot read matrix\n"
                "       Reached EOF before finding \"}\"",
                path, LineNbr(file));
    if (c == '#')
        Err("%s(%d): Comment not allowed here", path, LineNbr(file));
    if (c != '}')
        Err("%s(%d): Footer is not \"}\" "
            "(too many or two few entries in matrix?)", path, LineNbr(file));

    return true; // success
}

void ShapeFile::Open_(     // read shape file from disk
    const char* shapepath) // in
{
    lprintf("Reading %s: ", shapepath);
    STRCPY(shapepath_, shapepath);
    FILE* file = fopen(shapepath, "rb");
    if (!file)
        Err("Cannot open %s", shapepath);

    bool oldformat = false;
    Header(oldformat, shapepath, file);
    ImgDirs(dirs_, shapepath, file);
    shapes_.clear();
    bases_.clear();
    bits_.clear();
    poses_.clear();
    yaw00s_.clear();
    char base[SLEN]; unsigned bits; Shape shape;
    nchar_ = 0;
    int nrows = -1;
    while (ReadMat(base, bits, shape, file, shapepath))
    {
        bool normalshape = true;
        if (oldformat)
        {
            if (bits & FA_Meta)  // metashape?
                normalshape = false;
        }
        else if (bits & AT_Meta) // metashape?
        {
            normalshape = false;
            if (bits == AT_Pose)
            {
                if (shape.rows == 1 && shape.cols == 4) // yaw pitch roll err
                    poses_[base] = shape.clone();
                else
                    Err("%x %s bad format", bits, base);
            }
            else if (bits == AT_EYAW00)
            {
                if (shape.rows == 1 &&
                        (shape.cols == 4 || shape.cols == 8 ||
                         shape.cols == 10 || shape.cols == 11))
                    yaw00s_[base] = shape.clone();
                else
                    Err("%x %s bad format", bits, base);
            }
        }
        if (normalshape)
        {
            // check that all shapes have same number of points
            static bool noted;
            if (nrows == -1) // first shape?
                nrows = shape.rows;
            else if (shape.rows != nrows && !noted)
            {
                lprintf("\nNote: %s has %d row%s but %s has %d row%s\n",
                        base, shape.rows, plural(shape.rows),
                        bases_[0].c_str(), nrows, plural(nrows));
                noted = true;
            }
            shapes_.push_back(shape.clone());
            bases_.push_back(base);
            bits_.push_back(bits);
            int len = STRNLEN(base, 100);
            if (len > nchar_)
                nchar_ = len;
        }
    }
    fclose(file);
    nshapes_ = NSIZE(shapes_);
    lprintf("%d shape%s\n", nshapes_, plural(nshapes_));
    if (nshapes_ == 0)
        Err("No shapes in %s", shapepath);
}

void ShapeFile::SubsetRegex_( // select shapes matching regex
    const char* sregex)       // in: regex string, NULL or "" to match any
{
    if (sregex && sregex[0])
    {
        const regex re(CompileRegex(sregex));
        int nshapes = 0;
        for (int i = 0; i < nshapes_; i++)
            if (MatchRegex(bases_[i], re))
                {
                    shapes_[nshapes] = shapes_[i];
                    bases_ [nshapes] = bases_ [i];
                    bits_  [nshapes] = bits_  [i];
                    nshapes++;
                }

        if (nshapes == 0)
            Err("No shapes in %s match \"%s\"", shapepath_, sregex);
        shapes_.resize(nshapes);
        bases_.resize(nshapes);
        bits_.resize(nshapes);
        nshapes_ = nshapes;
    }
}

// How mask0 and mask1 work:
//
//      if (mask0=0 && mask1=0)
//          match all shapes
//      else if (mask1 == AT_Any)
//          match if any tagbit is in mask0
//      else
//          match if (tagbits & mask0) == mask1

void ShapeFile::SubsetMask_( // select shapes matching mask0 and mask1
    unsigned    mask0,       // in
    unsigned    mask1)       // in
{
    if (mask0 || mask1) // mask0=0 mask1=0 matches everything
    {
        int nshapes = 0;
        for (int i = 0; i < nshapes_; i++)
        {
            const unsigned bits = bits_[i];
            // mask1 = AT_Any is a special case meaning match any bit in mask0
            if (mask1 == AT_Any? (bits & mask0): (bits & mask0) == mask1)
            {
                shapes_[nshapes] = shapes_[i];
                bases_ [nshapes] = bases_[i];
                bits_  [nshapes] = bits;
                nshapes++;
            }
        }
        if (nshapes == 0)
            Err("No shapes in %s match mask0 0x%x mask1 0x%x",
                shapepath_, mask0, mask1);
        shapes_.resize(nshapes);
        bases_.resize(nshapes);
        bits_.resize(nshapes);
        nshapes_ = nshapes;
    }
}

static void RandInts(
    vec_int& ints,    // out: scrambled integers in range 0 ... NSIZE(ints)-1
    int      seed)    // in:  random seed
{
    const int n = NSIZE(ints);
    CV_Assert(n > 0);
    if (n > RAND_MAX)
        Err("vector size %d is too big (max allowed is %d)", n, RAND_MAX);
    CV_Assert(seed != 0);
    if (seed == 1)       // 1 has a special meaning which we don't want
        seed = int(1e6); // arb

    int i;
    for (i = 0; i < n; i++)
        ints[i] = i;

    srand((unsigned int)seed);

    // We use our own random shuffle here because different compilers
    // give different results which messes up regression testing.
    // (I think only Visual C 6.0 is incompatible with everyone else?)
    //
    // Following code is equivalent to
    //    random_shuffle(ints.begin(), ints.end(),
    //       pointer_to_unary_function<int,int>(RandInt));

    vec_int::iterator it = ints.begin();
    for (i = 2; ++it != ints.end(); i++)
        iter_swap(it, ints.begin() + rand() % n);
}

static void CheckNshapes(
    int         nshapes,      // in
    int         nallowed,     // in
    const char* sregex,       // in: regex string (used only for err msgs)
    const char* shapepath)    // in: shapfile path (used only for err msgs)
{
    if (nshapes <= 0)
        Err("Invalid number of shapes %d", nshapes);
    if (nshapes > nallowed)
    {
        char s1[SLEN]; s1[0] = 0;
        if (sregex && sregex[0]) // regular expression specified?
            sprintf(s1, "after matching regex %s", sregex);
        else
            sprintf(s1, "in %s", shapepath);
        Err("Want %d shape%s but only %d shape%s %s",
             nshapes, plural(nshapes),
             nallowed, plural(nallowed), s1);
    }
}

void ShapeFile::SubsetN_(     // select nshapes
    int nshapes,              // in: number of shapes to select
    int seed,                 // in: if 0 use first nshapes,
                              //     else rand subset of nshapes
    const char* sregex)       // in: regex string (used only for err msgs)
{
    CheckNshapes(nshapes, nshapes_, sregex, shapepath_);
    if (seed && nshapes > 1)
    {
        vec_int ints(nshapes_);
        RandInts(ints, seed);
        ints.resize(nshapes);
        sort(ints.begin(), ints.end());
        for (int i = 0; i < nshapes; i++)
        {
            const int j = ints[i];
            shapes_[i] = shapes_[j];
            bases_[i] = bases_[j];
            bits_[i] = bits_[j];
        }
    }
    nshapes_ = nshapes;
    shapes_.resize(nshapes);
    bases_.resize(nshapes);
    bits_.resize(nshapes);
}

static int Nchar(
    const vector<string>& bases) // in: basename of each shape
{
    int nchar = 0; // nbr of chars in longest string in bases
    for (int i = 0; i < NSIZE(bases); i++)
        if (int(bases[i].length()) > nchar)
            nchar = int(bases[i].length());
    return nchar;
}

void ShapeFile::Subset_( // select a subset of the shapes
    const char* sregex,  // in: regex string, NULL or "" to match any
    unsigned    mask0,   // in: select subset that matches mask0 and mask1
    unsigned    mask1,   // in: (see SubsetMask_)
    int         nshapes, // in: number of shapes to select
    int         seed)    // in: if 0 use first nshapes,
                         //     else rand subset of nshapes
{
    SubsetRegex_(sregex);      // select shapes matching the regex
    SubsetMask_(mask0, mask1); // select shapes matching mask0 and mask1
    if (nshapes)
        SubsetN_(nshapes, seed, sregex); // select nshapes

    nchar_ = Nchar(bases_);

    if (nshapes == 1 || nshapes_ == 1)
        lprintf("Using 1 shape");
    else if (nshapes == 0)
        lprintf("Using all %d shapes", nshapes_);
    else if (seed == 0)
        lprintf("Using the first %d shape%s", nshapes, plural(nshapes));
    else
        lprintf("Using a random subset of %d shape%s", nshapes, plural(nshapes));
    if (sregex && sregex[0])
    {
        lprintf(" matching %s", sregex);
        if (mask0 || mask1)
            lprintf(" and mask 0x%x 0x%x", mask0, mask1);
    }
    else if (mask0 || mask1)
         lprintf(" matching mask 0x%x 0x%x", mask0, mask1);
    if (NSIZE(bases_) > 1)
        lprintf(" (%s ... %s)",
                bases_[0].c_str(), bases_[nshapes_-1].c_str());
    lprintf("\n");
    if (mask0)
    {
        char s[SLEN]; s[0] = 0;
        AttrBitsToString(s, mask0, -1, " ");
        lprintf("(Mask0 0x%x is%s)\n", mask0, s);
    }
}

bool ShapeFile::MatchPose(        // shape is in given pose range?
    const string& base,           // in: shape's basename
    int           minyaw,         // in: minyaw == maxyaw means any yaw
    int           maxyaw,         // in
    int           minpitch,       // in
    int           maxpitch,       // in
    int           minroll,        // in
    int           maxroll)        // in
{
    CV_Assert(minyaw   >= -90 && maxyaw   <= 90);
    CV_Assert(minyaw <= maxyaw);
    CV_Assert(minpitch >= -90 && maxpitch <= 90);
    CV_Assert(minpitch <= maxpitch);
    CV_Assert(minroll  >= -90 && maxroll  <= 90);
    CV_Assert(minroll <= maxroll);

    const VEC pose(Pose_(base));

    // check that all fields of pose are valid
    bool valid = true;
    for (int k = 0; k < 4; k++)     // yaw, pitch, roll, err
        if (!Valid(pose(k)))
            valid = false;
    if (!valid)
    {
        lprintf("ignoring %s (no pose data in the shape file)\n", base.c_str());
        return false;       // note return
    }
    // sanity check that all fields of pose are within -180 to 180
    for (int k = 0; k < 3; k++) // yaw, pitch, roll
        if (pose(k) < -180 || pose(k) > 180)
            valid = false;
    if (!valid)
    {
        lprintf("ignoring %s (out-of-range pose: yaw %g pitch %g roll %g)\n",
                base.c_str(), pose(0), pose(1), pose(2));
        return false;       // note return
    }
    if (minyaw != maxyaw     && (pose(0) < minyaw   || pose(0) > maxyaw))
        return false;
    if (minpitch != maxpitch && (pose(1) < minpitch || pose(1) > maxpitch))
        return false;
    if (minroll != maxroll   && (pose(2) < minroll  || pose(2) > maxroll))
        return false;
    return true;
}

void ShapeFile::PoseSubset_( // select a subset of the shapes based on pose
    int minyaw,              // in: minyaw == maxyaw means any yaw
    int maxyaw,              // in
    int minpitch,            // in
    int maxpitch,            // in
    int minroll,             // in
    int maxroll)             // in
{
    if (minyaw == maxyaw && minpitch == maxpitch && minroll == maxroll)
        return; // want all poses,  note return

    int nshapes = nshapes_; // original number of shapes
    int j = 0;
    for (int i = 0; i < nshapes_; i++)
    {
        if (MatchPose(bases_[i], minyaw, maxyaw,
                      minpitch, maxpitch, minroll, maxroll))
        {
            shapes_[j] = shapes_[i];
            bases_ [j] = bases_ [i];
            bits_  [j] = bits_  [i];
            j++;
        }
    }
    if (j == 0)
        Err("No shapes in %s match the given pose range", shapepath_);
    nchar_ = Nchar(bases_);
    shapes_.resize(j);
    nshapes_ = j;

    if (nshapes_ == 1)
        lprintf("1 shape matches");
    else if (nshapes == nshapes_)
        lprintf("All %d shapes match", nshapes_);
    else
        lprintf("%d shapes match", nshapes_);
    lprintf(" yaw %d %d pitch %d %d roll %d %d",
            minyaw, maxyaw, minpitch, maxpitch, minroll, maxroll);
    if (NSIZE(bases_) > 1)
        lprintf(" (%s ... %s)",
                bases_[0].c_str(), bases_[NSIZE(bases_)-1].c_str());
    lprintf("\n");
}

// print x with at most one decimal digit, and no .0 if not necessary
static void Print(FILE* file, double x, const char* msg)
{
    const double x0 = cvRound(10 * x) / 10.; // round to 1 decimal digit
    Fprintf(file, int(x0) == x0? "%.0f%s":  "%.1f%s", x, msg);
}

// TODO this does not yet write the poses

void ShapeFile::Write_(      // create a shapefile
        const char* path,    // in: the shapefile name
        const char* dirs,    // in: can be NULL
        const char* comment) // in: can be NULL
const
{
    FILE* file = fopen(path, "wb");
    if (!file)
        Err("Cannot open %s for writing", path);
    Fprintf(file, "shape # %s\n\n", path);
    if (comment && comment[0])
        Fprintf(file, "%s\n", comment);
    Fprintf(file, "Directories %s\n\n", dirs);
    for (int ishape = 0; ishape < nshapes_; ishape++)
    {
        Fprintf(file, "\"%8.8x %s\"\n", bits_[ishape], bases_[ishape].c_str());
        const Shape& shape(shapes_[ishape]);
        Fprintf(file, "{ %d %d\n", shape.rows, shape.cols);
        for (int i = 0; i < shape.rows; i++)
        {
           Print(file, shape(i, IX), " ");
           Print(file, shape(i, IY), "\n");
        }
        Fprintf(file, "}\n");
    }
    fclose(file);
}

static void OpenFitFile(
    FILE*&      fitfile,   // out: opened fit file
    const char* shapepath, // in
    int         nshapes,   // in
    int         seed,      // in
    const char* sregex,    // in
    int         nchar)     // in
{
    MkDir("fit");

    char sregex1[SLEN]; sregex1[0] = 0;
    if (sregex && sregex[0])
    {
        sprintf(sregex1, "_REGEX_%s", sregex);
        RegexToLegalFilename(sregex1);
    }

    const char* smod = stasm_VERSION;
    while (*smod && *smod != '_') // skip to first underscore
        smod++;
    if (*smod != '_')             // no underscore?
        smod = "";

    char snshapes[SLEN]; snshapes[0] = 0;
    if (nshapes)
        sprintf(snshapes, "_n%d_seed%d", nshapes, seed);

    char fitpath[SLEN];
    sprintf(fitpath, "fit/%s%s%s%s.fit", Base(shapepath), snshapes, sregex1, smod);
    lprintf("Writing %s\n", fitpath);
    fitfile = fopen(fitpath, "wb");
    if (!fitfile)
        Err("Cannot open %s for writing", fitpath);
    Fprintf(fitfile,
            "%-*s meanfit iworst    me17    fm29 iworstfm29 "
            "intereye eyemouth "
            "estyaw    yaw  pitch   roll   poseerr dettime asmtime\n",
            nchar+1, "file");
}

void ProcessShapeFileArg(
    ShapeFile&   sh,       // out
    FILE**       fitfile,  // out: opened fit file (NULL if no fit file)
    int&         argc,     // in
    const char** argv,     // in: file.shape [N [SEED [REGEX]]]
    unsigned     mask0,    // in: select subset that matches mask0 and mask1
    unsigned     mask1)    // in: (see SubsetMask_)
{
    char shapepath[SLEN];
    int  nshapes;
    int  seed;
    char sregex[SBIG];

    if (argc < 1)
        Err("No shapefile argument.  Use swas -? for help.");

    strcpy(shapepath, *argv);
    int n = STRNLEN(shapepath, SLEN);

// Modified by Yuji Oyamada, 2013.02.01
    #if _MSC_VER // microsoft
        if (n < 6 || _stricmp(shapepath + n - 6, ".shape"))
            Err("Invalid shape file name %s (expected a .shape suffix)", shapepath);
    #else
        if (n < 6 || strcmp(shapepath + n - 6, ".shape"))
            Err("Invalid shape file name %s (expected a .shape suffix)", shapepath);
    #endif

    nshapes = 0;
    if (argc > 1)
    {
        argv++;
        argc--;
        if (1 != sscanf(*argv, "%d", &nshapes) || nshapes < 0 || nshapes > 1e6)
           Err("Invalid N argument %s (use 0 for all, else positive integer)", *argv);
    }
    seed = 0;
    if (argc > 1)
    {
        argv++;
        argc--;
        if (1 != sscanf(*argv, "%d", &seed))
           Err("Invalid SEED argument %s "
                "(must be an integer, use 0 for first N shapes)", *argv);
    }
    sregex[0] = 0;
    if (argc > 1)
    {
        argv++;
        argc--;
        if (1 != sscanf(*argv, "%s", sregex))
           Err("Invalid regex argument %s", *argv);
    }
    if (argc != 1)
        Err("Too many arguments, do not know what to do with \"%s\"", *(argv+1));

    srand((unsigned int)seed); // for SynthesizeFaceDetFromRefShape
    sh.Open_(shapepath);
    sh.Subset_(sregex, mask0, mask1, nshapes, seed);
    if (fitfile)
        OpenFitFile(*fitfile, shapepath, nshapes, seed, sregex, sh.nchar_);
}

void CheckAllShapesHaveSameNumberOfLandmarks(
    const ShapeFile& sh) // in
{
    const int npoints = sh.shapes_[0].rows;
    for (int ishape = 0; ishape < sh.nshapes_; ishape++)
        if (sh.shapes_[ishape].rows != npoints)
            Err("%s: %s has %d points but %s has %d points",
                sh.shapepath_,
                sh.bases_[ishape].c_str(), sh.shapes_[ishape].rows,
                sh.bases_[0].c_str(), sh.shapes_[0].rows);
}

const map_mat* GetYawMap(     // get the appropriate yaw map from the shapefile
    const ShapeFile& sh,      // in
    const char*      modname) // in: e.g. "yaw00"
{
    // TODO for now, we handle only yaw00, later for multiview models will handle more
    if (strcmp(modname, "yaw00"))
    {
        static int printed;
        PrintOnce(printed, "GetYawMap: %s unrecognized, treating as \"yaw00\"");
    }
    return &sh.yaw00s_;
}

} // namespace stasm

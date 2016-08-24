// swas.cpp: Run stasm on the images listed in a shapefile.
//           Compare landmark results to manually landmarked points and write
//           results to a "fit file" for post-processing (e.g. with swas.R).
//
// Copyright (C) 2005-2013, Stephen Milborrow

#include "stasm.h"
#include "opencv/highgui.h"
#include "../appmisc.h"
#include "../shapefile/shapefile.h"
#include "../shapefile/stasm_regex.h"
#include "fitmeas.h"
#include "fm29.h"

namespace stasm
{
static bool crop_g;                         // -c flag
static bool fitprefix_g = true;             // -f flag
static bool writeimgs_g = true;             // -i flag
static bool quiet_g;                        // -q flag
static int  minwidth_g = 25;                // -s -S flags
static bool ignore_multiface_imgs_g = true; // -M flag

// -q flag, func pointer to lprintf or logprintf
static void (*printf_g)(const char* pArgs, ...) = &lprintf;

//-----------------------------------------------------------------------------

static void Init(
    ShapeFile&   sh,        // out
    FILE*&       fitfile,   // out: opened fit file
    int          argc,      // in
    const char** argv)      // in
{
    static const char* const usage =
"swas [FLAGS] file.shape [N [SEED [REGEX]]]\n"
"\n"
"Examples:\n"
"    swas file.shape               (all faces in file.shape)\n"
"    swas file.shape 3             (first 3 faces in file.shape)\n"
"    swas file.shape 3 99          (3 random faces, random seed is 99)\n"
"    swas file.shape 0 0 x         (all faces with \"x\" in their name)\n"
"    swas file.shape 3 99 \"xy|z\"   (3 random faces with \"xy\"  or \"z\" in their name)\n"
"\n"
"Flags:\n"
"    -c  crop output images to face\n"
"    -d  enable debug prints\n"
"    -i  do not write landmarked images (faster)\n"
"    -f  do not prefix image names with fit\n"
"    -q  quiet\n"
"    -s  detect small faces (min width 10%% of image width, the default is 25%%)\n"
"    -S  detect very small faces (min width 5%% of image width)\n"
"    -L  no landmark search (just face det)\n"
"    -M  do not ignore images with more than one face (else use AT_MultiFace bit)\n"
"\n"
"    -?  help\n"
"\n"
"Swas version %s    www.milbo.users.sonic.net/stasm\n"; // %s is stasm_VERSION

    if (argc < 2)
        Err("No shapefile argument.  Use swas -? for help.");
    while (--argc > 0 && (*++argv)[0] == '-')
    {
        if ((*argv + 1)[1])
            Err("Invalid argument -%s (there must be a space after -%c).  "
                "Use swas -? for help.",
                *argv + 1, (*argv + 1)[0]);
        switch ((*argv + 1)[0])
        {
        case 'c':
            crop_g = true;
            break;
        case 'd':
            trace_g = true;
            break;
        case 'f':
            fitprefix_g = false;
            break;
        case 'i':
            writeimgs_g = false;
            break;
        case 'q':
            quiet_g = true;
            printf_g = &logprintf;  // print only to the log file
            break;
        case 's':
            minwidth_g = 10;
            break;
        case 'S':
            minwidth_g = 5;
            break;
        case 'M':
            ignore_multiface_imgs_g = true;
            break;
        case '?':               // -?
            printf(usage, stasm_VERSION);
            exit(1);
        default:
            Err("Invalid argument -%s.  Use swas -? for help.", *argv + 1);
            break;
        }
    }
    ProcessShapeFileArg(sh, &fitfile, argc, argv, 0, 0); // argv is now the file.shape
}

static void WriteLandmarkedImg(
    const Image& img,         // in: the image
    const char*  newpath,     // in: the image path
    const Shape& shape,       // in: the shape
    const Shape& cropshape,   // in: crop image to this shape (typically the ref shape)
    unsigned     color=C_RED, // in: rrggbb e.g. 0xff0000 is red
    int          iworst=-1)   // in: index of worst fitting point, -1 for none
{
    CImage cimg; cvtColor(img, cimg, CV_GRAY2BGR); // color image

    if (iworst >= 0) // draw circle at worst fitting point?
        cv::circle(cimg,
                   cv::Point(cvRound(shape(iworst, IX)),
                             cvRound(shape(iworst, IY))),
                   MAX(2, cvRound(ShapeWidth(shape) / 40)),
                   cv::Scalar(255, 255, 0), 2); // cyan

    DrawShape(cimg, shape, color);

    if (crop_g)
        CropCimgToShapeWithMargin(cimg, cropshape);

    if (!cv::imwrite(newpath, cimg))
        Err("Cannot write %s", newpath);
}

static void WriteImgs(
    const Image& img,                   // in
    const char*  imgpath,               // in
    const Shape& shape,                 // in
    const Shape& refshape,              // in
    double       meanfit_over_intereye, // in
    int          iworst,                // in
    double       me17,                  // in
    double       fm29,                  // in
    int          iworst_fm29)           // in
{
    const char* const base = Base(imgpath);
    char newpath[SLEN];
    double fit = 0;
    if (fm29 != NOFIT) // prefer fm29 if it is available
    {
        if (fitprefix_g)
            sprintf(newpath, "%5.3f_%s_fm29.bmp", fm29, base);
        else
            sprintf(newpath, "%s_stasm.bmp", base);
        WriteLandmarkedImg(img, newpath, shape, refshape, C_RED, iworst_fm29);
        fit = fm29;
    }
    else if (me17 != NOFIT)
    {
        if (fitprefix_g)
            sprintf(newpath, "%5.3f_%s_me17.bmp", me17, base);
        else
            sprintf(newpath, "%s_stasm.bmp", base);
        WriteLandmarkedImg(img, newpath, shape, refshape);
        fit = me17;
    }
    else
    {
        CV_Assert(meanfit_over_intereye != NOFIT);
        if (fitprefix_g)
            sprintf(newpath, "%5.3f_%s_meanfit.bmp", meanfit_over_intereye, base);
        else
            sprintf(newpath, "%s_stasm.bmp", base);
        WriteLandmarkedImg(img, newpath, shape, refshape, C_RED, iworst);
        fit = meanfit_over_intereye;
    }
    // reference shape in yellow
    if (fitprefix_g)
        sprintf(newpath, "%5.3f_%s__ref.bmp", fit, base);
    else
        sprintf(newpath, "%s__ref.bmp", base);
    WriteLandmarkedImg(img, newpath, refshape, refshape, C_YELLOW);
}

static void ProcessFace(
    const Image&     img,            // in
    const char*      imgpath,        // in
    int              foundface,      // in
    const Shape&     shape,          // in
    float            estyaw,         // in
    double           facedet_time,   // in
    double           asmsearch_time, // in
    const Shape&     refshape,       // in
    FILE*            fitfile,        // in
    const ShapeFile& sh,             // in
    int              ishape)         // in: shape index in the shapefile
{
    double meanfit = NOFIT; // fitness measure over all shapefile points
    int iworst = -1;        // worst fitting point in above measure of fitness
    double me17 = NOFIT;    // fitness measure me17
    double fm29 = NOFIT;    // fitness measure FM29 (only for 77 point shapes)
    int iworst_fm29 = -1;   // worst fitting point using FM29 measure
    if (!foundface)
        printf_g("no face ");
    else
    {
        if (trace_g)
            LogShape(shape, imgpath);
        // we will succesfully get the meanfit only if the shape can be converted to
        // the shapefile number of points (by ConvertShape in MeanFitOverInterEye)
        meanfit = MeanFitOverInterEye(iworst, shape, refshape);
        if (meanfit != NOFIT) // were able to get the mean fit?
            printf_g("meanfit %5.3f ", meanfit);
        // use fitness measure me17 if can convert the shape to a shape17
        me17 = Me17(shape, refshape);
        if (me17 != NOFIT) // were able to get the me17 fit?
            printf_g("me17 %5.3f ", me17);
        // get fitness measure fm29 if the shape has 77 points
        if (shape.rows == 77 && refshape.rows == 77)
        {
            Fm29(fm29, iworst_fm29, shape, refshape);
            printf_g("fm29 %5.3f ", fm29);
        }
        if (writeimgs_g) // -i flag
            WriteImgs(img, imgpath, shape, refshape,
                      meanfit, iworst, me17, fm29, iworst_fm29);
    }
    printf_g("\n");
    if (trace_g)
        lprintf("\n");
    const char* const base = Base(imgpath);
    const VEC pose(sh.Pose_(base));
    Fprintf(fitfile,
        "%-*s%s "
        "%7.5f % 6d "
        "%7.5f "
        "%7.5f     % 6d "
        "%8.2f %8.2f  ",
        sh.nchar_, base, " ",
        meanfit, iworst,
        me17,
        fm29, iworst_fm29,
        InterEyeDist(refshape), EyeMouthDist(refshape));
    Fprintf(fitfile,
        "% 5.0f "
        "% 6.0f % 6.0f % 6.0f %9.3f "
        "[%5.3f] [%5.3f]\n",
        estyaw,
        pose(0), pose(1), pose(2), pose(3),
        facedet_time, asmsearch_time);
}

static void ProcessShapes(
    const ShapeFile& sh,      // in
    FILE*            fitfile) // in
{
    Pacifier pacifier(sh.nshapes_); // only used if quiet_g
    for (int ishape = 0; ishape < sh.nshapes_; ishape++)
    {
        if (quiet_g)
            pacifier.Print_(ishape);
        else
        {
            if (sh.nshapes_ > 1)
                lprintf("%*d ", NumDigits(sh.nshapes_), ishape);
            lprintf("%*.*s:%s", sh.nchar_, sh.nchar_,
                    sh.bases_[ishape].c_str(), trace_g? "\n": " ");
        }
        if (ignore_multiface_imgs_g && (sh.bits_[ishape] & AT_MultiFace))
        {
            printf_g("multiple face, skipping\n");
            continue; // note continue
        }
        const char* imgpath = PathGivenDirs(sh.bases_[ishape], sh.dirs_, sh.shapepath_);
        Image img(cv::imread(imgpath, CV_LOAD_IMAGE_GRAYSCALE));
        if (!img.data)
            Err("Cannot load %s", imgpath);
        const clock_t start_time = clock();
        const Shape refshape(sh.shapes_[ishape]);
        const Shape refshape17(Shape17OrEmpty(refshape));
        if (refshape17.rows) // converted to a Shape17?
            SanityCheckShape17(refshape17);
        if (!stasm_open_image((const char*)img.data, img.cols, img.rows, imgpath,
                              0 /*multi*/, minwidth_g))
            Err("stasm_open_image failed:  %s", stasm_lasterr());

        clock_t start_time1 = clock();
        const double facedet_time = double(start_time1 - start_time) / CLOCKS_PER_SEC;

        int foundface = false;
        float estyaw = 0; // estimated yaw
        float landmarks[2 * stasm_NLANDMARKS]; // x,y coords

        if (!stasm_search_auto_ext(&foundface, landmarks, &estyaw))
            Err("stasm_search_auto failed: %s", stasm_lasterr());

        const Shape shape(LandmarksAsShape(landmarks));

        const double asmsearch_time = double(clock() - start_time) / CLOCKS_PER_SEC;

        ProcessFace(img, imgpath, foundface, shape, estyaw,
                    facedet_time, asmsearch_time, refshape, fitfile, sh, ishape);
    }
    if (quiet_g)
    {
        pacifier.End_();
        printf("\n");
    }
}

static void main1(int argc, const char** argv)
{
    OpenLogFile();
    print_g = true; // want to be able to see lprintfs
    const bool old_trace = trace_g;
    if (!stasm_init("../data", 1 /*trace*/))
        Err("stasm_init failed %s", stasm_lasterr());
    trace_g = old_trace;

    ShapeFile sh;  // contents of the shape file
    FILE* fitfile; // the fit file we will create

    Init(sh, fitfile, argc, argv);

    const clock_t start_time = clock();
    ProcessShapes(sh, fitfile);
    lprintf("[MeanTimePerImg %.3f]\n",
        double(clock() - start_time) / (sh.nshapes_ * CLOCKS_PER_SEC));
    fclose(fitfile);
}

} // namespace stasm

// This application calls Stasm's internal routines.  Thus we need to catch a
// potential throw from Stasm's error handlers.  Hence the try/catch code below.

int main(int argc, const char** argv)
{
    stasm::CatchOpenCvErrs();
    try
    {
        stasm::main1(argc, argv);
    }
    catch(...)
    {
        // a call was made to Err or a CV_Assert failed
        printf("\n%s\n", stasm_lasterr());
        exit(1);
    }
    return 0; // success
}

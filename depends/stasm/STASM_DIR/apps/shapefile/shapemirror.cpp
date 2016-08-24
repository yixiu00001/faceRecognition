// shapemirror.cpp: Mirror the shapes in a shape file
//                  and write the mirrored images.

#include <sys/types.h>
#include <sys/stat.h>
#include "opencv/highgui.h"
#include "stasm.h"
#include "../appmisc.h"
#include "shapefile.h"

namespace stasm
{
static bool writeimgs_g = true; // -i flag
static bool quiet_g;            // -q flag

static void GetOptions(
    int&          argc, // in
    const char**& argv) // io
{
    static const char* const usage =
        "shapemirror [FLAGS] file.shape\n"
        "\n"
        "Flags:\n"
        "    -i  do not write mirrored images\n"
        "    -q  quiet\n"
        "    -?  help\n";
    if (argc < 2)
        Err("No shapefile.  Use shapemirror -? for help.");
    while (--argc > 0 && (*++argv)[0] == '-')
    {
        if ((*argv + 1)[1])
            Err("Invalid argument -%s (there must be a space after -%c).  "
                "Use shapemirror -? for help.",
                *argv + 1, (*argv + 1)[0]);
        switch ((*argv + 1)[0])
        {
        case 'i':
            writeimgs_g = false;
            break;
        case '?':
            printf(usage); fflush(stdout);
            exit(1);
        default:
            Err("Invalid argument -%s.  Use shapemirror -? for help.", *argv + 1);
            break;
        }
    }
    if (argc != 1)
        Err("No shapefile.  Use shapemirror -? for help.");
    if (writeimgs_g)
        MkDir("new");
}

static void FlipPoint(
    Shape&       shape,    // io
    const Shape& oldshape, // in
    int          inew,     // in
    int          iold,     // in
    int          imgwidth) // in
{
    if (!PointUsed(oldshape, iold))
        shape(inew, IX) = shape(inew, IY) = 0;
    else
    {
        shape(inew, IX) = imgwidth - oldshape(iold, IX) - 1;
        shape(inew, IY) = oldshape(iold, IY);
        if (!PointUsed(shape, inew))   // falsely marked unused after conversion?
            shape(inew, IX) = XJITTER; // adjust so not marked as unused
    }
}

Shape FlipShapeExt( // flip shape horizontally, extended version
    const Shape& shape,    // in
    int          imgwidth) // in
{
    CV_Assert(NELEMS(LANDMARK_INFO_TAB) == shape.rows);
    const LANDMARK_INFO* info = LANDMARK_INFO_TAB;
    Shape outshape(shape.rows, 2);
    for (int i = 0; i < shape.rows; i++)
    {
        int partner = info[i].partner;
        if (partner == -1) // no partner e.g. tip of nose
            partner = i;
        FlipPoint(outshape, shape, partner, i, imgwidth);
    }
    return outshape;
}

static void Print(FILE* file, double x, const char* msg)
{
    Fprintf(file, int(x) == x? "%.0f%s":  "%.1f%s", x, msg);
}

static void ProcessShapes(
    const ShapeFile& sh,   // in
    FILE*            file) // in
{
    Pacifier pacifier(sh.nshapes_);
    for (int ishape = 0; ishape < sh.nshapes_; ishape++)
    {
        // we need the image's width, so must load the image off disk
        const char* path = PathGivenDirs(sh.bases_[ishape], sh.dirs_, sh.shapepath_);
        logprintf("\n");
        if (quiet_g)
            pacifier.Print_(ishape);
        else
        {
            printf("%.1f%% %d %s\n", 100. * ishape / sh.nshapes_, ishape, path);
            fflush(stdout);
        }
        CImage img(cv::imread(path, CV_LOAD_IMAGE_COLOR));
        if (!img.data)
            Err("Cannot load %s", path);
        Shape shape(sh.shapes_[ishape]);
        Shape newshape(FlipShapeExt(shape, img.cols));
        Fprintf(file, "\"%8.8x %sr\"\n", sh.bits_[ishape], sh.bases_[ishape].c_str());
        Fprintf(file, "{ %d %d\n", newshape.rows, newshape.cols);
        for (int i = 0; i < newshape.rows; i++)
        {
           Print(file, newshape(i, IX), " ");
           Print(file, newshape(i, IY), "\n");
        }
        Fprintf(file, "}\n");
        if (writeimgs_g)
        {
            char newpath[SLEN]; sprintf(newpath, "mirrored/%sr.jpg", Base(path));
            CImage flipped; cv::flip(img, flipped, 1);
            vec_int params(2); params[0] = CV_IMWRITE_JPEG_QUALITY; params[1] = 95;
            if (imwrite(newpath, flipped, params) != 1)
                Err("Cannot write %s", newpath);
        }
    }
    if (quiet_g)
    {
        pacifier.End_();
        lprintf("\n");
    }
}

static void ProcessFaceDets(
    const ShapeFile& sh,   // in
    FILE*            file) // in
{
    if (NSIZE(sh.yaw00s_))
        lprintf("Ignoring facedet records (run Tasm to generate the facedets)\n");
}

static void ProcessPoses(
    const ShapeFile& sh,   // in
    FILE*            file) // in
{
    lprintf("Processing pose data: ");
    int nposes = 0;
    for (int ishape = 0; ishape < sh.nshapes_; ishape++)
    {
        const VEC pose(sh.Pose_(Base(sh.bases_[ishape].c_str())));
        if (Valid(pose(0)))
        {
            static bool firsttime = true;
            if (firsttime)
            {
                firsttime = false;
                Fprintf(file, "# poses\n");
            }
            nposes++;
            Fprintf(file, "\"%8.8x %sr\"\n", AT_Pose, sh.bases_[ishape].c_str());
            Fprintf(file, "{ 1 4 %g %g %g %g\n}\n",
                -pose(0),  // yaw
                pose(1),   // pitch
                -pose(2),  // roll
                pose(3));  // err
        }
    }
    lprintf("%d record%s\n", nposes, plural(nposes));
}

static void main1(int argc, const char** argv)
{
    GetOptions(argc, argv);
    OpenLogFile();
    print_g = true;    // want to be able to see lprintfs
    if (writeimgs_g)
    {
        if (DirWriteable("mirrored", false))
            System(ssprintf("rm -f mirrored/*.jpg"));
        MkDir("mirrored");
    }
    ShapeFile sh;      // contents of the shape file
    sh.Open_(argv[0]); // argv[0] is the shapefile path
    CheckAllShapesHaveSameNumberOfLandmarks(sh);
    if (NELEMS(LANDMARK_INFO_TAB) != sh.shapes_[0].rows)
    {
        // If you get this message, you probably need to modify
        // landmarks.h to include your landmark table and then
        // rebuild this program.
        Err("NELEMS(LANDMARK_INFO_TAB) %d != number of points %d in %s",
            NELEMS(LANDMARK_INFO_TAB), sh.shapes_[0].rows, BaseExt(sh.shapepath_));
    }
    char newpath[SLEN];
    sprintf(newpath, "%s_r.shape", Base(argv[0]));
    lprintf("Generating %s ", newpath);
    if (writeimgs_g)
        lprintf("and mirrored/*r.jpg ");
    if (quiet_g)
        logprintf("\n");
    else
        lprintf("\n");
    FILE* file = fopen(newpath, "wb");
    if (!file)
        Err("Cannot open %s for writing", newpath);
    Fprintf(file, "shape %s\n\n", newpath);
    Fprintf(file, "Directories mirrored;%s\n\n", sh.dirs_);
    ProcessShapes(sh, file);
    ProcessFaceDets(sh, file);
    ProcessPoses(sh, file);
    fclose(file);
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
    return 0;       // success
}

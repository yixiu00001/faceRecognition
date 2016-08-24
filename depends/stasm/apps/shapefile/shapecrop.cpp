// shapecrop.cpp: Crop the images in a shape file, create a new
//                shapefile, and write the cropped images to disk.
//
// We crop to the area around the face.  We leave enough space around the
// face so that Stasm training or testing with the cropped images is
// (almost?) identical to using the full size images.  The advantage is
// that the cropped images are smaller and easier to deal with.

#include <sys/types.h>
#include <sys/stat.h>
#include "stasm.h"
#include "opencv/highgui.h"
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
        "shapecrop [FLAGS] file.shape\n"
        "\n"
        "Flags:\n"
        "    -i  do not write cropped images\n"
        "    -q  quiet\n"
        "    -?  help\n";
    if (argc < 2)
        Err("No shapefile.  Use shapecrop -? for help.");
    while (--argc > 0 && (*++argv)[0] == '-')
    {
        if ((*argv + 1)[1])
            Err("Invalid argument -%s (there must be a space after -%c).  "
                "Use shapecrop -? for help.",
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
            Err("Invalid argument -%s.  Use shapecrop -? for help.", *argv + 1);
            break;
        }
    }
    if (argc != 1)
        Err("No shapefile.  Use shapecrop -? for help.");
    if (writeimgs_g)
        MkDir("new");
}

static void GetCropLeavingSpaceAroundFace(
    int&          leftcrop,               // out
    int&          rightcrop,              // out
    int&          topcrop,                // out
    int&          botcrop,                // out
    const Shape&  shape,                  // in
    const CImage& img,                    // in
    double        eyemouth,               // in
    double        xexpand,                // in
    double        topexpand,              // in
    double        botexpand)              // in
{
    double xmin, xmax, ymin, ymax; ShapeMinMax(xmin, xmax, ymin, ymax, shape);
    if (xmin > img.cols || xmax < 0 || ymin > img.rows || ymax < 0)
        Err("Entire shape is off-image "
            "(xmin %g xmax %g ymin %g ymax %g imgwidth %d imgheight %d)",
            xmin, xmax, ymin, ymax, img.cols, img.rows);
    leftcrop  = MAX(0, cvRound(xmin - xexpand * eyemouth - 1));
    rightcrop = MAX(0, img.cols - cvRound(xmax + xexpand * eyemouth + 1));
    AdjustCropSoDivBy4(leftcrop, rightcrop,  img.cols, true);
    if (quiet_g)
        logprintf("Crop ");
    else
        lprintf("Crop ");
    topcrop = MAX(0, cvRound(ymin - topexpand * eyemouth - 1));
    botcrop = MAX(0, img.rows - cvRound(ymax + botexpand * eyemouth + 1));
    AdjustCropSoDivBy4(topcrop,  botcrop, img.rows);
    if (quiet_g)
        logprintf("%4d %4d%s%4d %4d\n",
            leftcrop, rightcrop,
            ((img.cols - leftcrop - rightcrop) % 4)? "!": " ",  // x not divisible by 4?
            topcrop, botcrop);
    else
        lprintf("%4d %4d%s%4d %4d\n",
            leftcrop, rightcrop,
            ((img.cols - leftcrop - rightcrop) % 4)? "!": " ",  // x not divisible by 4?
            topcrop, botcrop);
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
        const char* path = PathGivenDirs(sh.bases_[ishape], sh.dirs_, sh.shapepath_);
        if (quiet_g)
            pacifier.Print_(ishape);
        else
        {
            printf("%.1f%% %d %s ", 100. * ishape / sh.nshapes_, ishape, path);
            fflush(stdout);
        }
        CImage img(cv::imread(path, CV_LOAD_IMAGE_COLOR));
        if (!img.data)
            Err("Cannot load %s", path);
        Shape shape(sh.shapes_[ishape]);
        const double eyemouth = EyeMouthDist(shape);
        CV_Assert(eyemouth > 0);
        // constants to control how much area we leave around the face
        const double EXPAND = 1.2; // large expand
        // const double EXPAND = .1; // small expand
        const double xexpand = EXPAND;
        const double botexpand = EXPAND;
        // topexpand must be more if shape does not have forehead points
        const double topexpand = shape.rows == 77? EXPAND: 2.0;
        // const double topexpand = EXPAND;
        int leftcrop=0, rightcrop=0, topcrop=0, botcrop=0;
        GetCropLeavingSpaceAroundFace(leftcrop, rightcrop, topcrop, botcrop,
                                      shape, img, eyemouth,
                                      xexpand, topexpand, botexpand);
        CImage cropped(img, // extract the crop ROI
                       Rect(leftcrop, topcrop,
                            img.cols - rightcrop - leftcrop,
                            img.rows - topcrop   - botcrop));
        shape = ShiftShape(shape, -leftcrop, -topcrop);
        Fprintf(file, "\"%8.8x %s\"\n", sh.bits_[ishape], sh.bases_[ishape].c_str());
        Fprintf(file, "{ %d %d\n", shape.rows, shape.cols);
        for (int i = 0; i < shape.rows; i++)
        {
           Print(file, shape(i, IX), " ");
           Print(file, shape(i, IY), "\n");
        }
        Fprintf(file, "}\n");
        if (writeimgs_g)
        {
            char newpath[SLEN]; sprintf(newpath, "new/%s.jpg", Base(path));
            vec_int params(2); params[0] = CV_IMWRITE_JPEG_QUALITY; params[1] = 95;
            if (imwrite(newpath, cropped, params) != 1)
                Err("Cannot write %s", newpath);
        }
    }
    if (quiet_g)
    {
        pacifier.End_();
        lprintf("\n");
    }
}

static void ProcessPoses(
    const ShapeFile& sh,   // in
    FILE*            file) // in
{
    lprintf("Processing pose data: ");
    int nposes = 0;
    Fprintf(file, "# poses\n");
    for (int ishape = 0; ishape < sh.nshapes_; ishape++)
    {
        const VEC pose(sh.Pose_(Base(sh.bases_[ishape].c_str())));
        if (Valid(pose(0)))
        {
            nposes++;
            Fprintf(file, "\"%8.8x %s\"\n", AT_Pose, sh.bases_[ishape].c_str());
            Fprintf(file, "{ 1 4 %g %g %g %g\n}\n",
                pose(0),  // yaw
                pose(1),  // pitch
                pose(2),  // roll
                pose(3)); // err
        }
    }
    lprintf("%d record%s\n", nposes, plural(nposes));
}

static void main1(int argc, const char** argv)
{
    GetOptions(argc, argv);
    OpenLogFile();
    print_g = true;    // want to be able to see lprintfs
    ShapeFile sh;      // contents of the shape file
    sh.Open_(argv[0]); // argv[0] is the shapefile path
    char newpath[SLEN];
    sprintf(newpath, "%s_crop.shape", Base(argv[0]));
    lprintf("Generating %s ", newpath);
    if (writeimgs_g)
        lprintf("and new/*.jpg ");
    if (quiet_g)
        logprintf("\n");
    else
        lprintf("\n");
    sh.Write_(newpath, "new", "");
    FILE* file = fopen(newpath, "wb");
    if (!file)
        Err("Cannot open %s for writing", newpath);
    Fprintf(file, "shape %s\n\n", newpath);
    Fprintf(file, "Directories new\n\n");
    ProcessShapes(sh, file);
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

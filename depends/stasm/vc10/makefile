# makefile: makefile for Stasm using Visual C++
#
# Release build: mkdir Release; nmake CFG=Release -nologo -f ../vc10/makefile
# Debug build:   mkdir Debug;   nmake CFG=Debug   -nologo -f ../vc10/makefile
#
# milbo durban aug07

# OPENCV_HOME=E:/OpenCV2.3.1
# OPENCV_N=231
OPENCV_HOME=E:/OpenCV2.4.0
OPENCV_N=240

!IF "$(CFG)" == "Debug"
LIBSUFFIX = d
!ENDIF

!IFNDEF CFG
CFG=Release
!MESSAGE CFG not specifed on NMAKE command line
!MESSAGE Defaulting to CFG=$(CFG)
!MESSAGE To make this message go away, add CFG=Release to the nmake command line
!MESSAGE
!ENDIF

!IF  "$(CFG)" == "Release"

OUTDIR=./Release
# highgui is need for imread and imwrite in apps (not needed for stasm lib itself)
LIBS=$(OPENCV_LIBDIR)/opencv_core$(OPENCV_N).lib\
     $(OPENCV_LIBDIR)/opencv_highgui$(OPENCV_N).lib\
     $(OPENCV_LIBDIR)/opencv_imgproc$(OPENCV_N).lib\
     $(OPENCV_LIBDIR)/opencv_objdetect$(OPENCV_N).lib

!ELSEIF  "$(CFG)" == "Debug"

OUTDIR=./Debug
LIBS=$(OPENCV_LIBDIR)/opencv_core$(OPENCV_N)d.lib\
     $(OPENCV_LIBDIR)/opencv_highgui$(OPENCV_N)d.lib\
     $(OPENCV_LIBDIR)/opencv_imgproc$(OPENCV_N)d.lib\
     $(OPENCV_LIBDIR)/opencv_objdetect$(OPENCV_N)d.lib

!ELSEIF  "$(CFG)" == "ReleaseWithSymbols"

# This build is used for taking snapshots of the stack for profiling purposes.
# It is like Debug in that the symbols are generated (but no range checks etc.).
# But it is like Release in that the code is optimized.
# We use -MT not -MTd because we don't want _DEBUG defined.
# To take a noninvasive stack snapshot of stasm.exe while it is running:
#    "C:\Program Files\Debugging Tools for Windows (x64)\cdb.exe" -pv -pn stasm.exe -lines -c "~*kb 5;q"
# To see just the relevant stasm stuff:
#    "C:\Program Files\Debugging Tools for Windows (x64)\cdb.exe" -pv -pn stasm.exe -lines -c "~*kb 5;q" | egrep "stasm!|opencv_.*!"
# To see just the current function:
#    "C:\Program Files\Debugging Tools for Windows (x64)\cdb.exe" -pv -pn stasm.exe -lines -c "~*kb 5;q" | egrep "^[^ ]*!"
# For more details, see the R file profile-snap.R.

OUTDIR=./ReleaseWithSymbols
LIBS=$(OPENCV_LIBDIR)/opencv_core$(OPENCV_N).lib\
     $(OPENCV_LIBDIR)/opencv_highgui$(OPENCV_N).lib\
     $(OPENCV_LIBDIR)/opencv_imgproc$(OPENCV_N).lib\
     $(OPENCV_LIBDIR)/opencv_objdetect$(OPENCV_N).lib

!ELSE
!ERROR Invalid CFG "$(CFG)" (try something like nmake CFG=Release -f makefile)
!ENDIF

!IFNDEF MOD
MOD=MOD_1
!ENDIF

# TODO The MOD_DEPENDENCIES does not include all .mh files

!IF "$(MOD)" == "MOD_1"

MOD_DIR=MOD_1

EXTRA_MOD_OBJ=

# following is not a complete list
MOD_DEPENDENCIES=\
 ../stasm/$(MOD_DIR)/facedet.h\
 ../stasm/$(MOD_DIR)/initasm.h\
 ../stasm/$(MOD_DIR)/mh/yaw00.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev1_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev2_p00_classic.mh

!ELSEIF "$(MOD)" == "MOD_3"

MOD_DIR=MOD_3

EXTRA_MOD_OBJ=$(OUTDIR)/initasm22.obj $(OUTDIR)/initasm45.obj $(OUTDIR)/estpose.obj $(OUTDIR)/stasm_svm.obj

MOD_DEPENDENCIES=\
 ../stasm/$(MOD_DIR)/facedet.h\
 ../stasm/$(MOD_DIR)/initasm.h\
 ../stasm/$(MOD_DIR)/mh/yaw00.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw22.mh\
 ../stasm/$(MOD_DIR)/initasm22.h\
 ../stasm/$(MOD_DIR)/mh/yaw22_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw22_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw45.mh\
 ../stasm/$(MOD_DIR)/initasm45.h\
 ../stasm/$(MOD_DIR)/mh/yaw45_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw45_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/estpose.h\
 ../stasm/$(MOD_DIR)/stasm_svm.h\
 ../stasm/$(MOD_DIR)/posedet_data/epc_posedet00.mh\
 ../stasm/$(MOD_DIR)/posedet_data/epc_posedet22.mh\
 ../stasm/$(MOD_DIR)/posedet_data/epc_posedet45.mh\
 ../stasm/$(MOD_DIR)/posedet_data/svm_posedet00.mh\
 ../stasm/$(MOD_DIR)/posedet_data/svm_posedet22.mh\
 ../stasm/$(MOD_DIR)/posedet_data/svm_posedet45.mh

!ELSEIF "$(MOD)" == "MOD_A1"

MOD_DIR=MOD_A1

EXTRA_MOD_OBJ=

MOD_DEPENDENCIES=\
 ../stasm/$(MOD_DIR)/facedet.h\
 ../stasm/$(MOD_DIR)/initasm.h\
 ../stasm/$(MOD_DIR)/mh/yaw00.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p00_classic.mh

!ELSEIF "$(MOD)" == "MOD_A"

MOD_DIR=MOD_A

EXTRA_MOD_OBJ=$(OUTDIR)/initasm22.obj $(OUTDIR)/initasm45.obj

MOD_DEPENDENCIES=\
 ../stasm/$(MOD_DIR)/facedet.h\
 ../stasm/$(MOD_DIR)/initasm.h\
 ../stasm/$(MOD_DIR)/mh/yaw00.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw22.mh\
 ../stasm/$(MOD_DIR)/initasm22.h\
 ../stasm/$(MOD_DIR)/mh/yaw22_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw22_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw45.mh\
 ../stasm/$(MOD_DIR)/initasm45.h\
 ../stasm/$(MOD_DIR)/mh/yaw45_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw45_lev0_p00_classic.mh\

!ELSEIF "$(MOD)" == "MOD_A_EMU"

MOD_DIR=MOD_A

EXTRA_MOD_OBJ=$(OUTDIR)/initasm22.obj $(OUTDIR)/initasm45.obj $(OUTDIR)/MVFDCache.obj $(OUTDIR)/MVFDCacheReader.obj $(OUTDIR)/MVFDEmulator.obj

MOD_DEPENDENCIES=\
 ../stasm/$(MOD_DIR)/facedet.h\
 ../stasm/$(MOD_DIR)/initasm.h\
 ../stasm/$(MOD_DIR)/mh/yaw00.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw22.mh\
 ../stasm/$(MOD_DIR)/initasm22.h\
 ../stasm/$(MOD_DIR)/mh/yaw22_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw22_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw45.mh\
 ../stasm/$(MOD_DIR)/initasm45.h\
 ../stasm/$(MOD_DIR)/mh/yaw45_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw45_lev0_p00_classic.mh\

!ELSEIF "$(MOD)" == "MOD_E"

MOD_DIR=MOD_E

EXTRA_MOD_OBJ=

MOD_DEPENDENCIES=\
 ../stasm/$(MOD_DIR)/facedet.h\
 ../stasm/$(MOD_DIR)/initasm.h\
 ../stasm/$(MOD_DIR)/mh/yaw00.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev1_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev2_p16_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p30_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev1_p30_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev2_p30_hat.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev0_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev1_p00_classic.mh\
 ../stasm/$(MOD_DIR)/mh/yaw00_lev2_p00_classic.mh

!ELSE
!ERROR Invalid MOD "$(MOD)" (try something like nmake CFG=Release MOD=MOD_1 -f makefile)
!ENDIF

all: \
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 minimal.exe\
 minimal2.exe\
 test_stasm_lib.exe\
 test_stasm_lib_err.exe\
 shapeconvert.exe\
 shapefromstasm31.exe\
 shapemirror.exe\
 shapetostasm31.exe\
 stasm.exe\
 swas.exe\
 tasm.exe\
 wasm.exe\
 mdiff.exe

WINLIBS=advapi32.lib comctl32.lib comdlg32.lib gdi32.lib shell32.lib user32.lib

INCL=-I$(OPENCV_HOME)/build/include -I../stasm

# Following include allows this makefile to work for all supported versions of VisualC
# We put stuff that is specific to a compiler version in separate versions of makefile_include
!INCLUDE <./makefile_include>

CFLAGS=-D$(MOD) $(CFLAGS_)

# TODO remove files from clean and veryclean, add *.exe to clean?
clean:
        ..\tools\rm -f *.obj *.res Release/*.* Debug/*.* ReleaseWithSymbols/*.*
        ..\tools\rm -f ../apps/win/*.res *._xe *.obj *.map *.opt *.pdb *.ilk *.ps
        ..\tools\rm -f *.pbi *.pbo *.pbt *.pch *.ncb *.plg *.sdf
        ..\tools\rm -f *.lib stasm*.dll *.exp stasm_dll* stasm_lib* *.dsp *.dsw *.log
        ..\tools\rm -f *_stasm.bmp *_fm29.bmp *_me17.bmp *_meanfit.bmp *_ref.bmp 
        ..\tools\rm -f minimal.bmp example1.bmp example2.bmp 

veryclean: clean
        ..\tools\rm -f *.bmp *.pgm
        ..\tools\rm -f ../*/default.tag *.tar *.gz _*.shape _*.R
        ..\tools\rm -f *.vcproj.* *.vcxproj.user *.suo *.cache
        ..\tools\rm -f *.exe *.exe.manifest *.dll

# If any test below fails the make will abort with an error message

test_minimal:
        minimal
        ..\tools\cmp minimal.bmp ../tests/$(MOD_DIR)/minimal/minimal.bmp
        @..\tools\rm minimal.bmp

test_minimal2:
        minimal2
        ..\tools\cmp minimal2.bmp ../tests/$(MOD_DIR)/minimal2/minimal2.bmp
        @..\tools\rm minimal2.bmp

test_stasm_lib_multi0_minwidth25:
        test_stasm_lib 0 25 1 ../tests/data/testmulti.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_lib/multi0_minwidth25/stasm.log
        @..\tools\rm stasm.log test_stasm_lib_auto.bmp test_stasm_lib_pinned.bmp

test_stasm_lib_multi1_minwidth25:
        test_stasm_lib 1 25 1 ../tests/data/testmulti.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_lib/multi1_minwidth25/stasm.log
        @..\tools\rm stasm.log test_stasm_lib_auto.bmp

test_stasm_lib_multi0_minwidth10:
        test_stasm_lib 0 10 1 ../tests/data/testmulti.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_lib/multi0_minwidth10/stasm.log
        @..\tools\rm stasm.log test_stasm_lib_auto.bmp

test_stasm_lib_multi1_minwidth10:
        test_stasm_lib 1 10 1 ../tests/data/testmulti.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_lib/multi1_minwidth10/stasm.log
        @..\tools\rm stasm.log test_stasm_lib_auto.bmp

test_stasm_lib_err:
!IF  "$(CFG)" == "Debug"
	@echo Debug build: Skipping test_stasm_lib_err
!ELSE
        test_stasm_lib_err
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_lib/test_stasm_lib_err.log
        @..\tools\rm stasm.log
!ENDIF

test_stasm_main:
        stasm ../data/testface.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_main/single/test_stasm_main.log
        ..\tools\cmp testface_stasm.bmp ../tests/$(MOD_DIR)/stasm_main/single/testface_stasm.bmp
        @..\tools\rm stasm.log testface_stasm.bmp
        stasm -m ../tests/data/testmulti.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_main/multi/test_stasm_main_multi.log
        @..\tools\rm stasm.log testmulti_stasm.bmp
        stasm -d -m -s -c ../tests/data/testmulti.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_main/multi/test_stasm_main_multi_small.log
        ..\tools\cmp testmulti_stasm.bmp ../tests/$(MOD_DIR)/stasm_main/multi/testmulti_stasm.bmp
        @..\tools\rm stasm.log testmulti_stasm.bmp
        stasm -i -n 68 ../data/testface.jpg
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/stasm_main/multi/test_stasm_main_68.log
        @..\tools\rm stasm.log

test_swas:
        swas -f ../tests/data/testface.shape
        ..\tools\mdiff stasm.log ../tests/$(MOD_DIR)/swas/swas.log
!IF "$(MOD)" == "MOD_1"
        ..\tools\mdiff fit/testface.fit ../tests/$(MOD_DIR)/swas/testface.fit
        @..\tools\rm fit/testface.fit
!ELSE
        ..\tools\mdiff fit/testface_$(MOD).fit ../tests/$(MOD_DIR)/swas/testface_$(MOD).fit
        @..\tools\rm fit/testface_$(MOD).fit
!ENDIF
        ..\tools\cmp testface__ref.bmp ../tests/$(MOD_DIR)/swas/testface__ref.bmp
        ..\tools\cmp testface_stasm.bmp ../tests/$(MOD_DIR)/swas/testface_stasm.bmp
        @..\tools\rm stasm.log testface_stasm.bmp testface__ref.bmp

# test_tasm requires that the tasm code is installed i.e. that the tasm directory is populated
test_tasm:
    @..\tools\rm -rf test_tasmout
    tasm -d test_tasmout -w ../tasm/shapes/muct77_short.shape 0 0 "i000q|i003s|i017q"
    ..\tools\mdiff test_tasmout/log/tasm.log ../tests/tasm_small77/tasm.log
    ..\tools\mdiff test_tasmout/log/imputed.shape ../tests/tasm_small77/imputed.shape
    ..\tools\mdiff test_tasmout/mh/yaw00.mh ../tests/tasm_small77/yaw00.mh
    ..\tools\mdiff test_tasmout/mh/yaw00_shapemodel.mh ../tests/tasm_small77/yaw00_shapemodel.mh
    ..\tools\mdiff test_tasmout/mh/yaw00_lev0_p12_classic.mh ../tests/tasm_small77/yaw00_lev0_p12_classic.mh
    ..\tools\mdiff test_tasmout/mh/yaw00_lev1_p06_classic.mh ../tests/tasm_small77/yaw00_lev1_p06_classic.mh
    ..\tools\mdiff test_tasmout/mh/yaw00_lev2_p00_classic.mh ../tests/tasm_small77/yaw00_lev2_p00_classic.mh
    ..\tools\mdiff test_tasmout/mh/yaw00_lev3_p00_classic.mh ../tests/tasm_small77/yaw00_lev3_p00_classic.mh
    ..\tools\mdiff test_tasmout/mh/yaw00_lev0_p16_hat.mh ../tests/tasm_small77/yaw00_lev0_p16_hat.mh
    ..\tools\mdiff test_tasmout/log/lev3_p76_classic.desc ../tests/tasm_small77/lev3_p76_classic.desc
    ..\tools\mdiff test_tasmout/log/lev2_p76_hat.desc ../tests/tasm_small77/lev2_p76_hat.desc
    @rem p26 is imputed, so test it below
    ..\tools\mdiff test_tasmout/mh/yaw00_lev1_p26_hat.mh ../tests/tasm_small77/yaw00_lev1_p26_hat.mh
    @rem p30 is sometimes obscured if eye is closed, so test it below
    ..\tools\mdiff test_tasmout/mh/yaw00_lev2_p30_hat.mh ../tests/tasm_small77/yaw00_lev2_p30_hat.mh
    ..\tools\cmp test_tasmout/log/landmark00.bmp ../tests/tasm_small77/landmark00.bmp
    ..\tools\cmp test_tasmout/log/landmark30.bmp ../tests/tasm_small77/landmark30.bmp
    ..\tools\cmp test_tasmout/log/meanshape.bmp ../tests/tasm_small77/meanshape.bmp
    ..\tools\cmp test_tasmout/log/shape17.bmp ../tests/tasm_small77/shape17.bmp
    @..\tools\rm -r test_tasmout

test_shapeconvert:
        shapeconvert -n 17 ../tests/data/testface.shape
        ..\tools\mdiff testface_17.shape ../tests/shapeconvert/testface_17.shape
        @..\tools\rm testface_17.shape 

test_shapemirror:
        shapemirror ../tests/data/testface.shape
        ..\tools\mdiff testfacer.shape ../tests/shapemirror/testfacer.shape
        ..\tools\cmp new/testfacer.jpg ../tests/shapemirror/testfacer.jpg
        @..\tools\rm testfacer.shape new/testfacer.jpg

test_shapetostasm31:
        shapetostasm31 ../tests/data/testface.shape
        ..\tools\mdiff testface_stasm31.shape ../tests/shapetostasm31/testface_stasm31.shape
        @..\tools\rm testface_stasm31.shape 

test_mdiff:
	@rem the - prefix means ignore correct "error" returns for non matches
	.\mdiff ../apps/mdiff/tests/test1 ../apps/mdiff/tests/test1 >mdiff.log
	@rem following test tests that we can use a directory as second argument
	-.\mdiff ../apps/mdiff/tests/test1 ../apps/mdiff/tests       >>mdiff.log
	-.\mdiff ../apps/mdiff/tests/test1 ../apps/mdiff/tests/test2 >>mdiff.log
	-.\mdiff ../apps/mdiff/tests/test1 ../apps/mdiff/tests/test3 >>mdiff.log
	-.\mdiff ../apps/mdiff/tests/test1 ../apps/mdiff/tests/test4 >>mdiff.log
	-.\mdiff ../apps/mdiff/tests/test1 ../apps/mdiff/tests/test5 >>mdiff.log
	-.\mdiff ../apps/mdiff/tests/test6 ../apps/mdiff/tests/test6extra >>mdiff.log
	-.\mdiff ../apps/mdiff/tests/test7 ../apps/mdiff/tests/test7extra >>mdiff.log
	diff mdiff.log ../apps/mdiff/tests/mdiff.log
	@..\tools\rm mdiff.log

test:\
 test_stasm_lib_multi0_minwidth25\
 test_stasm_lib_multi1_minwidth25\
 test_stasm_lib_multi0_minwidth10\
 test_stasm_lib_multi1_minwidth10\
 test_stasm_main\
 test_swas\
 test_shapeconvert\
 test_shapetostasm31\
 test_stasm_lib_err\
 test_tasm\
 test_mdiff

fulltest:\
 test_minimal\
 test_minimal2\
 test

OBJ=\
 $(EXTRA_MOD_OBJ)\
 $(OUTDIR)/facedet.obj\
 $(OUTDIR)/initasm.obj\
 $(OUTDIR)/asm.obj\
 $(OUTDIR)/classicdesc.obj\
 $(OUTDIR)/convshape.obj\
 $(OUTDIR)/err.obj\
 $(OUTDIR)/eyedet.obj\
 $(OUTDIR)/eyedist.obj\
 $(OUTDIR)/faceroi.obj\
 $(OUTDIR)/hat.obj\
 $(OUTDIR)/hatdesc.obj\
 $(OUTDIR)/landmarks.obj\
 $(OUTDIR)/misc.obj\
 $(OUTDIR)/pinstart.obj\
 $(OUTDIR)/print.obj\
 $(OUTDIR)/shape17.obj\
 $(OUTDIR)/shapehacks.obj\
 $(OUTDIR)/shapemod.obj\
 $(OUTDIR)/startshape.obj\
 $(OUTDIR)/stasm.obj

STASM_LIB_OBJ=\
 $(OBJ)\
 $(OUTDIR)/stasm_lib.obj

WASM_OBJ=\
 $(OUTDIR)/wasm.res\
 $(OUTDIR)/appmisc.obj\
 $(OUTDIR)/findfile.obj\
 $(OUTDIR)/usermsg.obj\
 $(OUTDIR)/wasm.obj\
 $(OUTDIR)/writewind.obj

# Build the static library

$(OUTDIR)/stasm_lib$(LIBSUFFIX).lib: $(OUTDIR)/stasm_lib.obj ../stasm/stasm_lib.h $(STASM_LIB_OBJ)
        lib -nologo -out:$(OUTDIR)/stasm_lib$(LIBSUFFIX).lib  $(STASM_LIB_OBJ)

# executables

minimal.exe: $(OUTDIR)/minimal.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h
        link -out:minimal.exe $(LFLAGS)\
 $(OUTDIR)/minimal.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(LIBS)

minimal2.exe: $(OUTDIR)/minimal2.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h
        link -out:minimal2.exe $(LFLAGS)\
 $(OUTDIR)/minimal2.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(LIBS)

stasm.exe: $(OUTDIR)/stasm.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h\
 $(OUTDIR)/stasm_main.obj\
 $(OUTDIR)/appmisc.obj ../apps/appmisc.h
        link -out:stasm.exe $(LFLAGS)\
 $(OUTDIR)/stasm_main.obj $(OUTDIR)/appmisc.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(SETARGV) $(LIBS)

shapeconvert.exe: $(OUTDIR)/stasm.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h\
 $(OUTDIR)/shapeconvert.obj\
 $(OUTDIR)/appmisc.obj ../apps/appmisc.h\
 $(OUTDIR)/shapefile.obj ../apps/shapefile/shapefile.h\
 $(OUTDIR)/stasm_regex.obj ../apps/shapefile/stasm_regex.h
        link -out:shapeconvert.exe $(LFLAGS)\
 $(OUTDIR)/shapeconvert.obj $(OUTDIR)/appmisc.obj \
 $(OUTDIR)/shapefile.obj\
 $(OUTDIR)/stasm_regex.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(SETARGV) $(LIBS)

shapefromstasm31.exe: $(OUTDIR)/stasm.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h\
 $(OUTDIR)/shapefromstasm31.obj\
 $(OUTDIR)/appmisc.obj ../apps/appmisc.h\
 $(OUTDIR)/shapefile.obj ../apps/shapefile/shapefile.h\
 $(OUTDIR)/stasm_regex.obj ../apps/shapefile/stasm_regex.h
        link -out:shapefromstasm31.exe $(LFLAGS)\
 $(OUTDIR)/shapefromstasm31.obj $(OUTDIR)/appmisc.obj \
 $(OUTDIR)/shapefile.obj\
 $(OUTDIR)/stasm_regex.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(SETARGV) $(LIBS)

shapemirror.exe: $(OUTDIR)/stasm.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h\
 $(OUTDIR)/shapemirror.obj\
 $(OUTDIR)/appmisc.obj ../apps/appmisc.h\
 $(OUTDIR)/shapefile.obj ../apps/shapefile/shapefile.h\
 $(OUTDIR)/stasm_regex.obj ../apps/shapefile/stasm_regex.h
        link -out:shapemirror.exe $(LFLAGS)\
 $(OUTDIR)/shapemirror.obj $(OUTDIR)/appmisc.obj \
 $(OUTDIR)/shapefile.obj\
 $(OUTDIR)/stasm_regex.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(SETARGV) $(LIBS)

shapetostasm31.exe: $(OUTDIR)/stasm.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h\
 $(OUTDIR)/shapetostasm31.obj\
 $(OUTDIR)/appmisc.obj ../apps/appmisc.h\
 $(OUTDIR)/shapefile.obj ../apps/shapefile/shapefile.h\
 $(OUTDIR)/stasm_regex.obj ../apps/shapefile/stasm_regex.h
        link -out:shapetostasm31.exe $(LFLAGS)\
 $(OUTDIR)/shapetostasm31.obj $(OUTDIR)/appmisc.obj \
 $(OUTDIR)/shapefile.obj\
 $(OUTDIR)/stasm_regex.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(SETARGV) $(LIBS)

swas.exe: $(OUTDIR)/stasm.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h\
 $(OUTDIR)/swas.obj\
 $(OUTDIR)/appmisc.obj ../apps/appmisc.h\
 $(OUTDIR)/fitmeas.obj ../apps/swas/fitmeas.h\
 $(OUTDIR)/fm29.obj ../apps/swas/fm29.h\
 $(OUTDIR)/shapefile.obj ../apps/shapefile/shapefile.h\
 $(OUTDIR)/stasm_regex.obj ../apps/shapefile/stasm_regex.h
        link -out:swas.exe $(LFLAGS)\
 $(OUTDIR)/swas.obj $(OUTDIR)/appmisc.obj $(OUTDIR)/fitmeas.obj $(OUTDIR)/fm29.obj\
 $(OUTDIR)/shapefile.obj\
 $(OUTDIR)/stasm_regex.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(SETARGV) $(LIBS)

# psapi.lib is needed for GetProcessMemoryInfo under Windows

tasm.exe: $(OUTDIR)/stasm.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/tasmalignshapes.obj\
 $(OUTDIR)/tasmclassic.obj\
 $(OUTDIR)/tasmconf.obj\
 $(OUTDIR)/tasmdescmod.obj\
 $(OUTDIR)/tasmdraw.obj\
 $(OUTDIR)/tasmhat.obj\
 $(OUTDIR)/tasmimpute.obj\
 $(OUTDIR)/tasmlandtab.obj\
 $(OUTDIR)/tasmmeanshape.obj\
 $(OUTDIR)/tasmshapemod.obj\
 $(OUTDIR)/shapefile.obj\
 $(OUTDIR)/stasm_regex.obj\
 $(OUTDIR)/appmem.obj\
 $(OUTDIR)/appmisc.obj\
 $(OUTDIR)/linsolve.obj
        link -out:tasm.exe $(LFLAGS)\
 psapi.lib\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/tasmalignshapes.obj\
 $(OUTDIR)/tasmclassic.obj\
 $(OUTDIR)/tasmconf.obj\
 $(OUTDIR)/tasmdescmod.obj\
 $(OUTDIR)/tasmdraw.obj\
 $(OUTDIR)/tasmhat.obj\
 $(OUTDIR)/tasmimpute.obj\
 $(OUTDIR)/tasmlandtab.obj\
 $(OUTDIR)/tasmmeanshape.obj\
 $(OUTDIR)/tasmshapemod.obj\
 $(OUTDIR)/shapefile.obj\
 $(OUTDIR)/stasm_regex.obj\
 $(OUTDIR)/appmem.obj\
 $(OUTDIR)/appmisc.obj\
 $(OUTDIR)/linsolve.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(SETARGV) $(LIBS)

test_stasm_lib.exe: $(OUTDIR)/test_stasm_lib.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h ../stasm/stasm_lib_ext.h
        link -out:test_stasm_lib.exe $(LFLAGS)\
 $(OUTDIR)/test_stasm_lib.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(LIBS)

test_stasm_lib_err.exe: $(OUTDIR)/test_stasm_lib_err.obj $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib ../stasm/stasm_lib.h
        link -out:test_stasm_lib_err.exe $(LFLAGS)\
 $(OUTDIR)/test_stasm_lib_err.obj\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(LIBS)

wasm.exe: $(WASM_OBJ) $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(LIBS)
	link -out:wasm.exe -subsystem:windows $(LFLAGS)\
 $(WASM_OBJ)\
 $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib\
 $(LIBS) $(WINLIBS)

$(OUTDIR)/wasm.res: ../apps/win/wasm.rc ../apps/win/wasm.h ../apps/win/wasm.bmp ../apps/win/wasm.ico
	rc -r -nologo ../apps/win/wasm.rc
	..\tools\mv ../apps/win/wasm.res $(OUTDIR)/wasm.res

mdiff.exe: ../apps/mdiff/mdiff.cpp
	cl -O2 -W3 -nologo ../apps/mdiff/mdiff.cpp $(SETARGV) 
	..\tools\rm mdiff.obj

../stasm/stasm.h:\
 $(MOD_DEPENDENCIES)\
 ../stasm/misc.h\
 ../stasm/print.h\
 ../stasm/err.h\
 ../stasm/stasm_landmarks.h\
 ../stasm/stasm_lib.h\
 ../stasm/stasm_lib_ext.h\
 ../stasm/atface.h\
 ../stasm/landmarks.h\
 ../stasm/landtab_muct77.h\
 ../stasm/eyedet.h\
 ../stasm/basedesc.h\
 ../stasm/classicdesc.h\
 ../stasm/hat.h\
 ../stasm/hatdesc.h\
 ../stasm/shapemod.h\
 ../stasm/asm.h\
 ../stasm/convshape.h\
 ../stasm/eyedist.h\
 ../stasm/faceroi.h\
 ../stasm/pinstart.h\
 ../stasm/shape17.h\
 ../stasm/shapehacks.h\
 ../stasm/startshape.h

# Object files.  

# We precompile stasm.obj for speed in later compiles.
# All other obj files have a dependency on stasm.obj or stasm_lib
# i.e. if stasm.obj is rebuilt, all other files will also be rebuilt.

$(OUTDIR)/stasm.obj: ../stasm/stasm.cpp ../stasm/stasm.h make.bat
        @echo Precompiling stasm.cpp to generate stasm.pch
        cl -c ../stasm/stasm.cpp $(CFLAGS_PRECOMPILE)

# stasm_lib.obj is special because is is built with -DBUILD_AS_LIB

$(OUTDIR)/stasm_lib.obj: ../stasm/stasm_lib.cpp ../stasm/stasm_lib.h $(OUTDIR)/stasm.obj
        cl -c ../stasm/stasm_lib.cpp $(CFLAGS) -DBUILD_AS_LIB

# files that have extra dependencies

$(OUTDIR)/minimal.obj: ../apps/minimal.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib
        cl -c ../apps/minimal.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/minimal2.obj: ../apps/minimal2.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib
        cl -c ../apps/minimal2.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/stasm_main.obj: ../apps/stasm_main.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj\
 ../apps/appmisc.h
        cl -c ../apps/stasm_main.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/shapeconvert.obj: ../apps/shapefile/shapeconvert.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj\
 ../apps/appmisc.h\
 ../apps/shapefile/shapefile.h
        cl -c ../apps/shapefile/shapeconvert.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/shapefromstasm31.obj: ../apps/shapefile/shapefromstasm31.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj\
 ../apps/appmisc.h\
 ../apps/shapefile/shapefile.h\
 ../apps/shapefile/stasm_regex.h
        cl -c ../apps/shapefile/shapefromstasm31.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/shapemirror.obj: ../apps/shapefile/shapemirror.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj\
 ../apps/appmisc.h\
 ../apps/shapefile/shapefile.h\
 ../apps/shapefile/stasm_regex.h
        cl -c ../apps/shapefile/shapemirror.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/shapetostasm31.obj: ../apps/shapefile/shapetostasm31.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj\
 ../apps/appmisc.h\
 ../apps/shapefile/shapefile.h\
 ../apps/shapefile/stasm_regex.h
        cl -c ../apps/shapefile/shapetostasm31.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/swas.obj: ../apps/swas/swas.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj\
 ../apps/appmisc.h\
 ../apps/shapefile/shapefile.h\
 ../apps/swas/fitmeas.h\
 ../apps/swas/fm29.h\
 ../apps/shapefile/stasm_regex.h
        cl -c ../apps/swas/swas.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/test_stasm_lib.obj: ../apps/test_stasm_lib.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj
        cl -c ../apps/test_stasm_lib.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/test_stasm_lib_err.obj: ../apps/test_stasm_lib_err.cpp $(OUTDIR)/stasm_lib$(LIBSUFFIX).lib $(OUTDIR)/stasm.obj
        cl -c ../apps/test_stasm_lib_err.cpp $(CFLAGS_NOPRECOMPILE)

$(OUTDIR)/shapefile.obj: ../apps/shapefile/shapefile.cpp ../apps/shapefile/shapefile.h $(OUTDIR)/stasm.obj
        cl -c ../apps/shapefile/shapefile.cpp $(CFLAGS)

$(OUTDIR)/stasm_regex.obj: ../apps/shapefile/stasm_regex.cpp ../apps/shapefile/stasm_regex.h $(OUTDIR)/stasm.obj\
 ../apps/shapefile/stasm_regex.h
        cl -c ../apps/shapefile/stasm_regex.cpp $(CFLAGS)

$(OUTDIR)/wasm.obj: ../apps/win/wasm.cpp $(OUTDIR)/stasm.obj ../apps/win/wasm.h $(OUTDIR)/stasm.obj\
 ../apps/win/findfile.h\
 ../apps/win/usermsg.h\
 ../apps/win/writewind.h
	cl -c ../apps/win/wasm.cpp $(CFLAGS)

$(OUTDIR)/usermsg.obj: ../apps/win/usermsg.cpp ../apps/win/usermsg.h $(OUTDIR)/stasm.obj\
 ../apps/win/usermsg.h
	cl -c ../apps/win/usermsg.cpp $(CFLAGS)

$(OUTDIR)/writewind.obj: ../apps/win/writewind.cpp ../apps/win/writewind.h $(OUTDIR)/stasm.obj\
 ../apps/win/usermsg.h\
 ../apps/win/writewind.h
	cl -c ../apps/win/writewind.cpp $(CFLAGS)

# MOD dependent stuff

$(OUTDIR)/facedet.obj: ../stasm/$(MOD_DIR)/facedet.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
        cl -c ../stasm/$(MOD_DIR)/facedet.cpp $(CFLAGS)

$(OUTDIR)/initasm.obj: ../stasm/$(MOD_DIR)/initasm.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
	cl -c ../stasm/$(MOD_DIR)/initasm.cpp $(CFLAGS)

$(OUTDIR)/initasm22.obj: ../stasm/$(MOD_DIR)/initasm22.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
	cl -c ../stasm/$(MOD_DIR)/initasm22.cpp $(CFLAGS)

$(OUTDIR)/initasm45.obj: ../stasm/$(MOD_DIR)/initasm45.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
	cl -c ../stasm/$(MOD_DIR)/initasm45.cpp $(CFLAGS)

$(OUTDIR)/estpose.obj: ../stasm/$(MOD_DIR)/estpose.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
        cl -c ../stasm/$(MOD_DIR)/estpose.cpp $(CFLAGS)

$(OUTDIR)/stasm_svm.obj: ../stasm/$(MOD_DIR)/stasm_svm.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
        cl -c ../stasm/$(MOD_DIR)/stasm_svm.cpp $(CFLAGS)

$(OUTDIR)/MVFDCache.obj: ../stasm/$(MOD_DIR)/MVFDEmulator/MVFDCache.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
        cl -c ../stasm/$(MOD_DIR)/MVFDEmulator/MVFDCache.cpp $(CFLAGS)

$(OUTDIR)/MVFDCacheReader.obj: ../stasm/$(MOD_DIR)/MVFDEmulator/MVFDCacheReader.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
        cl -c ../stasm/$(MOD_DIR)/MVFDEmulator/MVFDCacheReader.cpp $(CFLAGS)

$(OUTDIR)/MVFDEmulator.obj: ../stasm/$(MOD_DIR)/MVFDEmulator/MVFDEmulator.cpp $(OUTDIR)/stasm.obj $(MOD_DEPENDENCIES)
        cl -c ../stasm/$(MOD_DIR)/MVFDEmulator/MVFDEmulator.cpp $(CFLAGS)

# generic files

$(OUTDIR)/linsolve.obj: ../apps/linsolve.cpp ../apps/linsolve.h $(OUTDIR)/stasm.obj
	cl -c ../apps/linsolve.cpp $(CFLAGS)

$(OUTDIR)/appmem.obj: ../apps/appmem.cpp ../apps/appmem.h $(OUTDIR)/stasm.obj
	cl -c ../apps/appmem.cpp $(CFLAGS)

$(OUTDIR)/appmisc.obj: ../apps/appmisc.cpp ../apps/appmisc.h $(OUTDIR)/stasm.obj
	cl -c ../apps/appmisc.cpp $(CFLAGS)

$(OUTDIR)/asm.obj: ../stasm/asm.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/asm.cpp $(CFLAGS)

$(OUTDIR)/classicdesc.obj: ../stasm/classicdesc.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/classicdesc.cpp $(CFLAGS)

$(OUTDIR)/convshape.obj: ../stasm/convshape.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/convshape.cpp $(CFLAGS)

$(OUTDIR)/err.obj: ../stasm/err.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/err.cpp $(CFLAGS)

$(OUTDIR)/eyedet.obj: ../stasm/eyedet.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/eyedet.cpp $(CFLAGS)

$(OUTDIR)/eyedist.obj: ../stasm/eyedist.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/eyedist.cpp $(CFLAGS)

$(OUTDIR)/faceroi.obj: ../stasm/faceroi.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/faceroi.cpp $(CFLAGS)

$(OUTDIR)/fitmeas.obj: ../apps/swas/fitmeas.cpp $(OUTDIR)/stasm.obj
	cl -c ../apps/swas/fitmeas.cpp $(CFLAGS)

$(OUTDIR)/findfile.obj: ../apps/win/findfile.cpp ../apps/win/findfile.h $(OUTDIR)/stasm.obj
	cl -c ../apps/win/findfile.cpp $(CFLAGS)

$(OUTDIR)/fm29.obj: ../apps/swas/fm29.cpp ../apps/swas/fm29.h $(OUTDIR)/stasm.obj
	cl -c ../apps/swas/fm29.cpp $(CFLAGS)

$(OUTDIR)/hat.obj: ../stasm/hat.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/hat.cpp $(CFLAGS)

$(OUTDIR)/hatdesc.obj: ../stasm/hatdesc.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/hatdesc.cpp $(CFLAGS)

$(OUTDIR)/landmarks.obj: ../stasm/landmarks.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/landmarks.cpp $(CFLAGS)

$(OUTDIR)/misc.obj: ../stasm/misc.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/misc.cpp $(CFLAGS)

$(OUTDIR)/pinstart.obj: ../stasm/pinstart.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/pinstart.cpp $(CFLAGS)

$(OUTDIR)/print.obj: ../stasm/print.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/print.cpp $(CFLAGS)

$(OUTDIR)/shape17.obj: ../stasm/shape17.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/shape17.cpp $(CFLAGS)

$(OUTDIR)/shapehacks.obj: ../stasm/shapehacks.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/shapehacks.cpp $(CFLAGS)

$(OUTDIR)/shapemod.obj: ../stasm/shapemod.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/shapemod.cpp $(CFLAGS)

$(OUTDIR)/startshape.obj: ../stasm/startshape.cpp $(OUTDIR)/stasm.obj
	cl -c ../stasm/startshape.cpp $(CFLAGS)

# additional dependencies here are for tasm.h, and we make the other obj depdend
# on tasm.h, hence for all dependencies in tasm.h
$(OUTDIR)/tasm.obj:\
 ../tasm/src/tasm.cpp\
 ../tasm/src/tasm.h\
 ../apps/appmisc.h\
 ../apps/appmem.h\
 ../apps/linsolve.h\
 ../apps/shapefile/shapefile.h\
 ../apps/shapefile/stasm_regex.h\
 ../tasm/src/tasmconf.h\
 ../tasm/src/tasmdescmod.h\
 ../tasm/src/tasmdraw.h\
 ../tasm/src/tasmhat.h\
 ../tasm/src/tasmclassic.h\
 ../tasm/src/tasmlandtab.h\
 ../tasm/src/tasmmeanshape.h\
 ../tasm/src/tasmshapemod.h\
 ../tasm/src/tasmalignshapes.h\
 ../tasm/src/tasmimpute.h\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasm.cpp $(CFLAGS)

$(OUTDIR)/tasmalignshapes.obj:\
 ../tasm/src/tasmalignshapes.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmalignshapes.cpp $(CFLAGS)

$(OUTDIR)/tasmclassic.obj:\
 ../tasm/src/tasmclassic.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmclassic.cpp $(CFLAGS)

$(OUTDIR)/tasmconf.obj:\
 ../tasm/src/tasmconf.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmconf.cpp $(CFLAGS)

$(OUTDIR)/tasmdescmod.obj:\
 ../tasm/src/tasmdescmod.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmdescmod.cpp $(CFLAGS)

$(OUTDIR)/tasmdraw.obj:\
 ../tasm/src/tasmdraw.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmdraw.cpp $(CFLAGS)

$(OUTDIR)/tasmhat.obj:\
 ../tasm/src/tasmhat.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmhat.cpp $(CFLAGS)

$(OUTDIR)/tasmimpute.obj:\
 ../tasm/src/tasmimpute.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmimpute.cpp $(CFLAGS)

$(OUTDIR)/tasmlandtab.obj:\
 ../tasm/src/tasmlandtab.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmlandtab.cpp $(CFLAGS)

$(OUTDIR)/tasmmeanshape.obj:\
 ../tasm/src/tasmmeanshape.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmmeanshape.cpp $(CFLAGS)

$(OUTDIR)/tasmshapemod.obj:\
 ../tasm/src/tasmshapemod.cpp\
 ../tasm/src/tasm.h\
 $(OUTDIR)/tasm.obj\
 $(OUTDIR)/stasm.obj
	cl -c ../tasm/src/tasmshapemod.cpp $(CFLAGS)

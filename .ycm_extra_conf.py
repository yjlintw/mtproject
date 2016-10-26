import os
import ycm_core
from clang_helpers import PrepareClangFlags
 
# Set this to the absolute path to the folder (NOT the file!) containing the
# compile_commands.json file to use that instead of 'flags'. See here for
# more details: http://clang.llvm.org/docs/JSONCompilationDatabase.html
# Most projects will NOT need to set this to anything; you can just change the
# 'flags' list of compilation flags. Notice that YCM itself uses that approach.
compilation_database_folder = ''
 
# These are the compilation flags that will be used in case there's no
# compilation database set.
flags = [
    # '-c',
    # '-O3'
    # '-Wall',
    '-std=c++11',
    '-DNDEBUG',
    '-D__MACOSX_CORE__',
    # '-stdlib=libstdc++',
    # '-fexceptions',
    # '-march=native',
    # '-mtune=native',
    # '-finline-functions',
    # '-arch i386',
    # '-fpascal-strings',
    # '-mmacosx-version-min=10.9',
    # '-fasm-blocks',
    # '-funroll-loops',
    # '-mssse3', 
    # '-fmessage-length=0', 
    # '-D__MACOSX_CORE__',
    '-x',
    'c++',
    # '-c',
    # '-0s',
    '-I',
    '.',
    # '-isystem',
    # '/usr/include',
    '-isystem',
    '/usr/local/include',
    # '-isystem',
    #'/Library/Developer/CommandLineTools/usr/bin/../lib/c++/v1',
    # '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../include/c++/v1',
    # '-isystem',
    # '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/../lib/clang/6.0/include',
    # '-isystem',
    # '/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include',
    '-I./src' 
    '-I../../../libs/FreeImage/include', 
    '-I../../../libs/cairo/include',
    '-I../../../libs/cairo/include/cairo',
    '-I../../../libs/cairo/include/libpng15',
    '-I../../../libs/cairo/include/pixman-1',
    '-I../../../libs/fmodex/include',
    '-I../../../libs/freetype/include',
    '-I../../../libs/freetype/include/freetype2',
    '-I../../../libs/freetype/include/freetype2/freetype',
    '-I../../../libs/freetype/include/freetype2/freetype/config',
    '-I../../../libs/freetype/include/freetype2/freetype/internal',
    '-I../../../libs/freetype/include/freetype2/freetype/internal/services',
    '-I../../../libs/glm/include',
    '-I../../../libs/glew/include', 
    '-I../../../libs/glew/include/GL',
    '-I../../../libs/glfw/include',
    '-I../../../libs/glfw/include/GLFW',
    '-I../../../libs/glu/include',
    '-I../../../libs/glut/include',
    '-I../../../libs/json/include',
    '-I../../../libs/kiss/include',
    '-I../../../libs/openssl/include',
    '-I../../../libs/openssl/include/openssl',
    '-I../../../libs/poco/include',
    '-I../../../libs/rtAudio/include',
    '-I../../../libs/tess2/include',
    '-I../../../libs/openFrameworks',
    '-I../../../libs/openFrameworks/3d',
    '-I../../../libs/openFrameworks/app',
    '-I../../../libs/openFrameworks/communication',
    '-I../../../libs/openFrameworks/events',
    '-I../../../libs/openFrameworks/gl',
    '-I../../../libs/openFrameworks/graphics',
    '-I../../../libs/openFrameworks/math',
    '-I../../../libs/openFrameworks/sound',
    '-I../../../libs/openFrameworks/types',
    '-I../../../libs/openFrameworks/utils',
    '-I../../../libs/openFrameworks/video',
    '-I../../../libs/utf8cpp/include',
    '-I../../../libs/utf8cpp/include/utf8',
    # '-x objective-c++',
    # '-MMD',
    # '-MP' 
    # '-MF',
    # 'obj/osx/Release/src/ofApp.d',
    ]

addons = [
	'-I../../../addons/ofxOpenCv/src',
	'-I../../../addons/ofxOpenCv/libs',
	'-I../../../addons/ofxOpenCv/libs/opencv',
	'-I../../../addons/ofxOpenCv/libs/opencv/include',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/features2d',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/core',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/gpu',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/ts',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/contrib',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/flann',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/objdetect',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/highgui',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/legacy',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/imgproc',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/video',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/ml',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/calib3d',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/osx',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/win_cb',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/ios',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/linux',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/vs',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/linuxarmv6l',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/android',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/android/armeabi',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/android/armeabi-v7a',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/linuxarmv7l',
	'-I../../../addons/ofxOpenCv/libs/opencv/lib/linux64',
	'-I../../../addons/ofxOpenCv/libs/opencv/include',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/features2d',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/core',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/gpu',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/ts',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/contrib',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/flann',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/objdetect',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/highgui',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/legacy',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/imgproc',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/video',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/ml',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv2/calib3d',
	'-I../../../addons/ofxOpenCv/libs/opencv/include/opencv',
	'-I../../../addons/ofxCv/src',
	'-I../../../addons/ofxCv/libs',
	'-I../../../addons/ofxCv/libs/CLD',
	'-I../../../addons/ofxCv/libs/CLD/include',
	'-I../../../addons/ofxCv/libs/CLD/include/CLD',
	'-I../../../addons/ofxCv/libs/CLD/src',
	'-I../../../addons/ofxCv/libs/ofxCv',
	'-I../../../addons/ofxCv/libs/ofxCv/include',
	'-I../../../addons/ofxCv/libs/ofxCv/include/ofxCv',
	'-I../../../addons/ofxCv/libs/ofxCv/src',
	'-I../../../addons/ofxCv/libs/CLD/include',
	'-I../../../addons/ofxCv/libs/CLD/include/CLD',
	'-I../../../addons/ofxCv/libs/ofxCv/include',
	'-I../../../addons/ofxCv/libs/ofxCv/include/ofxCv',
	'-I../../../addons/ofxFaceTracker/src',
	'-I../../../addons/ofxFaceTracker/libs',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker/include',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker/include/FaceTracker',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker/src',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker/src/lib',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker/model',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker/include',
	'-I../../../addons/ofxFaceTracker/libs/FaceTracker/include/FaceTracker',
        '-I../../../addons/ofxKinect/src',
        '-I../../../addons/ofxKinect/src/extra',
        '-I../../../addons/ofxKinect/libs/libusb-1.0/include/libusb-1.0',
        '-I../../../addons/ofxKinect/libs/libfreenect/include',
        '-I../../../addons/ofxNetwork/src',
        '-I../../../addons/ofxBlobTracker/src',
        '-I../../../addons/ofxFiducialFinder/src',
        '-I../../../addons/ofxFiducialFinder/src/libfidtrack',
	'-pthread',
	'-D_REENTRANT'
]

flags += addons


# flagsRec=['/Users/peacedove/Documents/Code/OF/of_v0.8.3_osx_release/libs/','/Users/peacedove/Documents/Code/OF/of_v0.8.3_osx_release/addons/']

# def AddDirsRecursively( flagsRec ):
#     global flags
#     new_flags = []
#     for flag in flagsRec:
#         for root, dirs, files in os.walk(flag, topdown=True):
#             for name in dirs:
#                 new_flags.append('-I')
#                 new_flags.append(os.path.join(root,name))
#     flags += new_flags
# AddDirsRecursively( flagsRec )

if compilation_database_folder:
    database = ycm_core.CompilationDatabase(compilation_database_folder)
else:
    database = None
 
 
def DirectoryOfThisScript():
    return os.path.dirname(os.path.abspath(__file__))
 
 
def MakeRelativePathsInFlagsAbsolute(flags, working_directory):
    if not working_directory:
        return flags
    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=']
    for flag in flags:
        new_flag = flag
 
        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/'):
                new_flag = os.path.join(working_directory, flag)
 
        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break
 
            if flag.startswith(path_flag):
                path = flag[len(path_flag):]
                new_flag = path_flag + os.path.join(working_directory, path)
                break
 
        if new_flag:
            new_flags.append(new_flag)
    return new_flags
 
 
def FlagsForFile(filename):
    if database:
        # Bear in mind that compilation_info.compiler_flags_ does NOT return a
        # python list, but a "list-like" StringVec object
        compilation_info = database.GetCompilationInfoForFile(filename)
        final_flags = PrepareClangFlags(
            MakeRelativePathsInFlagsAbsolute(
                compilation_info.compiler_flags_,
                compilation_info.compiler_working_dir_),
            filename)
    else:
        relative_to = DirectoryOfThisScript()
        final_flags = MakeRelativePathsInFlagsAbsolute(flags, relative_to)
 
    return {
        'flags': final_flags,
        'do_cache': True}


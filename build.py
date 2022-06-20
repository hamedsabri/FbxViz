#!/usr/bin/env python

"""
    Convenient utility script to help building the project without
    using boilerplate CMake routines.
"""

from distutils.spawn import find_executable

import argparse
import contextlib
import codecs
import datetime
import multiprocessing
import os
import platform
import re
import shlex
import shutil
import subprocess
import sys
import time

############################################################
def Windows():
    return platform.system() == "Windows"

def Linux():
    return platform.system() == "Linux"

def MacOS():
    return platform.system() == "Darwin"

def GetCommandOutput(command):
    """Executes the specified command and returns output or None."""
    try:
        return subprocess.check_output(
            shlex.split(command), stderr=subprocess.STDOUT).strip()
    except subprocess.CalledProcessError:
        pass
    return None

def GetXcodeDeveloperDirectory():
    """Returns the active developer directory as reported by 'xcode-select -p'.
    Returns None if none is set."""
    if not MacOS():
        return None

    return GetCommandOutput("xcode-select -p")

def GetVisualStudioCompilerAndVersion():
    """Returns a tuple containing the path to the Visual Studio compiler
    and a tuple for its version, e.g. (14, 0). If the compiler is not found
    or version number cannot be determined, returns None."""
    if not Windows():
        return None

    msvcCompiler = find_executable('cl')
    if msvcCompiler:
        # VisualStudioVersion environment variable should be set by the
        # Visual Studio Command Prompt.
        match = re.search(
            r"(\d+)\.(\d+)",
            os.environ.get("VisualStudioVersion", ""))
        if match:
            return (msvcCompiler, tuple(int(v) for v in match.groups()))
    return None

def IsVisualStudio2019OrGreater():
    VISUAL_STUDIO_2019_VERSION = (16, 0)
    msvcCompilerAndVersion = GetVisualStudioCompilerAndVersion()
    if msvcCompilerAndVersion:
        _, version = msvcCompilerAndVersion
        return version >= VISUAL_STUDIO_2019_VERSION
    return False

def GetCPUCount():
    try:
        return multiprocessing.cpu_count()
    except NotImplementedError:
        return 1

def Run(context, cmd):
    """Run the specified command in a subprocess."""
    print("INFO:", 'Running "{cmd}"'.format(cmd=cmd))

    with codecs.open(context.logFileLocation, "a", "utf-8") as logfile:
        logfile.write("#####################################################################################" + "\n")
        logfile.write("log date: " + datetime.datetime.now().strftime("%Y-%m-%d %H:%M") + "\n")
        logfile.write("#####################################################################################" + "\n")
        logfile.write("\n")
        logfile.write(cmd)
        logfile.write("\n")

        p = subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        encoding = sys.stdout.encoding or "UTF-8"
        while True:
            l = p.stdout.readline().decode(encoding)
            if l != "":
                # Avoid "UnicodeEncodeError: 'ascii' codec can't encode 
                # character" errors by serializing utf8 byte strings.
                logfile.write(l)
                sys.stdout.write(l)
            elif p.poll() is not None:
                break

    if p.returncode != 0:
        with open(context.logFileLocation, "r") as logfile:
            print(logfile.read())

        raise RuntimeError("Failed to run '{cmd}'\nSee {log} for more details."
                           .format(cmd=cmd, log=os.path.abspath(context.logFileLocation)))

def BuildVariant(context): 
    if context.variant == 'debug':
        return "Debug"
    elif context.variant == 'release':
        return "Release"
    elif context.variant == 'relWithDebInfo':
        return "RelWithDebInfo"
    return "RelWithDebInfo"

def FormatMultiProcs(numJobs, generator):
    tag = "-j"
    if generator:
        if "Visual Studio" in generator:
            tag = "/M:"
        elif "Xcode" in generator:
            tag = "-j "

    return "{tag}{procs}".format(tag=tag, procs=numJobs)

def onerror(func, path, exc_info):
    """
    If the error is due to an access error (read only file)
    add write permission and then retries.
    If the error is for another reason it re-raises the error.
    """
    import stat
    if not os.access(path, os.W_OK):
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise

def StartBuild():
    global start_time
    start_time = time.time()

def StopBuild():
    end_time = time.time()
    elapsed_seconds = end_time - start_time
    hours, remainder = divmod(elapsed_seconds, 3600)
    minutes, seconds = divmod(remainder, 60)
    print("Elapsed time: {:02}:{:02}:{:02}".format(int(hours), int(minutes), int(seconds)))

############################################################
# contextmanager
@contextlib.contextmanager
def CurrentWorkingDirectory(dir):
    """Context manager that sets the current working directory to the given
    directory and resets it to the original directory when closed."""
    curdir = os.getcwd()
    os.chdir(dir)
    try:
        yield
    finally:
        os.chdir(curdir)

############################################################
# CMAKE
def RunCMake(context, extraArgs=None, stages=None):
    """Invoke CMake to configure, build, and install a library whose 
    source code is located in the current working directory."""

    srcDir = os.getcwd()
    instDir = context.instDir
    buildDir = context.buildDir

    if 'clean' in stages and os.path.isdir(buildDir):
        shutil.rmtree(buildDir, onerror=onerror)

    if 'clean' in stages and os.path.isdir(instDir):
        shutil.rmtree(instDir)

    if not os.path.isdir(buildDir):
        os.makedirs(buildDir)

    generator = context.cmakeGenerator

    if generator is None and Windows():
        if IsVisualStudio2019OrGreater():
            generator = "Visual Studio 16 2019"
        else:
            generator = "Visual Studio 15 2017 Win64"

    if generator is not None:
        generator = '-G "{gen}"'.format(gen=generator)

    if IsVisualStudio2019OrGreater():
            generator = generator + " -A x64"
            
    # get build variant 
    variant= BuildVariant(context)

    with CurrentWorkingDirectory(buildDir):
        # recreate build_log.txt everytime the script runs
        if os.path.isfile(context.logFileLocation):
            os.remove(context.logFileLocation)

        if 'configure' in stages:
            Run(context,
                'cmake '
                '-DCMAKE_INSTALL_PREFIX="{instDir}" '
                '-DCMAKE_BUILD_TYPE={variant} '
                '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON '
                '{generator} '
                '{extraArgs} '
                '"{srcDir}"'
                .format(instDir=instDir,
                        variant=variant,
                        srcDir=srcDir,
                        generator=(generator or ""),
                        extraArgs=(" ".join(extraArgs) if extraArgs else "")))
 
        installArg = ""
        if 'install' in stages:
            installArg = "--target install"

        if 'build' in stages or 'install' in stages:
            Run(context, "cmake --build . --config {variant} {installArg} -- {multiproc}"
                .format(variant=variant,
                        installArg=installArg,
                        multiproc=FormatMultiProcs(context.numJobs, generator)))

def BuildAndInstall(context, buildArgs, stages):
    with CurrentWorkingDirectory(context.srcDir):
        extraArgs = []
        stagesArgs = []

        if context.fbxLocation:
            extraArgs.append('-DFBX_LOCATION="{fbxLocation}"'
                             .format(fbxLocation=context.fbxLocation))
        extraArgs += buildArgs
        stagesArgs += stages

        RunCMake(context, extraArgs, stagesArgs)

        # Ensure directory structure is created and is writable.
        for dir in [context.workspaceDir, context.buildDir, context.instDir]:
            try:
                if os.path.isdir(dir):
                    testFile = os.path.join(dir, "canwrite")
                    open(testFile, "w").close()
                    os.remove(testFile)
                else:
                    os.makedirs(dir)
            except Exception as e:
                print("Could not write to directory {dir}. Change permissions "
                           "or choose a different location to install to."
                           .format(dir=dir))
                sys.exit(1)
        print("""Success FBXViz build and install !!!!""")


############################################################
# ArgumentParser
parser = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter)

parser.add_argument("--fbx-location", type=str,
                    help="Directory where FBX is located.")

parser.add_argument("workspace_location", type=str,
                    help="Directory where the project use as a workspace to build and install plugin/libraries.")

parser.add_argument("--generator", type=str,
                    help=("CMake generator to use when building libraries with "
                          "cmake"))

parser.add_argument("--build-args", type=str, nargs="*", default=[],
                   help=("Comma-separated list of arguments passed into CMake when building libraries"))

parser.add_argument("--stages", type=str, nargs="*", default=['clean','configure','build','install'],
                   help=("Comma-separated list of stages to execute.(possible stages: clean, configure, build, install, test, package)"))

parser.add_argument("--variant", type=str, default='relWithDebInfo',
                   help=("possible variants: debug, release, relWithDebInfo)"))

parser.add_argument("-j", "--jobs", type=int, default=GetCPUCount(),
                    help=("Number of build jobs to run in parallel. "
                          "(default: # of processors [{0}])"
                          .format(GetCPUCount())))

args = parser.parse_args()

############################################################
# InstallContext
class InstallContext:
    def __init__(self, args):

        # Assume the project's top level cmake is in the current source directory
        self.srcDir = os.path.normpath(
            os.path.join(os.path.abspath(os.path.dirname(__file__))))

        # Workspace directory 
        self.workspaceDir = os.path.abspath(args.workspace_location)

        # CMake generator
        self.cmakeGenerator = args.generator

        # Variant
        self.variant = args.variant

        # Number of jobs
        self.numJobs = args.jobs
        if self.numJobs <= 0:
            raise ValueError("Number of jobs must be greater than 0")

        # Build directory
        self.buildDir = os.path.join(self.workspaceDir, "build", BuildVariant(self))

        # Install directory
        self.instDir = os.path.join(self.workspaceDir, "install", BuildVariant(self))

        # FBX Location
        self.fbxLocation = (os.path.abspath(args.fbx_location)
                           if args.fbx_location else None)

        # Log File Name
        logFileName="build_log.txt"
        self.logFileLocation=os.path.join(self.buildDir, logFileName)

        # Build arguments
        self.buildArgs = list()
        for argList in args.build_args:
            for arg in argList.split(","):
                self.buildArgs.append(arg)

        # Stages arguments
        self.stagesArgs = list()
        for argList in args.stages:
            for arg in argList.split(","):
                self.stagesArgs.append(arg)

try:
    context = InstallContext(args)
except Exception as e:
    print(str(e))
    sys.exit(1)

if __name__ == "__main__":
    # Summarize
    summaryMsg = """
    Building with settings:
      Source directory          {srcDir}
      Workspace directory       {workspaceDir}
      Build directory           {buildDir}
      Install directory         {instDir}
      Variant                   {buildVariant}
      CMake generator           {cmakeGenerator}"""

    summaryMsg += """
    Build Log                 {logFileLocation}"""

    if context.buildArgs:
      summaryMsg += """
      Build arguments           {buildArgs}"""

    if context.stagesArgs:
      summaryMsg += """
      Stages arguments          {stagesArgs}"""

    summaryMsg = summaryMsg.format(
        srcDir=context.srcDir,
        workspaceDir=context.workspaceDir,
        buildDir=context.buildDir,
        instDir=context.instDir,
        logFileLocation=context.logFileLocation,
        buildArgs=context.buildArgs,
        stagesArgs=context.stagesArgs,
        buildVariant=BuildVariant(context),
        cmakeGenerator=("Default" if not context.cmakeGenerator
                        else context.cmakeGenerator)
    )

    print(summaryMsg)

    # BuildAndInstall
    if any(stage in ['clean', 'configure', 'build', 'install'] for stage in context.stagesArgs):
        StartBuild()
        BuildAndInstall(context, context.buildArgs, context.stagesArgs)
        StopBuild()

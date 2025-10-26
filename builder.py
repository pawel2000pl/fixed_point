#!/usr/bin/python3

from ast import arg
from os import mkdir, walk, path
from sys import argv
import json
import zlib
import re
import subprocess

try:
    from termcolor import colored
except:
    colored = lambda t, c: t

BUILD_DIRECTORY = "bin"
BUILD_LIB_DIRECTORY = "bin/lib"
SOURCRE_DIRECTORY = "src"
TEST_FILE_FILTER = ".*([tT][eE][sS][tT]).*"
INCLUDE_DIR_FILE = "include_dir"

CPP_STANDARD = 11

DEBUG_FLAGS = ["-std=c++%d"%CPP_STANDARD, "-Wall", "-Wextra", "-g", "-O1", "-fPIC"]
RELEASE_FLAGS = ["-std=c++%d"%CPP_STANDARD, "-O3", "-Wall", "-Wextra", "-Ofast", "-DNDEBUG", "-fPIC"]
LINKING_PARAMS = ["-std=c++%d"%CPP_STANDARD, "-lm"]

COMMAND_BUILD = "build"
COMMAND_RELEASE = "release"
COMMAND_TEST = "test"

BUILD_INFO = BUILD_DIRECTORY+"/buildInfo.json"

def replacePath(baseFileName: str, includeStatement: str):
    base = path.basename(baseFileName)
    nt = "$nonexistingtoken$"
    fileName = (baseFileName+nt).replace(base+nt, "")
    while includeStatement.startswith("/"):
        includeStatement = includeStatement[1:]
    while includeStatement.startswith("./"):
        includeStatement = includeStatement[2:]
    while includeStatement.startswith("../"):
        includeStatement = includeStatement[3:]
        fileName = fileName[:fileName.rfind("/", 0, -1)+1]
    return fileName + includeStatement

def readFileDependencies(fileName, include_paths):
    result = {fileName}
    matcher = re.compile('^\\s*#include\\s+[<"]([\\./a-zA-Z0-9_]+\\.((h(pp)?)|(inl)))([">])', re.IGNORECASE)
    with open(fileName, 'r') as f:
        for line in f:
            match = matcher.match(line)
            if match:
                group = match.group(1)
                mode_include = match.group(6) == '>'
                if mode_include:
                    possible_files = [ip + '/' + group for ip in include_paths]
                    possible_files = filter(path.isfile, possible_files)
                    result.update(possible_files)
                else:
                    result.add(replacePath(fileName, group))
    return result

def readDependencies(fileList, include_paths):
    deps = {fileName: readFileDependencies(fileName, include_paths) for fileName in fileList}
    changes = True
    while changes:
        changes = False
        for k, v in list(deps.items()):
            for dep in v:
                deps[k] = deps[k].union(deps[dep])
            changes = changes or len(v) != len(deps[k])
    return deps

def readReveresDependencies(fileList, include_paths):
    deps = readDependencies(fileList, include_paths)
    result = {k: set() for k in deps.keys()}
    for k, v in deps.items():
        for k2 in v:
            result[k2].add(k)
    return result

def getObjectFileName(sourceFileName):
    token = "prohibitedFileName"
    replace = re.compile(f'(\\.cpp{token})|(\\.c{token})', re.IGNORECASE)
    baseName = path.basename(sourceFileName)+token
    return BUILD_LIB_DIRECTORY + "/" + replace.sub('.o', baseName)

def getExecutableFileName(sourceFileName):
    token = "prohibitedFileName"
    baseName = path.basename(sourceFileName)+token
    replace = re.compile(f'(\\.cpp{token})|(\\.c{token})', re.IGNORECASE)
    return BUILD_DIRECTORY + "/" + replace.sub('', baseName)

def listAllFiles(path: str):
    res = []
    for (dir_path, dir_names, file_names) in walk(path):
        res.extend([dir_path+"/"+fn for fn in file_names])
    return res

def crc32(fileName):
    with open(fileName, 'rb') as fh:
        hash = 0
        while True:
            s = fh.read(65536)
            if not s:
                break
            hash = zlib.crc32(s, hash)
        return "%08X" % (hash & 0xFFFFFFFF)


def isExecutable(fileName):
    with open(fileName, 'r') as f:
        for line in f:
            if re.search("((int)|(void))[\\s]+(main)[\\s]*\\((.*)\\)", line) is not None:
                return True
    return False

def tryCreateDir(path):
    try:
        mkdir(path)
        return True
    except OSError as e:
        pass
    return False

tryCreateDir(BUILD_DIRECTORY)
tryCreateDir(BUILD_LIB_DIRECTORY)

buildInfo = dict()
try:
    with open(BUILD_INFO) as f:
        buildInfo = json.loads(f.read())
except:
    pass

buildInfo.setdefault("headers", dict())
buildInfo.setdefault("sources", dict())

newBuildInfo = dict()

allFiles = listAllFiles(SOURCRE_DIRECTORY)
include_paths = [fn[:-len(INCLUDE_DIR_FILE)-1] for fn in allFiles if fn.endswith('/'+INCLUDE_DIR_FILE)]
include_paths.append(SOURCRE_DIRECTORY)
deps = readReveresDependencies(allFiles, include_paths)
headers = [fn for fn in allFiles if fn.lower().endswith(".h") or fn.lower().endswith(".hpp")]
sources = [fn for fn in allFiles if fn.lower().endswith(".c") or fn.lower().endswith(".cpp")]
executables = [fn for fn in sources if isExecutable(fn)]

newBuildInfo["headers"] = {fileName: crc32(fileName) for fileName in headers}
newBuildInfo["sources"] = {fileName: crc32(fileName) for fileName in sources}
filesToBuild = []
newBuildInfo["release"] = COMMAND_RELEASE in argv

if COMMAND_BUILD in argv or newBuildInfo["release"] != buildInfo.get("release", None):
    filesToBuild.extend(sources)
else:
    for fileName, crc in newBuildInfo["sources"].items():
        if crc != buildInfo["sources"].get(fileName, None) or (not path.isfile(getObjectFileName(fileName))):
            filesToBuild.extend(deps[fileName])

    for fileName, crc in newBuildInfo["headers"].items():
        if crc != buildInfo["headers"].get(fileName, None):
            filesToBuild.extend(deps[fileName])

filesToBuild = set(fn for fn in filesToBuild if fn.lower().endswith(".c") or fn.lower().endswith(".cpp"))

flags = RELEASE_FLAGS if COMMAND_RELEASE in argv else DEBUG_FLAGS
include_flags = ['-I'+ip for ip in include_paths]

buildCount = 0
errors = False
for fileName in filesToBuild:
    print(colored("*", "blue"), "Compilation of", fileName, end="\t", flush=True)
    command = ["g++", "-c"] + flags + include_flags + [fileName, "-o", getObjectFileName(fileName)]
    buildCount += 1
    sp = subprocess.run(command)
    if sp.returncode:
        newBuildInfo["sources"][fileName] = None
        errors = True
        print(" [", colored("ERROR", "red"), "]", flush=True)
    else:
        print(" [", colored("OK", "green"), "]", flush=True)

if not errors and (buildCount > 0 or buildInfo.get("linking_error", False)):
    releaseObjectFiles = [getObjectFileName(fileName) for fileName in sources if re.search(TEST_FILE_FILTER, fileName) is None]
    testObjectFiles = [getObjectFileName(fileName) for fileName in sources]

    executableObjects = [getObjectFileName(fileName) for fileName in executables]
    for fileName in executables:
        print(colored("*", "blue"), "Linking", fileName, end="\t", flush=True)
        if  re.search(TEST_FILE_FILTER, fileName) is None:
            currentObjectFiles = releaseObjectFiles
        else:
            currentObjectFiles = testObjectFiles
        currentObjectFiles = [fn for fn in currentObjectFiles if (fn not in executableObjects) or (fn == getObjectFileName(fileName))]
        command = ["g++"] + currentObjectFiles + ["-o", getExecutableFileName(fileName)] + LINKING_PARAMS
        sp = subprocess.run(command, stdout=subprocess.PIPE)
        if sp.returncode:
            errors = True
            newBuildInfo["linking_error"] = True
            print(" [", colored("ERROR", "red"), "]", flush=True)
        else:
            print(" [", colored("OK", "green"), "]", flush=True)
            newBuildInfo["linking_error"] = False

if errors:
    failCount = buildInfo.get("fails", 0)+1
    newBuildInfo["fails"] = failCount
    justLike = "" if failCount == 1 else (" just like the previous one" if failCount == 2 else f" just like {failCount-1} previous builds")
    print(colored("Build failed"+justLike, "red"))
else:
    newBuildInfo["fails"] = 0
    print(colored("Build successful", "green"))

try:
    with open(BUILD_INFO, "w") as f:
        f.write(json.dumps(newBuildInfo, indent=4))
except:
    pass

execs = [getExecutableFileName(fileName) for fileName in executables]
testExecs = [fn for fn in execs if re.search(TEST_FILE_FILTER, fn) is not None]
testExecs.sort()

if not errors:
    if COMMAND_TEST in argv:
        failed = 0
        passed = 0
        for fn in testExecs:
            print(colored("Testing " + fn, "blue"))
            sp = subprocess.run([fn])
            if sp.returncode:
                failed += 1
                print(" [", colored("ERROR", "red"), "]", flush=True)
            else:
                passed += 1
                print(" [", colored("OK", "green"), "]", flush=True)

        if failed == 0:
            print(" [", colored("All tests passed", "green"), "] ")
        elif passed == 0:
            print(" [", colored("All tests failed", "red"), "] ")
            errors = True
        else:
            print(" [", colored(f"{passed} tests passed, {failed} tests failed", "red"), "] ")
            errors = True

exit(1 if errors else 0)

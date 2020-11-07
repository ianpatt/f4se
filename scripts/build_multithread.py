#! python2
import os, sys, string, datetime, shutil, subprocess

def mkdir_safe(a):
	try:
		os.mkdir(a)
	except OSError:
		pass

def quote(a):
	if a.find(" ") >= 0:
		return "\"" + a + "\""
	else:
		return a

# build paths
vanillaRoot = "vanilla"
modifiedRoot = "modified"
buildRoot = "build_src"
buildBinRoot = "build_bin"
fallout4Path = os.environ["Fallout4Path"]
compilerPath = os.path.join(fallout4Path, "Papyrus Compiler\PapyrusCompiler.exe")
buildTime = datetime.datetime.utcnow().isoformat(" ")

if not os.access(vanillaRoot, os.F_OK):
	print "vanilla source files not found"
	sys.exit(0)

# clean
print "cleaning old files"

shutil.rmtree(buildRoot, True)
shutil.rmtree(buildBinRoot, True)
mkdir_safe(buildRoot)
mkdir_safe(buildBinRoot)

# scan modified folder, append contents to vanilla file
print "merging files"

for name in os.listdir(modifiedRoot):
	# build paths
	modifiedPath = os.path.join(modifiedRoot, name)
	vanillaPath = os.path.join(vanillaRoot, name)
	buildPath = os.path.join(buildRoot, name)
	
	# open files
	modifiedSrc = open(modifiedPath, "r")
	try:
		vanillaSrc = open(vanillaPath, "r")
	except IOError:
		vanillaSrc = None

	dst = open(buildPath, "w")
	# copy data
	if vanillaSrc:
		dst.writelines(vanillaSrc.readlines())
		dst.write("\n")
		dst.write("\n")
		dst.write("; F4SE additions built " + buildTime + " UTC\n")
	dst.writelines(modifiedSrc.readlines())
	dst.close()

# run compiler on merged files
print "compiling"

argList = [
	compilerPath,
	quote("F4SE.ppj"),
]
	
if subprocess.call(argList) != 0:
	print "Compile failed."

#! python2
import os, sys, string, shutil, stat

# build paths
vanillaRoot = "vanilla"

fallout4Path = os.environ["Fallout4Path"]
scriptPath = os.path.join(fallout4Path, "Data\Scripts\Source\Base")

if not os.access(vanillaRoot, os.F_OK):
	print "vanilla source files not found"
	sys.exit(0)

print "Copying base files"

for name in os.listdir(vanillaRoot):
	vanillaPath = os.path.join(vanillaRoot, name)
	basePath = os.path.join(scriptPath, name)
	os.chmod(vanillaPath, stat.S_IWRITE )
	shutil.copy2(basePath, vanillaPath)

print "Finished copying files."

#! python2
import os, sys, struct

buildPath = "build_bin"

for name in os.listdir(buildPath):
	filePath = os.path.join(buildPath, name)

	file_header = []
	file_data = []
	with open(filePath, "rb") as script:
		file_header = script.read(16)

		# These were almost Pascal strings, too bad this could have been shorter
		dir_size = struct.unpack("<H", script.read(2))[0]
		dir = struct.unpack("<" + str(dir_size) + "s", script.read(dir_size))[0]
		user_size = struct.unpack("<H", script.read(2))[0]
		user = struct.unpack("<" + str(user_size) + "s", script.read(user_size))[0]
		computer_size = struct.unpack("<H", script.read(2))[0]
		computer = struct.unpack("<" + str(computer_size) + "s", script.read(computer_size))[0]

		# Read the remaining data from here
		file_data = script.read()

	print "Sanitizing " + name
	print "\t" + dir
	print "\t" + user
	print "\t" + computer

	# Rewrite the file without the names
	with open(filePath, "wb") as script:
		script.write(file_header)
		script.write(struct.pack("<H", 0)) # Directory
		script.write(struct.pack("<H", 0)) # User
		script.write(struct.pack("<H", 0)) # Computer
		script.write(file_data)
import os

HEADER_TYPES = ( ".h", ".hpp", ".hxx", ".inl" )
SOURCE_TYPES = ( ".c", ".cpp", ".cxx" )
ALL_TYPES = HEADER_TYPES + SOURCE_TYPES

def make_cmake(a_directories):
	for directory in  a_directories:
		os.chdir(directory)

		tmp = []
		with os.scandir() as it:
			for entry in it:
				if not entry.name.startswith('.') and entry.is_file():
					tmp.append(entry.name)

		headers = []
		sources = []
		for file in tmp:
			if file.endswith(HEADER_TYPES):
				headers.append(file)
			elif file.endswith(SOURCE_TYPES):
				sources.append(file)

		headers.sort()
		sources.sort()

		def do_make(a_filename, a_varname, a_files):
			with open("cmake/{}.cmake".format(a_filename), "w", encoding="utf-8") as out:
				out.write("set({}\n".format(a_varname))

				for file in a_files:
					out.write("\t{}\n".format(file))

				out.write(")\n")

		do_make("headerlist", "HEADERS", headers)
		do_make("sourcelist", "SOURCES", sources)

		os.chdir("..")

def make_papyrus(a_directories):
	root = os.getcwd()
	os.chdir("scripts")
	for directory in a_directories:
		os.chdir(directory)

		files = []
		with os.scandir() as it:
			for entry in it:
				if entry.is_file() and entry.name.endswith(".psc"):
					files.append(entry.name.rpartition(".")[0])

		files.sort()
		with open("{}/cmake/{}_scripts.cmake".format(root, directory), "w", encoding="utf-8") as out:
			out.write("set({}_SCRIPTS\n".format(directory.upper()))

			for file in files:
				out.write("\t{}\n".format(file))

			out.write(")\n")

		os.chdir("..")
	os.chdir("..")

def main():
	cur = os.path.dirname(os.path.realpath(__file__))
	os.chdir("{}/..".format(cur))
	make_cmake(( "f4se", "f4se_common", "f4se_loader", "f4se_loader_common", "f4se_steam_loader" ))
	make_papyrus(( "modified", "vanilla" ))

if __name__ == "__main__":
	main()

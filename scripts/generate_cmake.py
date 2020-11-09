import os

HEADER_TYPES = (".h", ".hpp", ".hxx", ".inl")
SOURCE_TYPES = (".c", ".cpp", ".cxx")
ALL_TYPES = HEADER_TYPES + SOURCE_TYPES

def make_cmake(a_directories):
	for directory in  a_directories:
		os.chdir("../" + directory)

		tmp = list()
		with os.scandir() as it:
			for entry in it:
				if not entry.name.startswith('.') and entry.is_file():
					tmp.append(entry.name)

		headers = list()
		sources = list()
		for file in tmp:
			if name.endswith(HEADER_TYPES):
				headers.append(file)
			elif name.endswith(SOURCE_TYPES):
				sources.append(file)

		headers.sort()
		sources.sort()

		def do_make(a_filename, a_varname, a_files):
			out = open("cmake/" + a_filename + ".cmake", "w", encoding="utf-8")
			out.write("set(" + a_varname + "\n")

			for file in a_files:
				out.write("\t" + file + "\n")

			out.write(")\n")

		do_make("headerlist", "headers", headers)
		do_make("sourcelist", "sources", sources)

def main():
	cur = os.path.dirname(os.path.realpath(__file__))
	os.chdir(cur)
	make_cmake([ "f4se", "f4se_common", "f4se_loader", "f4se_loader_common", "f4se_steam_loader" ])

if __name__ == "__main__":
	main()

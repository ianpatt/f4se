import argparse, os, zipfile

def make_zipfile(a_name):
	return zipfile.ZipFile("{}.7z".format(a_name), "w", zipfile.ZIP_LZMA)

def make_rel_archive(a_name, a_args):
	zip = make_zipfile(a_name)
	def write(a_from, a_to):
		zip.write(a_from, "{}/{}".format(a_name, a_to))

	for file in a_args.files:
		write(file, os.path.basename(file))

	with os.scandir(os.path.join("papyrus", "sanitized")) as it:
		for entry in it:
			if entry.is_file() and entry.name.endswith(".pex"):
				write(entry.path, "Data/Scripts/{}".format(entry.name))

	with os.scandir(os.path.join("papyrus", "src")) as it:
		for entry in it:
			if entry.is_file() and entry.name.endswith(".psc"):
				write(entry.path, "Data/Scripts/Source/User/{}".format(entry.name))

def make_dbg_archive(a_name, a_args):
	zip = make_zipfile("{}_pdbs".format(a_name))
	for pdb in a_args.pdbs:
		zip.write(pdb, os.path.basename(pdb))

def parse_arguments():
	parser = argparse.ArgumentParser(description="archive build artifacts for distribution")
	parser.add_argument("--files", type=str, help="the files to archive", nargs="+", required=True)
	parser.add_argument("--major", type=int, help="f4se major version", required=True)
	parser.add_argument("--minor", type=int, help="f4se minor version", required=True)
	parser.add_argument("--patch", type=int, help="f4se patch version", required=True)
	parser.add_argument("--pdbs", type=str, help="the pdbs to archive", nargs="+", required=True)
	parser.add_argument("--src-dir", type=str, help="the project root source directory", required=True)
	return parser.parse_args()

def main():
	print("Archiving artifacts...")

	out = "artifacts"
	os.makedirs(out, exist_ok=True)
	os.chdir(out)

	args = parse_arguments()
	name = "f4se_{}_{:02}_{:02}".format(args.major, args.minor, args.patch)
	make_rel_archive(name, args)
	make_dbg_archive(name, args)

if __name__ == "__main__":
	main()

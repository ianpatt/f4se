import argparse, datetime, mmap, os, shutil, struct, subprocess, zipfile

FLAG_FILE = "Institute_Papyrus_Flags.flg"

def copy_vanilla_scripts(a_args):
	print("Copying vanilla scripts...")

	filename = os.path.join(a_args.f4dir, "Data", "Scripts", "Source", "Base", "Base.zip")
	with zipfile.ZipFile(filename) as zip:
		vanilla = os.path.join(a_args.src_dir, "scripts", "vanilla")
		with os.scandir(vanilla) as it:
			for entry in it:
				if entry.is_file() and entry.name.endswith(( ".psc", ".flg" )):
					dst = os.path.join(vanilla, entry.name)
					with zip.open(entry.name) as zf, open(dst, "wb") as f:
						shutil.copyfileobj(zf, f)

def merge_scripts(a_args):
	print("Merging scripts...")

	root = os.path.join(a_args.src_dir, "scripts")

	vanilla = {}
	with os.scandir(os.path.join(root, "vanilla")) as it:
		for entry in it:
			if entry.is_file() and entry.name.endswith(".psc"):
				vanilla[entry.name] = entry.path

	now = datetime.datetime.utcnow().isoformat()
	outdir = "src"
	os.makedirs(outdir, exist_ok=True)

	with os.scandir(os.path.join(root, "modified")) as it:
		for entry in it:
			if entry.is_file() and entry.name.endswith(".psc"):
				with open(os.path.join(outdir, entry.name), "w") as dst:
					if entry.name in vanilla:
						with open(vanilla[entry.name]) as src:
							shutil.copyfileobj(src, dst)
							dst.write("\n")
							dst.write("\n")
							dst.write("; F4SE additions built {} UTC\n".format(now))
					with open(entry.path) as src:
						shutil.copyfileobj(src, dst)

	with open(os.path.join(root, "vanilla", FLAG_FILE)) as src, open(os.path.join(outdir, FLAG_FILE), "w") as dst:
		shutil.copyfileobj(src, dst)

def compile_scripts(a_args):
	compiler = os.path.join(a_args.f4dir, "Papyrus Compiler", "PapyrusCompiler.exe")
	vanilla = os.path.join(a_args.src_dir, "scripts", "vanilla")
	merged = os.path.join(os.getcwd(), "src")
	output = os.path.join(os.getcwd(), "bin")

	subprocess.run([
			compiler,
			merged,
			"-import={};{}".format(merged, vanilla),
			"-output={}".format(output),
			"-flags={}".format(FLAG_FILE),
			"-optimize",
			"-release",
			"-final",
			"-all",
		],
		check=True)

def sanitize_scripts(a_args):
	print("Sanitizing scripts...")

	def read_header(a_buffer):
		bstr = a_buffer.read(16)
		result = struct.unpack("<IBBHQ", bstr)
		return type('Header', (), {
			"magic": result[0],
			"major": result[1],
			"minor": result[2],
			"id": result[3],
			"timestamp": result[4],
			"bytes": bstr,
		})

	def read_wstring(a_buffer):
		len = struct.unpack("<H", a_buffer.read(2))[0]
		a_buffer.seek(len, os.SEEK_CUR)

	outdir = "sanitized"
	os.makedirs(outdir, exist_ok=True)

	with os.scandir("bin") as it:
		for entry in it:
			if entry.is_file() and entry.name.endswith(".pex"):
				with open(entry.path, "rb") as f, mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ) as mm:
					header = read_header(mm)
					if header.magic != 0xFA57C0DE or header.major != 3 or header.minor != 9 or header.id != 2:
						raise Exception("Unrecognized file format")

					read_wstring(mm)	# src path
					read_wstring(mm)	# user
					read_wstring(mm)	# machine

					with open(os.path.join(outdir, entry.name), "wb") as o:
						o.write(header.bytes)
						for i in range(3):
							o.write(b'\x00\x00')
						o.write(mm)

def parse_arguments():
	parser = argparse.ArgumentParser(description="compile papyrus source scripts")
	parser.add_argument("--f4dir", type=str, help="the fallout 4 directory", required=True)
	parser.add_argument("--src-dir", type=str, help="the project root source directory", required=True)
	parser.add_argument("--target", type=str, help="the target to execute", required=True)
	return parser.parse_args()

def main():
	out = os.path.join("artifacts", "papyrus")
	os.makedirs(out, exist_ok=True)
	os.chdir(out)

	args = parse_arguments()
	targets = {
		"copy_vanilla": copy_vanilla_scripts,
		"merge": merge_scripts,
		"compile": compile_scripts,
		"sanitize": sanitize_scripts,
	}

	targets[args.target](args)

if __name__ == "__main__":
	main()

## gbafhash documentation

Note: to compile this tool, run `gcc -O3 *.c -o ../gbafhash` (or your compiler's equivalent) from this directory.
Depending on your compiler's settings, some warnings may be generated; those warnings are spurious and should be
silenced or ignored.

### Purpose

This tool will compute hashes for functions in GBA compiled files. The goal of this tool is to aid decompilation of
said files, by quickly verifying whether a new version of a function matches the previous one.

The tool works on two kinds of files, told apart by a value in the ELF header:

* **Relocatable files**, typically carrying a `.o` extension, are files that result from compiling a translation unit
  (e.g., a `.c` file); they will contain external references in the form of _relocations_ that must be resolved in
  order to obtain the final binary data.
* **Executable files**, typically carrying a `.elf` extension, are files that result from the full process of
  compiling and linking the program; they are the final binary files from which ROM images (typically `.gba` files)
  are extracted. These files should not contain any external references, as any program image loaded into a GBA must
  be self-contained (since the platform does not contain an OS or a dynamic program loader).

### Execution modes

Resulting from the differences between relocatable and executable files and the different possible ways of performing
relocation, and taking into account the main goal of matching functions to a known good source, the program can
operate in four different modes:

* **Pseudo-relocation**: used with relocatable files; it is intended for situations where the known good source is
  unavailable or unreliable. All relocations are made up, with values based on a hash of the symbol's name; therefore,
  the computed hash of the functions will depend on the names of external symbols, and will be different from the hash
  of the real functions in the linked binary. This mode is only useful for comparing relocatable files against one
  another.
* **Self-relocation**: used with executable files; it is similar to pseudo-relocation, but instead of making up the
  values of symbols, it uses the symbols in the executable file itself. This mode is useful for computing reference
  hashes from a known good source, or to compare a whole binary against such a source.
* **External relocation**: intended for use with relocatable files (although it will accept executable files as well),
  this mode will perform relocations against an external known good source, which must be an executable file itself.
  The goal of this mode is to ensure that the hashes of the functions in a relocatable file match the hashes that
  would be found in the corresponding executable file, thus making these hashes depend on the known good source
  instead of depending on environmental constraints such as symbol names. This mode is therefore recommended whenever
  a known good source is available.
* **External comparison**: similar to external relocation, but instead of computing hashes for the functions in the
  file, it will compare them byte-by-byte with the functions in the external known good source and output a simple
  OK/FAIL status for each one. (Functions that differ in size will always output FAIL without any relocation or
  comparison, since they cannot possibly match each other.) This mode can be useful for a simpler comparison between
  two executable files, or between a relocatable file and a known good source.

### Usage

    gbafhash [-c <base> | -r <base>] [-f <pattern> [-f <pattern> ...]] [--] <file> [<file> ...]
    gbafhash [-? | -v]

Any number of input files can be given; the program will simply output the hashes (or the OK/FAIL results in external
comparison mode) for the functions in the file if only one input file is given, but if there is more than one, it
will prepend each list with the corresponding filename and separate them with a blank line.

The execution mode is controlled by the `-c` and `-r` options, which respectively select the external comparison and
external relocation modes; the known good source to be used as a reference file is given as an argument to these
options. (These options cannot be used simultaneously.) If neither option is given, the program will select between
the other two execution modes based on the ELF file type declared in the file header for each input file, using
pseudo-relocation mode for relocatable files and self-relocation mode for executable files.

By default, the program will compute hashes for all functions found in the input files. This can be constrained using
the `-f` option, which can take a function name (to be matched exactly) or a pattern containing a single `*` character
that will match any sequence of characters in the name (for example, `a*z` will match `az`, `abz` and `abcz`). This
option can be given multiple times; in that case, all functions that match at least one of the patterns will be
hashed. (Note that patterns containing more than one `*` character are not supported at the moment.)

Finally, the `--` flag can be used as a delimiter, forcing all subsequent arguments to be interpreted as filenames
and not as options, even if they begin with a hyphen. The `-v` option will make the program print its version number,
and the `-?` option will make it print some help information.

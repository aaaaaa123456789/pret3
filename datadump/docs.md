## ddump documentation

Note: to compile this tool, run `gcc -O3 *.c -o ../ddump` (or your compiler's equivalent) from this directory.
Depending on your compiler's settings, some warnings may be generated; those warnings are spurious and should be
silenced or ignored.

### Usage

    ddump [-a <mode> | -s <script>] [-c <config>] [-l <symfile>] [--] <repo path> <file> [<file> [...]]

(Note to previous version users: beware that the repository path is now the first argument)

The repository path should be the directory to which the `.incbin` directives are relative, without a trailing slash.
The following command-line arguments are treated as input files, and they are processed in place.

If the `-a` or `-s` options aren't used, the program will start in interactive mode. This mode prompts for an action
for each `.incbin` directive found in the input file. Note that in interactive mode, only one input file can be passed
to the command line; passing two filenames will treat the second one as the output file (instead of working in place),
and passing three or more is an error.

Command-line options:

* `-a <mode>`: runs the program in automatic dumping mode. Valid values for `<mode>` are `8`, `16`, `32` and `ptr`.
  Each mode is equivalent to the corresponding `data` command in interactive mode; for instance, `-a 8` will dump all
  `.incbin` directives as single bytes. (Note that `.incbin` directives whose lengths are not aligned to the selected
  dumping mode will not be dumped, as well as those which don't contain valid pointers in `-a ptr` mode.)
* `-c <config>`: sets a setting to a value prior to running the program. The argument to this option must be in
  `setting=value` form, and multiple settings can be separated by commas; no spaces are allowed. This option can also
  be given multiple times for the same effect: for instance, `-c headers=off,indent=tab` is the same as
  `-c headers=off -c indent=tab`. Valid settings and values are the same as in interactive configuration mode.
* `-l <symfile>`: loads a symfile before running the program, the same as if it was loaded via the `loadsym` command in
  interactive mode.
* `-s <script>`: runs the program in automatic scripting mode. The program will run the given script for each `.incbin`
  directive encountered in the input files, and dump all of those for which the script doesn't error. Note that if the
  script itself is bugged (for instance, if it contains a syntax error), nothing will be dumped.
* `--`: marks the end of the option list. This is useful if the repository path or any of the input filenames may start
  with a hyphen.

Note that options and input files may be mixed freely (with the exception that all arguments that occur after `--` will
be treated as filenames); however, options' arguments must immediately follow the option. Also, coalescing options into
a single switch (such as `-ac` for `-a -c`) is forbidden.

### Further reading

Information about the scripting language can be found on [the wiki][datadump-wiki].

[datadump-wiki]: https://github.com/TwitchPlaysPokemon/pret3/wiki/datadump-scripting-language

# pret3
Miscellaneous GBA decompilation tools.

A makefile is included to build all the tools here; type `make` to do so. The folders have slightly different names
than the tools themselves to avoid a collision between the folder names and the filenames; hopefully it should be
clear which folder belongs to which tool.

To build datadump: `gcc -O3 datadump/*.c -o ddump`. For more information, view [the documentation](datadump/docs.md).

To build gbafhash: `gcc -O3 funchash/*c -o gbafhash`. For more information, view the documentation that is currently
a WIP.

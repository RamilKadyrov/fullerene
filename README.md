# Fullerene diagram #

An application to create a Schlegel diagram of a fullerene.

![A fullerene Schlegel diagram example](/example/1.svg)

## To build: ##

### Windows ###

Visual Studio required.

### Linux ###

    make

## To use: ##

    fullerene input_file_name.txt output_file_name.svg

for example:

    bin/fullerene resources/1.txt example/1.svg

### Input file format ###

Number of faces, Index of face to expand, pentagons indexes with comma delimiter

Input files examples are in a resources folder

### Output file format ###

Output files are in a SVG format

Output files examples are in a example folder
-----------------------------
Tutorial 1: Read a model file
-----------------------------

CellML files contain all the information defining a model, as well as
annotation data regarding the source or provenance of any items that are
included.  At the end of this tutorial you will be able to open and read
an existing CellML file and load its contents into a model.

**Get started:** Download tutorial resources from here ??

The most basic form of a useful* CellML model has four parts
or :term:`entities<entity>`:

- the model itself
- a component (there can be more than one, but that will come later)
- some variables
- some mathematics describing how the variables interact

\* other collections of information may be considered "valid" in that
do not disobey the specification, but they are not actually useful
for anything, so we don't deal with them here.

The model is a hierarchical structure, as in the diagram below. ??

?? diagram showing model hierarchy

Each of these entities has a corresponding object type in the libcellml
library, named respectively ``Model``, ``Component``, ``Variable``. The
mathematics is a little different, but we'll get to that shortly.

CellML files are not really intended for humans to read, but in simple ones such as this example, we'r smart enough to figure it out.  Here's what we'll be reading:

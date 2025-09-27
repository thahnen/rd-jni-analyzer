# R&D: Analyze native libraries using JNI for unused methods

The [Java Native Interface](https://en.wikipedia.org/wiki/Java_Native_Interface) is part of the
Java runtime to interactive with native code, e.g. C/C++. This relies on the `natvie` keyword in
Java added to a method (static or class does not matter).

This little research project aims at analyzing native libraries for JNI related exported methods
and whether or not they are still used in the current application (on the current classpath).

The logic to analyze antive libraries for JNI related methods itself is done via JNI by providing
OS and architecture specific native libraries, written in (Visual) C++ that interacts with a Java
wrapper doing the actual anlysis.

# R&D: Analyze native libraries using JNI for unused methods

The [Java Native Interface](https://en.wikipedia.org/wiki/Java_Native_Interface) is part of the
Java runtime to interactive with native code, e.g. C/C++. This relies on the `natvie` keyword in
Java added to a method (static or class does not matter).

This little research project aims at analyzing native libraries for JNI related exported methods
and whether or not they are still used in the current application (on the current classpath).

The logic to analyze antive libraries for JNI related methods itself is done via JNI by providing
OS and architecture specific native libraries, written in (Visual) C++ that interacts with a Java
wrapper doing the actual anlysis.

## Building the archive / running tests

As this project is build using native shared libraries added into the JAR archive (and the Java
classpath used for running tests), this has to be build before running Maven. This in turn is OS
and architecture dependent.

For build instructions on the OS specific shared libraries, take a look at the *README.md* inside
`src/main/visual_cpp`. After each build, the shared libary is copied into the folder
`src/main/resources`.

To build the Jar archive and run the unit tests, invoke the following command:

> mvn clean verify

## SonarQube Cloud analysis

Currently, the analysis only supports the macOS native C++ code and not the Windows one as well.
To execute the analysis, invoke the following command - the property `sonar.token` is set inside
the file `.sonar-project.properties`:

```shell
mvn clean verify \
  org.sonarsource.scanner.maven:sonar-maven-plugin:sonar \
  -D$(cat .sonar-project.properties)
```

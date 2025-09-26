# (Visual) C++ / JNI Shared Libraries

This folder is the base folder for the native, OS and architecture specific projects to create the
(Visual) C++ Shared Libraries (*NativeInspector*) that are shipped inside the JAR archives.

To re-generate the C header file, invoke the following commands from the main project directory:

> javac -h src/main/visual_cpp/include src/main/java/com/hahnentt/rd/jni/NativeInspector.java

> rm -f src/main/java/com/hahnentt/rd/jni/*.class

## macOS: Eclipse CDT C++ Managed Build

The shared libary for macOS is created using the *Eclipse CDT* and a *C++ Managed Build* project
that can be build from the command line via the *org.eclipse.cdt.managedbuilder.core.headlessbuild*
Eclipse application or from within the IDE itself.

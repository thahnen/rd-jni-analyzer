# (Visual) C++ / JNI Shared Libraries

This folder is the base folder for the native, OS and architecture specific projects to create the
(Visual) C++ Shared Libraries (*NativeInspector*) that are shipped inside the JAR archives.

To re-generate the C header file, invoke the following commands from the main project directory:

> javac -h src/main/visual_cpp/include src/main/java/com/hahnentt/rd/jni/NativeInspector.java

> rm -f src/main/java/com/hahnentt/rd/jni/*.class

## macOS: Eclipse CDT C++ Managed Build

The shared libary for macOS is created using the *Eclipse CDT* and a *C++ Managed Build* project
that can be build from the command line via the *org.eclipse.cdt.managedbuilder.core.headlessbuild*
Eclipse application or from within the IDE itself:

```shell
/Applications/Eclipse\ CPP.app/Contents/MacOS/eclipse \
  -noSplash \
  -application org.eclipse.cdt.managedbuilder.core.headlessbuild \
  -data ~/workspaces/jni-analyzer-cpp \
  -cleanBuild libNativeInspectorOSX
```

This will also create the *Compilation Database* at *Release/compile_commands.json* that has to be
added as the value to the property `sonar.cfamily.compile-commands` for the SonarQube analysis.

## Windows: Visual Studio 2015 Visual C++

The shared library for Windows is created using *Visual Studio 2015* and a *Visual C++* project
that can be build from the *Developer Console for VS2015* using the *msbuild* tool or from within
the IDE itself:

> msbuild.exe /p:configuration=Release /p:platform=x64

To build the compilation information using *Build Wrapper*, since the *Compilation Database* cannot
be easily obtained from a *Visual 2015* build for a *Visual C++* project, run the following command
inside the *Developer Console for VS2015*:

```shell
build-wrapper-win-x86-64.exe \
  --out-dir  build_wrapper_output_directory \
  msbuild.exe /p:configuration=Release /p:platform=x64
```

The output directory `build_wrapper_output_directory` has to be added as the value to the property
`sonar.cfamily.build-wrapper-output` for the SonarQube analysis.

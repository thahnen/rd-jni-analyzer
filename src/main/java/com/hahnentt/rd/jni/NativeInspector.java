package com.hahnentt.rd.jni;

import java.nio.file.Files;
import java.nio.file.StandardCopyOption;

/**
 *  Utility to inspect a native file, e.g. a shared library, and to retrieve the exported JNI
 *  related methods for further inspection.
 *
 *  @since 26.09.2025 18:48:04
 *  @author thahnen
 */
public class NativeInspector {
  private static boolean nativeLibrariyLoaded;

  static {
    var os = System.getProperty("os.name").toLowerCase();
    var libName = "NativeInspector";
    var libFileName = libName;
    if (os.contains("win")) {
      libFileName += ".dll";
    } else if (os.contains("mac")) {
      libFileName += ".dylib";
    } else {
      libFileName += ".so";
    }

    try (var in = NativeInspector.class.getResourceAsStream("/" + libFileName)) {
      var tempPath = Files.createTempFile(libName, libFileName);
      Files.copy(in, tempPath, StandardCopyOption.REPLACE_EXISTING);
      tempPath.toFile().deleteOnExit();

      System.load(tempPath.toAbsolutePath().toString());
      nativeLibrariyLoaded = true;
    } catch (Exception ignored) {
      nativeLibrariyLoaded = false;
    }
  }

  private NativeInspector() {
    // Utility class
  }

  /**
   *  This tries to retrieve all the exported JNI related methods from a native file, e.g. a shared
   *  library.
   *  
   *  @param nativeFilePath is an absolute path on disk
   *  @return all the exported JNI related methods
   *  @throws NativeLibraryNotLoadedException when library could not be loaded
   *  @throws NativeLibraryRuntimeException when excepting originated from native code
   */
  public static String[] listExportedJniMethods(String nativeFilePath)
    throws NativeLibraryNotLoadedException, NativeLibraryRuntimeException {
    if (!nativeLibrariyLoaded) {
      throw new NativeLibraryNotLoadedException("");
    }

    try {
      return listExportedJniMethodsNative(nativeFilePath);
    } catch (Exception exception) {
      throw new NativeLibraryRuntimeException("", exception);
    }
  }

  /**
   *  This is the actual native (Visual) C++ implementation done in each native OS and architecture
   *  specific library that will be shipped inside the JAR archive.
   *  
   *  @param nativeFilePath is an absolute path on disk
   *  @return all the exported JNI related methods
   */
  private static native String[] listExportedJniMethodsNative(String nativeFilePath);

  /**
   *  This exception is thrown when {@link NativeInspector#listExportedJniMethods(String)} invoked
   *  but the loading of the native library failed in the static initializer.
   */
  public static class NativeLibraryNotLoadedException extends RuntimeException {
    private static final long serialVersionUID = -2530514270666018797L;

    public NativeLibraryNotLoadedException(String message) {
      super(message);
    }
  }

  /**
   *  This exception is thrown when the (Visual) C++ code inside the native library threw an
   *  exception, e.g. when reaching an unsupported case, or when running into a C++ specific
   *  runtime exception.
   */
  public static class NativeLibraryRuntimeException extends RuntimeException {
    private static final long serialVersionUID = -2506114282703270017L;

    public NativeLibraryRuntimeException(String message, Throwable cause) {
      super(message, cause);
    }
  }
}

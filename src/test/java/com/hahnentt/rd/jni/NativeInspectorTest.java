package com.hahnentt.rd.jni;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Assumptions;
import org.junit.jupiter.api.Test;

/**
 *  These are not really unit tests but rather integration tests against existing shared libaries
 *  inside the Java installation
 *
 *  @since 26.09.2025 21:53:24
 *  @author thahnen
 */
class NativeInspectorTest {
  private static final String OS = System.getProperty("os.name").toLowerCase();

  @Test
  void test_macOS_librmi() {
    Assumptions.assumeTrue(OS.contains("mac"));

    var javaHome = System.getProperty("java.home");
    Assertions.assertNotNull(javaHome);

    var libjavaPath = javaHome + "/lib/librmi.dylib";
    var exportedJniMethods = NativeInspector.listExportedJniMethods(libjavaPath);
    Assertions.assertTrue(exportedJniMethods.length > 0);
    Assertions.assertEquals("_Java_sun_rmi_transport_GC_maxObjectInspectionAge", exportedJniMethods[0]);
  }
}

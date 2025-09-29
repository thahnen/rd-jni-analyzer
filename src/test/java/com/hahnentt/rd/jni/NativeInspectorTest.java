package com.hahnentt.rd.jni;

import java.io.File;
import java.util.Arrays;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Assumptions;
import org.junit.jupiter.api.Test;

/**
 *  These are not really unit tests but rather integration tests against existing shared libraries
 *  from different public sources (e.g. Java Runtime).
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
  
  @Test
  void test_win_VisusClient_DLL_x86() {
	  Assumptions.assumeTrue(OS.contains("win"));
	  
	  var visusClientDllx86 = new File("src/test/resources/x86/VisusClient.dll");
	  var exportedJniMethods = NativeInspector.listExportedJniMethods(visusClientDllx86.getAbsolutePath());
	  Assertions.assertTrue(exportedJniMethods.length > 0);
	  Assertions.assertTrue(
			  Arrays.stream(exportedJniMethods).anyMatch("Java_com_visustt_tools_system_SystemProperties_isWin64"::equals));
  }
  
  @Test
  void test_win_VisusClient_DLL_x64() {
	  Assumptions.assumeTrue(OS.contains("win"));
	  
	  var visusClientDllx64 = new File("src/test/resources/x64/VisusClient.dll");
	  var exportedJniMethods = NativeInspector.listExportedJniMethods(visusClientDllx64.getAbsolutePath());
	  Assertions.assertTrue(exportedJniMethods.length > 0);
	  Assertions.assertTrue(
			  Arrays.stream(exportedJniMethods).anyMatch("Java_com_visustt_tools_system_SystemProperties_isWin64"::equals));
  }
}

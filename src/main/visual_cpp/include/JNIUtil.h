/*
 * JNIUtil.h
 *
 *  Created on: 26.09.2025
 *      Author: thahnen
 */
#ifndef JNIUTIL_H
#define JNIUTIL_H

#include <vector>
#include <jni.h>

/**
 *  This is used to throw a "java.lang.UnsupportedOperationException" (a runtime exception) towards
 *  the Java runtime in case anything breaks in this native code.
 *
 *  @param env Java environment
 *  @param msg the message added to the exception
 */
void throwUnsupportedOperationException(JNIEnv *env, const char *msg) {
  jclass exClass = env->FindClass("java/lang/UnsupportedOperationException");

  // Yes, "exClass" could be a "nullptr", but we do not care
  env->ThrowNew(exClass, msg);
}

/**
 *  This converts a C++ vector of strings into an array of "java.lang.String".
 *
 *  @param env Java environment
 *  @param vec of strings to be converted
 *  @return the Java String array
 */
jobjectArray convertVectorToJavaArray(JNIEnv *env, std::vector<std::string> vec) {
  jclass stringClass = env->FindClass("java/lang/String");
  jobjectArray result = env->NewObjectArray((jsize) vec.size(), stringClass, nullptr);
  for (int i = 0; i < vec.size(); i++)
    env->SetObjectArrayElement(result, i, env->NewStringUTF(vec[i].c_str()));

  return result;
}

#endif

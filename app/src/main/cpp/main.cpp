#include <jni.h>
#include "Client.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_mark_client_MainActivity_stringFromJNI(JNIEnv* env, jobject) {
  return env->NewStringUTF("Native loaded!");
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return JNI_VERSION_1_6;
}

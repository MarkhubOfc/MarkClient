#include <jni.h>
#include "Client.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_mark_client_MainActivity_stringFromJNI(JNIEnv* env, jobject) {
  return env->NewStringUTF("Native loaded!");
}

__attribute__((constructor))
void on_load() {
  Client::get().init();
}

__attribute__((destructor))
void on_unload() {
  Client::get().shutdown();
}

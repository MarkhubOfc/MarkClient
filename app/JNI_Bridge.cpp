#include <jni.h>
#include "Client.h"

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeInitClient(JNIEnv*, jobject) {
  Client::get().init();
}

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeToggleMenu(JNIEnv*, jobject) {
  Client::get().toggleMenu();
}

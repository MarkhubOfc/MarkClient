#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_android.h"
#include "Client.h"

static ANativeWindow* g_window = nullptr;
static EGLDisplay g_display = EGL_NO_DISPLAY;
static EGLSurface g_surface = EGL_NO_SURFACE;
static EGLContext g_context = EGL_NO_CONTEXT;
static int g_width = 0;
static int g_height = 0;
static bool g_initialized = false;

bool initEGL() {
  g_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (g_display == EGL_NO_DISPLAY) return false;
  
  EGLint major, minor;
  if (!eglInitialize(g_display, &major, &minor)) return false;
  
  EGLint attribs[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 16,
    EGL_NONE
  };
  
  EGLConfig config;
  EGLint numConfigs;
  if (!eglChooseConfig(g_display, attribs, &config, 1, &numConfigs)) return false;
  
  EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
  g_context = eglCreateContext(g_display, config, EGL_NO_CONTEXT, contextAttribs);
  if (g_context == EGL_NO_CONTEXT) return false;
  
  g_surface = eglCreateWindowSurface(g_display, config, g_window, nullptr);
  if (g_surface == EGL_NO_SURFACE) return false;
  
  if (!eglMakeCurrent(g_display, g_surface, g_surface, g_context)) return false;
  
  eglQuerySurface(g_display, g_surface, EGL_WIDTH, &g_width);
  eglQuerySurface(g_display, g_surface, EGL_HEIGHT, &g_height);
  
  glViewport(0, 0, g_width, g_height);
  return true;
}

void shutdownEGL() {
  eglMakeCurrent(g_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
  eglDestroyContext(g_display, g_context);
  eglDestroySurface(g_display, g_surface);
  eglTerminate(g_display);
  g_display = EGL_NO_DISPLAY;
  g_surface = EGL_NO_SURFACE;
  g_context = EGL_NO_CONTEXT;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_mark_client_MainActivity_stringFromJNI(JNIEnv* env, jobject) {
  return env->NewStringUTF("Native loaded!");
}

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeInitClient(JNIEnv*, jobject) {
  Client::get().init();
}

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeInitSurface(JNIEnv* env, jobject, jobject surface, jint width, jint height) {
  g_window = ANativeWindow_fromSurface(env, surface);
  if (!g_window) return;
  
  if (!initEGL()) return;
  
  Client::get().initImGui(g_window, g_width, g_height);
  g_initialized = true;
}

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeRender(JNIEnv*, jobject) {
  if (!g_initialized) return;
  Client::get().tick();
  Client::get().render();
  eglSwapBuffers(g_display, g_surface);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeShutdown(JNIEnv*, jobject) {
  if (!g_initialized) return;
  Client::get().shutdown();
  shutdownEGL();
  if (g_window) {
    ANativeWindow_release(g_window);
    g_window = nullptr;
  }
  g_initialized = false;
}

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeToggleMenu(JNIEnv*, jobject) {
  if (!g_initialized) return;
  Client::get().toggleMenu();
}

extern "C" JNIEXPORT void JNICALL
Java_com_mark_client_MainActivity_nativeTouchEvent(JNIEnv*, jobject, jint action, jfloat x, jfloat y) {
  if (!g_initialized) return;
  ImGuiIO& io = ImGui::GetIO();
  io.MousePos = ImVec2(x, y);
  if (action == 0) io.MouseDown[0] = true;
  else if (action == 1 || action == 3) io.MouseDown[0] = false;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
  return JNI_VERSION_1_6;
}

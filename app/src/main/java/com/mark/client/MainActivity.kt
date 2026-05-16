package com.mark.client

import android.app.Activity
import android.os.Bundle
import android.view.MotionEvent
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.widget.FrameLayout
import android.widget.TextView
import android.view.Choreographer
import android.view.Gravity

class MainActivity : Activity() {
  
  companion object {
    init {
      try {
        System.loadLibrary("markclient")
      } catch (e: UnsatisfiedLinkError) {
        android.util.Log.e("MarkClient", "Failed to load lib: ${e.message}")
      }
    }
  }
  
  private external fun stringFromJNI(): String
  private external fun nativeInitClient()
  private external fun nativeToggleMenu()
  private external fun nativeInitSurface(surface: Surface, width: Int, height: Int)
  private external fun nativeRender()
  private external fun nativeShutdown()
  private external fun nativeTouchEvent(action: Int, x: Float, y: Float)
  
  private var surfaceView: SurfaceView? = null
  private var choreographerCallback: Choreographer.FrameCallback? = null
  
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    
    nativeInitClient()
    
    val layout = FrameLayout(this)
    
    surfaceView = SurfaceView(this).apply {
      holder.addCallback(object : SurfaceHolder.Callback {
        override fun surfaceCreated(holder: SurfaceHolder) {
          nativeInitSurface(holder.surface, width, height)
          startRenderLoop()
        }
        override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {
          nativeShutdown()
          nativeInitSurface(holder.surface, w, h)
        }
        override fun surfaceDestroyed(holder: SurfaceHolder) {
          stopRenderLoop()
          nativeShutdown()
        }
      })
    }
    
    val tv = TextView(this).apply {
      text = "Mark Client v1.0\n${stringFromJNI()}\nTap to open menu"
      textSize = 20f
      setTextColor(0xFFFFFFFF.toInt())
      gravity = Gravity.CENTER
      setOnClickListener { nativeToggleMenu() }
    }
    
    layout.addView(surfaceView)
    layout.addView(tv)
    setContentView(layout)
  }
  
  private fun startRenderLoop() {
    choreographerCallback = object : Choreographer.FrameCallback {
      override fun doFrame(frameTimeNanos: Long) {
        nativeRender()
        Choreographer.getInstance().postFrameCallback(this)
      }
    }
    Choreographer.getInstance().postFrameCallback(choreographerCallback!!)
  }
  
  private fun stopRenderLoop() {
    choreographerCallback?.let {
      Choreographer.getInstance().removeFrameCallback(it)
    }
  }
  
  override fun onTouchEvent(event: MotionEvent): Boolean {
    nativeTouchEvent(event.action, event.x, event.y)
    return true
  }
  
  override fun onDestroy() {
    super.onDestroy()
    stopRenderLoop()
  }
}

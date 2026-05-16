package com.mark.client

import android.app.Activity
import android.os.Bundle
import android.view.MotionEvent
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.widget.TextView
import android.view.View
import android.view.Choreographer

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
  private external fun nativeInitImGui(surface: Surface, width: Int, height: Int)
  private external fun nativeRender()
  private external fun nativeShutdown()
  
  private var surfaceView: SurfaceView? = null
  private var textView: TextView? = null
  private var menuOpen = false
  
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    
    nativeInitClient()
    
    val layout = android.widget.FrameLayout(this)
    
    surfaceView = SurfaceView(this).apply {
      holder.addCallback(object : SurfaceHolder.Callback {
        override fun surfaceCreated(holder: SurfaceHolder) {
          nativeInitImGui(holder.surface, width, height)
          startRenderLoop()
        }
        override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {}
        override fun surfaceDestroyed(holder: SurfaceHolder) {
          nativeShutdown()
        }
      })
    }
    
    textView = TextView(this).apply {
      text = "Mark Client v1.0\n${stringFromJNI()}\nTap to open menu"
      textSize = 20f
      setTextColor(0xFFFFFFFF.toInt())
      setOnClickListener {
        nativeToggleMenu()
      }
    }
    
    layout.addView(surfaceView)
    layout.addView(textView)
    setContentView(layout)
  }
  
  private fun startRenderLoop() {
    Choreographer.getInstance().postFrameCallback(object : Choreographer.FrameCallback {
      override fun doFrame(frameTimeNanos: Long) {
        nativeRender()
        Choreographer.getInstance().postFrameCallback(this)
      }
    })
  }
  
  override fun onTouchEvent(event: MotionEvent): Boolean {
    return super.onTouchEvent(event)
  }
}

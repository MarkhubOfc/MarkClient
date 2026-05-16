package com.mark.client

import android.app.Activity
import android.os.Bundle
import android.view.Gravity
import android.view.MotionEvent
import android.view.Surface
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.widget.Button
import android.widget.FrameLayout
import android.widget.TextView
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
  private external fun nativeIsMenuVisible(): Boolean
  private external fun nativeInitSurface(surface: Surface, width: Int, height: Int)
  private external fun nativeRender()
  private external fun nativeShutdown()
  private external fun nativeTouchEvent(action: Int, x: Float, y: Float)
  
  private var surfaceView: SurfaceView? = null
  private var choreographerCallback: Choreographer.FrameCallback? = null
  private var menuButton: Button? = null
  private var statusText: TextView? = null
  
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    
    val layout = FrameLayout(this)
    
    statusText = TextView(this).apply {
      text = "Status: Initializing..."
      textSize = 14f
      setTextColor(0xFF00FF00.toInt())
      gravity = Gravity.BOTTOM or Gravity.CENTER_HORIZONTAL
    }
    
    surfaceView = SurfaceView(this).apply {
      setZOrderOnTop(false)
      holder.addCallback(object : SurfaceHolder.Callback {
        override fun surfaceCreated(holder: SurfaceHolder) {
          statusText?.text = "Status: Surface created"
          nativeInitClient()
          nativeInitSurface(holder.surface, width, height)
          statusText?.text = "Status: Native init done, menu=" + nativeIsMenuVisible()
          startRenderLoop()
        }
        override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {
          statusText?.text = "Status: Surface changed"
          nativeShutdown()
          nativeInitSurface(holder.surface, w, h)
        }
        override fun surfaceDestroyed(holder: SurfaceHolder) {
          statusText?.text = "Status: Surface destroyed"
          stopRenderLoop()
          nativeShutdown()
        }
      })
    }
    
    val tv = TextView(this).apply {
      text = "Mark Client v1.0\n${stringFromJNI()}"
      textSize = 20f
      setTextColor(0xFFFFFFFF.toInt())
      gravity = Gravity.CENTER
    }
    
    menuButton = Button(this).apply {
      text = "HIDE"
      setOnClickListener {
        nativeToggleMenu()
        val visible = nativeIsMenuVisible()
        text = if (visible) "HIDE" else "MENU"
        statusText?.text = "Status: Menu toggled, visible=$visible"
      }
    }
    
    val btnParams = FrameLayout.LayoutParams(
      FrameLayout.LayoutParams.WRAP_CONTENT,
      FrameLayout.LayoutParams.WRAP_CONTENT
    ).apply {
      gravity = Gravity.TOP or Gravity.END
      topMargin = 50
      rightMargin = 50
    }
    
    val statusParams = FrameLayout.LayoutParams(
      FrameLayout.LayoutParams.MATCH_PARENT,
      FrameLayout.LayoutParams.WRAP_CONTENT
    ).apply {
      gravity = Gravity.BOTTOM
      bottomMargin = 100
    }
    
    layout.addView(surfaceView)
    layout.addView(tv)
    layout.addView(menuButton, btnParams)
    layout.addView(statusText, statusParams)
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
    nativeTouchEvent(event.actionMasked, event.x, event.y)
    return true
  }
  
  override fun onDestroy() {
    super.onDestroy()
    stopRenderLoop()
  }
}

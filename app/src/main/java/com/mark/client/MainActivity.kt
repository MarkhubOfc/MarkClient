package com.mark.client

import android.app.Activity
import android.content.Intent
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.provider.Settings
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
  private external fun nativeForceShowMenu()
  private external fun nativeIsMenuVisible(): Boolean
  private external fun nativeInitSurface(surface: Surface, width: Int, height: Int)
  private external fun nativeRender()
  private external fun nativeShutdown()
  private external fun nativeTouchEvent(action: Int, x: Float, y: Float)
  
  private var surfaceView: SurfaceView? = null
  private var choreographerCallback: Choreographer.FrameCallback? = null
  private var menuButton: Button? = null
  private var infoText: TextView? = null
  
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      if (!Settings.canDrawOverlays(this)) {
        val intent = Intent(
          Settings.ACTION_MANAGE_OVERLAY_PERMISSION,
          Uri.parse("package:$packageName")
        )
        startActivity(intent)
      }
    }
    
    val layout = FrameLayout(this)
    
    surfaceView = SurfaceView(this).apply {
      setZOrderOnTop(false)
      holder.addCallback(object : SurfaceHolder.Callback {
        override fun surfaceCreated(holder: SurfaceHolder) {
          nativeInitClient()
          nativeInitSurface(holder.surface, width, height)
          infoText?.text = "Native OK | Menu: ${nativeIsMenuVisible()}"
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
    
    infoText = TextView(this).apply {
      text = "Loading..."
      textSize = 14f
      setTextColor(0xFF00FF00.toInt())
      gravity = Gravity.BOTTOM or Gravity.CENTER_HORIZONTAL
    }
    
    menuButton = Button(this).apply {
      text = "FORCE SHOW"
      setOnClickListener {
        nativeForceShowMenu()
        val visible = nativeIsMenuVisible()
        text = if (visible) "HIDE" else "SHOW"
        infoText?.text = "Forced menu: $visible"
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
    
    val infoParams = FrameLayout.LayoutParams(
      FrameLayout.LayoutParams.MATCH_PARENT,
      FrameLayout.LayoutParams.WRAP_CONTENT
    ).apply {
      gravity = Gravity.BOTTOM
      bottomMargin = 150
    }
    
    layout.addView(surfaceView)
    layout.addView(infoText, infoParams)
    layout.addView(menuButton, btnParams)
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

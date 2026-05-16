package com.mark.client

import android.app.Activity
import android.os.Bundle
import android.widget.TextView

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
  
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    
    nativeInitClient()
    
    val tv = TextView(this)
    tv.text = "Mark Client v1.0\n${stringFromJNI()}\nTap to open menu"
    tv.textSize = 20f
    tv.setOnClickListener { nativeToggleMenu() }
    setContentView(tv)
  }
  
  private external fun stringFromJNI(): String
  private external fun nativeInitClient()
  private external fun nativeToggleMenu()
}

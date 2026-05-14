package com.mark.client

import android.app.Activity
import android.os.Bundle
import android.widget.TextView

class MainActivity : Activity() {
  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    val tv = TextView(this)
    tv.text = "Mark Client v1.0\n${stringFromJNI()}"
    tv.textSize = 20f
    setContentView(tv)
  }
  private external fun stringFromJNI(): String
}

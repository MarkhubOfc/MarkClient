package com.mark.client

import android.app.Application
import android.util.Log

class MarkClient : Application() {
  companion object {
    const val TAG = "MarkClient"
    init { System.loadLibrary("markclient") }
  }
  
  override fun onCreate() {
    super.onCreate()
    Log.i(TAG, "Client initialized")
  }
}

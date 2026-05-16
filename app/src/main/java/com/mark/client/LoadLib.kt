package com.mark.client

import android.content.Context
import android.graphics.PixelFormat
import android.view.MotionEvent
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.widget.FrameLayout

object LoadLib {

    data class WindowConfig(
        val title: String = "Window",
        val theme: WindowTheme = WindowTheme.Dark,
        val width: Float = 0.9f,
        val height: Float = 0.7f,
        val draggable: Boolean = true,
        val closable: Boolean = true
    )

    enum class WindowTheme {
        Dark, Light
    }

    class WindowBuilder(private val context: Context) {
        private var config = WindowConfig()

        fun title(title: String) = apply { config = config.copy(title = title) }
        fun theme(theme: WindowTheme) = apply { config = config.copy(theme = theme) }
        fun size(width: Float, height: Float) = apply {
            config = config.copy(width = width, height = height)
        }
        fun draggable(value: Boolean) = apply { config = config.copy(draggable = value) }
        fun closable(value: Boolean) = apply { config = config.copy(closable = value) }

        fun build(): FloatingWindow {
            return FloatingWindow(context, config)
        }
    }

    class FloatingWindow(context: Context, val config: WindowConfig) : FrameLayout(context) {
        private var surfaceView: SurfaceView? = null
        private var touchListener: ((Float, Float, Boolean) -> Unit)? = null

        init {
            setupSurface()
        }

        private fun setupSurface() {
            surfaceView = SurfaceView(context).apply {
                setZOrderOnTop(true)
                holder.setFormat(PixelFormat.TRANSLUCENT)
                holder.addCallback(object : SurfaceHolder.Callback {
                    override fun surfaceCreated(holder: SurfaceHolder) {}
                    override fun surfaceChanged(holder: SurfaceHolder, format: Int, w: Int, h: Int) {}
                    override fun surfaceDestroyed(holder: SurfaceHolder) {}
                })
            }
            addView(surfaceView)
        }

        fun setTouchCallback(callback: (Float, Float, Boolean) -> Unit) {
            touchListener = callback
        }

        override fun onTouchEvent(event: MotionEvent): Boolean {
            val down = event.actionMasked == MotionEvent.ACTION_DOWN
            touchListener?.invoke(event.x, event.y, down)
            return true
        }

        fun getSurfaceView(): SurfaceView? = surfaceView
    }

    fun Window(config: WindowConfig.() -> Unit): WindowBuilder {
        val cfg = WindowConfig().apply(config)
        return WindowBuilder(cfg)
    }
}

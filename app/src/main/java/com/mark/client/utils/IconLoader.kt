package com.mark.client

import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.util.Log

object IconLoader {

    private const val TAG = "IconLoader"
    private const val ICON_PATH = "Icon/"
    private val cache = mutableMapOf<String, Bitmap>()

    fun load(context: Context, name: String): Bitmap? {
        val key = ICON_PATH + name
        cache[key]?.let { return it }

        return try {
            context.assets.open(key).use { stream ->
                BitmapFactory.decodeStream(stream)?.also {
                    cache[key] = it
                    Log.i(TAG, "Loaded icon: $name (${it.width}x${it.height})")
                }
            }
        } catch (e: Exception) {
            Log.e(TAG, "Failed to load icon: $name - ${e.message}")
            null
        }
    }

    fun loadAll(context: Context, names: List<String>): Map<String, Bitmap> {
        return names.mapNotNull { name ->
            load(context, name)?.let { name to it }
        }.toMap()
    }

    fun toByteArray(bitmap: Bitmap): ByteArray {
        val size = bitmap.rowBytes * bitmap.height
        val buffer = java.nio.ByteBuffer.allocate(size)
        bitmap.copyPixelsToBuffer(buffer)
        return buffer.array()
    }

    fun getWidth(bitmap: Bitmap): Int = bitmap.width
    fun getHeight(bitmap: Bitmap): Int = bitmap.height

    fun clear() {
        cache.clear()
    }

    fun listAvailable(context: Context): List<String> {
        return try {
            context.assets.list(ICON_PATH)?.toList() ?: emptyList()
        } catch (e: Exception) {
            Log.e(TAG, "Failed to list icons: ${e.message}")
            emptyList()
        }
    }
}

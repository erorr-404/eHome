package com.erorr404.ehome

import android.annotation.SuppressLint
import android.content.Context
import android.os.Bundle
import android.webkit.JavascriptInterface
import android.webkit.WebChromeClient
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat

class MainActivity : AppCompatActivity() {
    @SuppressLint("JavascriptInterface", "SetJavaScriptEnabled")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        val mainWebView: WebView = findViewById(R.id.mainWebView)
        mainWebView.settings.javaScriptEnabled = true // Enable JavaScript
        mainWebView.webViewClient = WebViewClient()  // Set WebViewClient to keep navigation inside the WebView
        mainWebView.addJavascriptInterface(WebInterface(this), "AndroidBridge") // Add JavaScript interface

        mainWebView.loadUrl("file:///android_asset/www/index.html") // For local HTML
        WebView.setWebContentsDebuggingEnabled(true)
    }
}

private class WebInterface(private val context: Context) {

    @JavascriptInterface
    fun showToast(message: String) {
        Toast.makeText(context, message, Toast.LENGTH_SHORT).show()
    }

    @JavascriptInterface
    fun getAndroidData(): String {
        return "Data from Android"
    }
}
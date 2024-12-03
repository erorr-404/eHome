package com.erorr404.ehome

import android.annotation.SuppressLint
import android.content.Context
import android.os.Bundle
import android.webkit.JavascriptInterface
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.util.Log

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

        val mainWebView: WebView = findViewById(R.id.mainWebView) // Get WebView
        mainWebView.settings.javaScriptEnabled = true // Enable JavaScript
        mainWebView.webViewClient = WebViewClient()  // Set WebViewClient to keep navigation inside the WebView
        mainWebView.addJavascriptInterface(WebInterface(this), "AndroidBridge") // Add JavaScript interface

        mainWebView.loadUrl("file:///android_asset/www/index.html") // For local HTML
        WebView.setWebContentsDebuggingEnabled(true) // For debugging using chrome://inspect or something
    }
}

private class WebInterface(private val context: Context) {
    @JavascriptInterface
    fun logError(message: String) {
        Log.e("WebViewError", message)
    }

    @JavascriptInterface
    fun showToast(message: String) {
        Toast.makeText(context, message, Toast.LENGTH_SHORT).show()
    }

    @JavascriptInterface
    fun isInternetAvailable(): Boolean {
        val connectivityManager = context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        val network = connectivityManager.activeNetwork ?: return false
        val networkCapabilities = connectivityManager.getNetworkCapabilities(network) ?: return false
        return networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
    }
}
package com.example.rainapp1

import android.os.Bundle
import android.webkit.WebView
import android.webkit.WebViewClient
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val myWebView: WebView = findViewById(R.id.webview)
        val webSettings = myWebView.settings
        webSettings.javaScriptEnabled = true
        myWebView.webViewClient = WebViewClient()
        myWebView.loadUrl("https://rainflow.live")

    }

    override fun onBackPressed()
    {
        val myWebView: WebView = findViewById(R.id.webview)
        val webSettings = myWebView.settings
        webSettings.javaScriptEnabled = true
        myWebView.webViewClient = WebViewClient()
        var currentURL = myWebView.url
        if (currentURL == "https://rainflow.live/flood")
        {
            myWebView.loadUrl("https://rainflow.live")
        }
        else if (currentURL == "https://rainflow.live/rain")
        {
            myWebView.loadUrl("https://rainflow.live")
        }
        else if (currentURL == "https://rainflow.live/mobileflood")
        {
            myWebView.loadUrl("https://rainflow.live/mobile")
        }
        else if (currentURL == "https://rainflow.live/mobilerain")
        {
            myWebView.loadUrl("https://rainflow.live/mobile")
        }
        else if (currentURL == "https://dashboard.rainflow.live")
        {
            myWebView.loadUrl("https://rainflow.live")
        }
        else
        {
            myWebView.loadUrl("https://rainflow.live")
        }


    }


}

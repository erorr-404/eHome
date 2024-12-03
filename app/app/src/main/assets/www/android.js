function showAndroidToast(text) { // Show Android Toast with text
  AndroidBridge.showToast(text);
}

function checkInternetConnection() { // Check if there is Internet connection
  const internet = AndroidBridge.isInternetAvailable();
  return internet
}

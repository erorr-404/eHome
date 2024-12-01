function callAndroidFunction() {
    // Call Android method from JavaScript
    AndroidBridge.showToast("Hello from JavaScript!");
  }

  function getAndroidData() {
    // Get data from Android
    let data = AndroidBridge.getAndroidData();
    document.getElementById("androidData").innerText = data;
  }


try {
    const aList = Android.testFunction()
    console.log(aList);
} catch(e) {
    console.log("Error: ", e)
}
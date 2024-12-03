const mainContainer = $("#main") // Get <main></main>

const statsPage = $("#stats-page") // Get statistics, home and settings pages
const homePage = $("#home-page")
const settingsPage = $("#settings-page")

const statsButton = $("#stats-button") // Get stats, home and settings buttons
const homeButton = $("#home-button")
const settingsButton = $("#settings-button")


function fetchData() { // Fetch data from ESP32
  $.getJSON("http://192.168.1.105/data", function (data) {
    console.log(data);
    showAndroidToast(data)
  })
    .fail(function () { // If got error
      try { // Check if there is Internet connection
        const internetAvailable = checkInternetConnection()
        if (internetAvailable) { // If there is Internet connection, then there is no light at home
          console.log("There is no light at home.");
          showAndroidToast("Дома немає світла")
        } else {
          console.log("There is no Internet connection.");
          showAndroidToast("Немає підключення до інтернету") // Notify user there is no Internet connection
        }
      } catch { // JS can use checkInternetConnection() function only with JavascriptInterface
        console.log("This function is available only for mobile app.");
        showAndroidToast("Щось не так із checkInternetConnection()")
      }
    })
}

function updateData(json) { // Update UI data
  const temperature = json.t
  const humidity = json.h
  const status = json.s
  const noise = json.n
  const light = json.l
}


$(document).ready(function() { // When document is ready, hide all pages except home one
  statsPage.hide()
  settingsPage.hide()
  homeButton.addClass("btn-info"); // Change button color
  fetchData()
});

statsButton.on("click", function() { // Add event listener to stats button
  homePage.hide() // Hide all pages except stats one
  settingsPage.hide()
  statsPage.show()
  statsButton.addClass("btn-info"); // Change button colors
  homeButton.removeClass("btn-info");
  settingsButton.removeClass("btn-info");
})

homeButton.on("click", function() { // Add event listener to home button
  settingsPage.hide() // Hide all pages except home one
  statsPage.hide()
  homePage.show()
  homeButton.addClass("btn-info"); // Change button colors
  settingsButton.removeClass("btn-info");
  statsButton.removeClass("btn-info");
})

settingsButton.on("click", function() { // Add event listener to settings button
  statsPage.hide() // Hide all pages except setting one
  homePage.hide()
  settingsPage.show()
  settingsButton.addClass("btn-info"); // Change button colors
  homeButton.removeClass("btn-info");
  statsButton.removeClass("btn-info");
})



const mainContainer = $("#main") // Get <main></main>

const statsPage = $("#stats-page") // Get statistics, home and settings pages
const homePage = $("#home-page")
const settingsPage = $("#settings-page")

const statsButton = $("#stats-button") // Get stats, home and settings buttons
const homeButton = $("#home-button")
const settingsButton = $("#settings-button")

const lightStatus = $("#light-status")
const espStartTime = $("#esp-start-time")
const tempLabel = $("#temp")
const humidityLabel = $("#humidity")
const lightLabel = $("#lightness")
const noiseLabel = $("#noise")


// function showLoading() {
//   $('#loadingOverlay').fadeIn(); // Плавно показуємо оверлей
//   console.log("Overlay faded IN");
// }

function hideLoading() {
  $('#loadingOverlay').fadeOut(); // Плавно приховуємо оверлей
  $("#loadingOverlay").hide()
  console.log("Overlay faded OUT");
}

function convertMillisecondsToTime(ms) {
  const hours = Math.floor(ms / (1000 * 60 * 60)); // Конвертуємо в години
  const minutes = Math.floor((ms % (1000 * 60 * 60)) / (1000 * 60)); // Залишок у хвилинах

  // Форматуємо години і хвилини як двозначні числа
  const formattedHours = String(hours).padStart(2, '0');
  const formattedMinutes = String(minutes).padStart(2, '0');

  return (formattedHours, formattedMinutes)
}

function fetchData() { // Fetch data from ESP32
  $.getJSON("http://192.168.1.103/", function (data) {
    updateData(data)
  })
  .fail(function (jqXHR, textStatus, errorThrown) { // If got error
    console.error("Помилка при підключенні до ESP32:", textStatus, errorThrown);
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
  .always(function() {
    hideLoading()
  });
}

function updateData(json) { // Update UI data
  const temperature = json.t
  const humidity = json.h
  const status = json.s
  const noise = json.n
  const light = json.l
  const time = json.o
  const timeTuple = convertMillisecondsToTime(time)
  const hours = timeTuple[0]
  const mins = timeTuple[1]

  status == "OK" ? lightStatus.text("Є світло") : lightStatus.text("Світла нема")
  espStartTime.text(`З'явилось ${hours} годин ${mins} хвилин тому.`)
  tempLabel.text(temperature + "C")
  humidityLabel.text(humidity + "%")
  lightLabel.text(light)
  noiseLabel.text(noise)

  console.log("Updated interface");
  
}


$(document).ready(function() { // When document is ready, hide all pages except home one
  // showLoading()
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



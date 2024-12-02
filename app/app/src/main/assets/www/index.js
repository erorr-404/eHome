const mainContainer = $("#main") // Get <main></main>

const statsPage = $("#stats-page") // Get statistics, home and settings pages
const homePage = $("#home-page")
const settingsPage = $("#settings-page")

const statsButton = $("#stats-button") // Get stats, home and settings buttons
const homeButton = $("#home-button")
const settingsButton = $("#settings-button")


$(document).ready(function() { // When document is ready, hide all pages except home one
  statsPage.hide()
  settingsPage.hide()
  homeButton.addClass("btn-info"); // Change button color
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


function fetchData() { // Fetch data from ESP32
  fetch("http://192.168.1.105/data")
    .then((response) => response.json())
    .then((json) => {
      console.log(json);
    })
}
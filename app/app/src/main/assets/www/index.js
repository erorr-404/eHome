
function fetchData() {
  fetch("http://192.168.1.105/data")
    .then((response) => response.json())
    .then((json) => {
      console.log(json);
    })
}
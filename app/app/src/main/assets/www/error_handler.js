// Перехоплення всіх помилок у JavaScript
window.onerror = function (message, source, lineno, colno, error) {
  const errorMessage = `Повідомлення: ${message}\nДжерело: ${source}\nРядок: ${lineno}, Колонка: ${colno}\nПомилка: ${error ? error.stack : 'N/A'}`;
  console.log(errorMessage);  // Для відображення у веб-консолі
  if (typeof AndroidLogger !== 'undefined') {
      AndroidLogger.logError(errorMessage); // Передача в LogCat
  }
};

// Тестове повідомлення про помилку
function generateError() {
  fetch("http://192.168.1.105/data").catch(err => {
      throw new Error("CORS-запит не вдався: " + err.message);
  });
}
from machine import Pin, Timer, ADC
import usocket as socket
from time import sleep, time
from dht import DHT11
import json
import network


with open("config.json") as f: # Load program config
    config = json.load(f)

dht_pin = Pin(5, Pin.IN) # Pin for DHT11
sound_a_pin = Pin(32, Pin.IN) # Sound Analog pin
light_pin = Pin(33, Pin.IN) # Light sensor pin
led = Pin(2, Pin.OUT) # Led pin

dht = DHT11(dht_pin) # DHT11 instance

sound_a = ADC(sound_a_pin) # Sound pin ADC intsnce
sound_a.atten(ADC.ATTN_11DB) # Full range: 3.3v

light_adc = ADC(light_pin) # Light ADC instance
light_adc.width(ADC.WIDTH_12BIT)
light_adc.atten(ADC.ATTN_11DB)

measurement_timer = Timer(0) # Timer for regular temperature and humidity measurement
time_init = time() # Time when booted up


def get_temp_and_hum(d: DHT11): # Measures temp and hum and returns them
    try:
        d.measure()
        t, h = d.temperature(), d.humidity()
    except OSError:
        print("Cant read temperature and humidity")
        t, h = 0, 0
    
    return t, h

def get_sound(adc: ADC): # Get the sound value
    try:
        sound = adc.read()
    except OSError:
        print("Cant read from sound sensor")
        sound = 0
    
    return sound

def get_light(adc: ADC): # Get light
    try:
        light = adc.read()
    except OSError:
        light = 0
    
    return light

def get_light_status(value: int): # Get the string about light
    description = ""
    # We'll have a few thresholds, qualitatively determined
    if value < 41:
        description = "Dark"
    elif value < 819:
        description = "Dim"
    elif value < 2048:
        description = "Light"
    elif value < 3277:
        description = "Bright"
    else:
        description = "Very bright"
    return description

def measurement_callback(timer): # This function runs every second
    t, h = get_temp_and_hum(dht)
    s = get_sound(sound_a)
    l = get_light(light_adc)
    print(f"Temp: {t} Hum: {h} Sound: {s} Light: {l}")
        
# Make previous function run every second
measurement_timer.init(period=config["meassurement_interval"], mode=Timer.PERIODIC, callback=measurement_callback)

station = network.WLAN(network.STA_IF)

print(f"Connecting to: {config["ssid"]}, {config["password"]}...")

station.active(True)
station.connect(config["ssid"], config["password"])

while station.isconnected() == False:
    led.value(not led.value())  # Перемикаємо стан LED
    sleep(0.2)  # Мерехтіння кожні 200 мс

led.on()
print('Connection successful')
print(station.ifconfig())

# Створення сокета
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))  # Прив'язуємо до порту 80
s.listen(5)       # Максимум 5 підключень у черзі
print("Server is ready. Waiting for requests on port 80...")

while True:
    conn, addr = s.accept()
    print('Got a connection from %s' % str(addr))
    
    # Отримання HTTP-запиту
    request = conn.recv(1024).decode('utf-8')
    print("Запит:", request)
    
    # Розбір HTTP-запиту
    method, _, _ = request.partition('\r\n')  # Зчитуємо перший рядок запиту
    method = method.split(' ')[0]  # Визначаємо метод (GET, POST, OPTIONS тощо)
    
    # Формування відповіді
    if method == 'OPTIONS':  # Обробка preflight-запиту
        response = (
            "HTTP/1.1 204 No Content\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Connection: close\r\n\r\n"
        )
        conn.send(response)
    else:  # Основний обробник запитів
        t, h = get_temp_and_hum(dht)
        response_body = json.dumps({
            "t": t,
            "h": h,
            "s": "OK",
            "n": get_sound(sound_a),
            "l": get_light(light_adc),
            "o": time() - time_init
        })
        
        response = (
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Access-Control-Allow-Origin: *\r\n"  # Дозволяємо всі домени
            "Connection: close\r\n\r\n" +
            response_body
        )
        conn.send(response)
    
    # Закриваємо з'єднання
    conn.close()
    
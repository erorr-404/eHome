from machine import Pin, Timer, ADC
from time import sleep
from dht import DHT11
import json
import network


with open("config.json") as f: # Load program config
    config = json.load(f)

dht_pin = Pin(5, Pin.IN) # Pin for DHT11
led = Pin(2, Pin.OUT) # Pin for built-in LED
sound_a_pin = Pin(4, Pin.IN) # Sound Analog pin
sound_d_pin = Pin(19, Pin.IN) # Sound Digital pin
light_pin = Pin(15, Pin.IN) # Light sensor pin

dht = DHT11(dht_pin) # DHT11 instance

sound_a = ADC(sound_a_pin) # Sound pin ADC intsnce
sound_a.atten(ADC.ATTN_11DB) # Full range: 3.3v

light_adc = ADC(light_pin) # Light ADC instance
light_adc.width(ADC.WIDTH_12BIT)
light_adc.atten(ADC.ATTN_11DB)

wlan = network.WLAN(network.STA_IF) # Create WLAN instance
wlan.active(True)

while not wlan.isconnected():
    led.on()
    try:
        wlan.connect(config["ssid"], config["password"])
    except OSError as error:
        led.off()
        print(f'ERROR: {error}')
        break
    
    sleep(1)
    led.off()

led.on()
sleep(5)
print(f"Connected to {config['ssid']}!")
print(f"IP: {wlan.ifconfig()}")


measurement_timer = Timer(0) # Timer for regular temperature and humidity measurement

def get_temp_and_hum(d: DHT11): # Measures temp and hum and returns them
    d.measure()
    return d.temperature(), d.humidity()

def get_sound(adc: ADC): # Get the sound value
    sound = adc.read()
    return sound

def get_light(adc: ADC): # Get light
    light = adc.read()
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
#measurement_timer.init(period=1000, mode=Timer.PERIODIC,callback=measurement_callback)

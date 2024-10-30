from machine import Pin, Timer

led = Pin(15, Pin.OUT)
button = Pin(14, Pin.IN, Pin.PULLDOWN)

while True:
    if button.value():
        led.toggle()
        time.sleep(0.5)

placa:
    esp32 devkit1
wifi :
    - el esp32 creara un accesspoit para recuperar credesioales con un captive portal
    - usa el display para indicar los pasos a seguir al usuario

screen:
    oled 128x64 --> i2c

sensors:
    BME280 --> i2c

i2c connection:
    gpio22--> SDA
    gpio21 --> SCL

objetivos del proyecto:
    
    - Implementar en el display una interfaz de usuario moderna con fuentes y gráficos atractivos y modernos.
    - Traducir todo el texto del display a italiano.
    - cuando este conectado al wifi, muestra la direccion ip local y publica
    - Mostrar mensajes de error en el display si ocurren problemas con el sensor.
    - No cambies nada en otras carpetas que tengan otros projectos.
    - Puedes instalar librerias o aplicaciones.
    - una vez conectado a internet, deve mostrar en el display:
        - hora y fecha
        - ciudad, en este caso ciro marina
        - estado meteorologico (ejemplo: despejado, nublado, etc) con un icono, temperatura, humedad y presion atmosferica
        - el meteo lo determina conectandose a una api gratuita, sin api key o similar
        - mostrara tambien los datos locales determinados por el bme280
        - mostrara los datos locales y de ciro marina con una animacion, puedes preguntar cual seria la mejor, si tienes alguna idea
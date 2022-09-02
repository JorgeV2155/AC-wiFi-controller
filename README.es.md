# AC-wiFi-controller
Controlador vía WiFi para tu AC basado en Arduino que te permite controlar tu AC Daikin con tu móvil o incluso con un asistente como siri
Este _README_ está disponible también en [Inglés](README.md)
## INFORMACIÓN

El programa añadido es válido para su uso con los aires acondicionados Daikin, serie ARC.
<br />
<br />
Este es el mando que está usando actualmente para su A/C:<br /><br /><br />
![Mando del aire acondicionado](assets/images/RemoteControlARC.jpg)
## CIRCUITO

### COMPONENTES

- Placa **Arduino Mega**
- Módulo **ESP8266**
- Diodo emisor de **infrarrojos**
- Resistencia 220Ω
- Cables<br /><br /><br />

![Circuito de arduino](assets/images/Arduino_circuit.png)

## PROGRAMA DE ARDUINO

El programa de Arduino se encuentra disponible bajo el nombre de [_ACController_finalVersion.ino_](/Arduino/ACController_finalVersion/ACController_finalVersion.ino)

El programa usa la siguiente librería:

-	[DYIRDaikin.h](https://github.com/danny-source/Arduino_DY_IRDaikin)

Explicación de este:

En su setup() se encarga de:

- Iniciar la librería del aire acondicionado
- Iniciar Serial para poder recibir información de la placa esp8266

En el loop() se encarga de:

- Ir recibiendo los datos enviados por la placa esp8266 e ir analizándolos:
  - Va carácter por carácter identificando si están las letras que le indican que parámetros debe enviar al aire acondicionado. Si identifica una de ellas lo que hace es guardar el siguiente número para, posteriormente enviarlo al aire acondicionado
- Hay un apartado en donde se identifica si ha escrito ready el módulo
  - Este código sirve por si el módulo tiene un error por cualquier cosa como puede ser sobrecalentamiento, excesivas peticiones, error en una petición…
  - El módulo se reinicia y cuando está listo indica ready; El programa lo detecta y envía los parámetros requeridos para que el módulo pueda ser usado como servidor web y reciba las peticiones que se le envían

La solicitud http se envía a la dirección que el modulo te entregue al poner AT+CIPSTA, seguido de _:_ y el puerto que usted haya indicado en el comando AT+CIPMUX=1,x (siendo x el puerto indicado)

Ejemplo:

http://192.168.1.50:85/_p0_t20_m1_f1_s0

las variables se añaden precedidas de un \_ y seguidas por su valor.

### POSIBLES VALORES

- TIEMPO (c) [OPCIONAL]: Esto nos permite enviar órdenes al arduino que serán cumplidas no en ese instante sino pasado el tiempo especificado. ¿Cómo se especifica el tiempo?: El tiempo nos lo indican los numeros que van después de la letra c. El primer numero (caracter) nos indica el numero de caracteres que el numero a continuación tendrá. El número a continuación (Que tendrá x caracteres, siendo x el número indicado anteriormente) es la cantidad en segundos que el programa debe esperar.
- ENCENDIDO/APAGADO (p): 0 off, 1 on
- TEMPERATURA (t): Un número entero en el rango de temperatura soportado por tu A/C
- MODO (m): 0=FAN, 1=COOL, 2=DRY, 3=HEAT(if you have one)
- VENTILADOR (f): 0~4=velocidad(1,2,3,4,5),5=auto,6=moon(modo noche)
- OSCILACIÓN (s): 0 off, 1 on



## APLICACIÓN EN REACT NATIVE

He desarrollado una aplicación en React native que permite el control del aire a través de esta aplicación móvil, válida para dispositivos Android & iOS

El código fuente está incluido en este github y necesita una serie de librerías para funcionar:

- React-navigation (v6)
  - @react-navigation/bottom-tabs
  - @react-navigation/native
- React-native-circle-slider
- React-native-elements
- react-native-webview
- Some Icons:
  - FontAwesome
  - MaterialCommunityIcons
  - Feather
  - EvilIcons
  - Ionicons

La aplicación ha sido creada usando expo-cli
![Imagen de la app](assets/images/App_image.png)

Escribidme si teneis algún problema, duda o sugerencia

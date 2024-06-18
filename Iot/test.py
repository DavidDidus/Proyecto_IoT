import datetime
import paho.mqtt.client as mqtt
import psycopg2

# Umbral de humedad para decidir el riego
THRESHOLD_VALUE = 100

def iniciarConexion():
    parametros = {
        "host": "localhost",
        "port": "5432",
        "user": "postgres",
        "password": "postgres",
        "database": "postgres"
    }
    global conexion
    try:
        conexion = psycopg2.connect(**parametros)
        print("Conectado")
    except (Exception, psycopg2.Error) as error:
        print("Error al conectar con la base de datos", error)
    

# Función de callback para la conexión
def on_connect(client, userdata, flags, rc):
    print("Conectado con código de resultado " + str(rc))
    client.subscribe("test_invernadero/topic")

# Función de callback para la recepción de mensajes
def on_message(client, userdata, msg):
    print(f"Mensaje recibido en tema {msg.topic}: {msg.payload.decode()}" + str(datetime.datetime.now()))

    # Insertar datos en la base de datos
    try:
        insertarRegistro(msg)
    except (Exception, psycopg2.Error) as error:
        print("Error al insertar en la base de datos", error)

    # Proceso de decisión de riego basado en el valor del sensor
    try:
        sensor_value = int(msg.payload.decode())
        if sensor_value >= THRESHOLD_VALUE:
            send_watering_instruction(client, True)
        else:
            send_watering_instruction(client, False)
    except ValueError:
        print("Error al convertir el valor del sensor")

def insertarRegistro(msg):
    cursor = conexion.cursor()
    cursor.execute("INSERT INTO registro_test(registro) values ('Presionado " + msg.payload.decode() + " " + str(datetime.datetime.now()) + "')")
    conexion.commit()
    cursor.close()

def verRegistros():
    cursor = conexion.cursor()
    cursor.execute("SELECT * FROM registro_test")
    registros = cursor.fetchall()
    for registro in registros:
        print(registro)
    cursor.close()

def send_watering_instruction(client, should_water):
    if should_water:
        message = "REGAR"
        print("Instrucción: Regar la planta")
    else:
        message = "NO_REGAR"
        print("Instrucción: No regar la planta")

    client.publish("test_invernadero/instrucciones", message)

iniciarConexion()
# Configurar el cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conectar al broker MQTT
client.connect("test.mosquitto.org", 1883, 60)

verRegistros()

# Bucle de red
client.loop_forever()


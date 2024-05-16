import paho.mqtt.client as mqtt

# Función de callback para la conexión
def on_connect(client, userdata, flags, rc):
    print("Conectado con código de resultado " + str(rc))
    client.subscribe("test_invernadero/topic")

# Función de callback para la recepción de mensajes
def on_message(client, userdata, msg):
    print(f"Mensaje recibido en tema {msg.topic}: {msg.payload.decode()}")

# Configurar el cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conectar al broker MQTT
client.connect("test.mosquitto.org", 1883, 60)

# Bucle de red
client.loop_forever()

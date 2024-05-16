import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Conectado al broker MQTT con código de resultado "+str(rc))
    client.subscribe("outTopic")

def on_message(client, userdata, msg):
    print("Mensaje recibido en el topic "+msg.topic+": "+str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set("IoT_Admin", "1234")
client.connect("172.20.10.12", 1883, 60)

client.loop_forever()
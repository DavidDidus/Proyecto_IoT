from datetime import datetime
from http import server

import dash
from dash import html, dcc
import plotly.graph_objs as go
from dash.dependencies import Input, Output
import paho.mqtt.client as mqtt
import threading
import json

from dash.exceptions import PreventUpdate

# Inicializar la aplicación Dash
app = dash.Dash(__name__)

# Almacenar datos recibidos
data_store = {
    "time": [],
    "humedad1": [],
    "humedad2": [],
    "humedad3": []
}

# Configuración del cliente MQTT
mqtt_broker = "test.mosquitto.org"
mqtt_port = 1883
mqtt_topic = "test_invernadero/topic"


def on_connect(client, userdata, flags, rc):
    print("Conectado al broker MQTT con código " + str(rc))
    client.subscribe(mqtt_topic)


def on_message(client, userdata, msg):
    global data_store
    message = msg.payload.decode()
    print("Mensaje recibido: " + message)

    # Parsear el mensaje JSON
    data = json.loads(message)
    humedad1 = data["humedad1"]
    humedad2 = data["humedad2"]
    humedad3 = data["humedad3"]

    current_time = datetime.now().strftime('%H:%M:%S')
    data_store["time"].append(current_time)
    data_store["humedad1"].append(humedad1)
    data_store["humedad2"].append(humedad2)
    data_store["humedad3"].append(humedad3)

    # Limitar el almacenamiento a 100 puntos de datos
    if len(data_store["time"]) > 100:
        data_store["time"].pop(0)
        data_store["humedad1"].pop(0)
        data_store["humedad2"].pop(0)
        data_store["humedad3"].pop(0)


# Configurar el cliente MQTT
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(mqtt_broker, mqtt_port, 60)

# Iniciar el cliente MQTT en un hilo separado
threading.Thread(target=mqtt_client.loop_forever).start()

app.layout = html.Div(style={"backgroundColor": "#000000"}, children=[
    dcc.Interval(id='interval-component', interval=10 * 1000, n_intervals=0),  # Intervalo de 10 segundos
    html.H1("Invernadero Dashboard", style={'textAlign': 'center', 'color': 'white'}),

    dcc.Tabs(id='tabs', value='tab-humedad1', children=[
        dcc.Tab(label='HUMEDAD1', value='tab-humedad1', style={'backgroundColor': '#000000', 'color': 'white'}),
        dcc.Tab(label='HUMEDAD2', value='tab-humedad2', style={'backgroundColor': '#000000', 'color': 'white'}),
        dcc.Tab(label='HUMEDAD3', value='tab-humedad3', style={'backgroundColor': '#000000', 'color': 'white'})
    ], style={'color': 'white'}),

    html.Div(id='tabs-content', style={'padding': '20px'})
])


@app.callback(
    Output(component_id='tabs-content', component_property='children'),
    [Input(component_id='tabs', component_property='value'),
     Input(component_id='interval-component', component_property='n_intervals')]
)

def render_content(tab, n_intervals):
    global data_store

    if n_intervals == 0:
        raise PreventUpdate

    if tab == 'tab-humedad1':
        return dcc.Graph(
            id='humedad1-graph',
            figure={
                'data': [go.Scatter(
                    x=data_store["time"],
                    y=data_store["humedad1"],
                    mode='lines+markers',
                    line=dict(color='red')
                )],
                'layout': go.Layout(
                    title='Humedad Espinaca',
                    xaxis={'title': 'Tiempo'},
                    yaxis={'title': 'Humedad del Suelo (%)'},
                    plot_bgcolor='white',
                    paper_bgcolor='white',
                    font=dict(color='black')
                )
            }
        )
    elif tab == 'tab-humedad2':
        return dcc.Graph(
            id='humedad2-graph',
            figure={
                'data': [go.Scatter(
                    x=data_store["time"],
                    y=data_store["humedad2"],
                    mode='lines+markers',
                    line=dict(color='green')
                )],
                'layout': go.Layout(
                    title='Humedad Lechuga',
                    xaxis={'title': 'Tiempo'},
                    yaxis={'title': 'Humedad del Suelo (%)'},
                    plot_bgcolor='white',
                    paper_bgcolor='white',
                    font=dict(color='black')
                )
            }
        )
    elif tab == 'tab-humedad3':
        return dcc.Graph(
            id='humedad3-graph',
            figure={
                'data': [go.Scatter(
                    x=data_store["time"],
                    y=data_store["humedad3"],
                    mode='lines+markers',
                    line=dict(color='blue')
                )],
                'layout': go.Layout(
                    title='Humedad Tomate',
                    xaxis={'title': 'Tiempo'},
                    yaxis={'title': 'Humedad del Suelo (%)'},
                    plot_bgcolor='white',
                    paper_bgcolor='white',
                    font=dict(color='black')
                )
            }
        )


if __name__ == "__main__":
    # Start the Dash server in the main thread
    app.run_server(debug=True, port=8050, use_reloader=False)

from flask import Flask, render_template, jsonify
import serial
import threading
import re
import time

app = Flask(__name__)

# =====================================================
# CONEXÃO COM O ARDUINO
# =====================================================

arduino = serial.Serial("COM3", 9600, timeout=1)

# Últimos valores recebidos
dados = {
    "sensor": 0,
    "luz": 0,
    "pwm": 0
}


# =====================================================
# LEITURA DA SERIAL
# =====================================================

def ler_arduino():

    time.sleep(2)

    while True:

        linha = arduino.readline().decode(
            "utf-8",
            errors="ignore"
        ).strip()

        if linha:

            print(linha)

            resultado = re.search(
                r"LDR:\s*(\d+)\s*\|\s*Luz:\s*([\d.]+)%\s*\|\s*PWM:\s*(\d+)",
                linha
            )

            if resultado:

                dados["sensor"] = int(resultado.group(1))
                dados["luz"] = float(resultado.group(2))
                dados["pwm"] = int(resultado.group(3))


# =====================================================
# PÁGINA PRINCIPAL
# =====================================================

@app.route("/")
def pagina_principal():

    return render_template("index.html")


# =====================================================
# API DOS DADOS
# =====================================================

@app.route("/dados")
def enviar_dados():

    return jsonify(dados)


# =====================================================
# INICIA LEITURA DO ARDUINO
# =====================================================

thread = threading.Thread(
    target=ler_arduino,
    daemon=True
)

thread.start()


# =====================================================
# EXECUTA O SERVIDOR
# =====================================================

if __name__ == "__main__":

    app.run(
        host="127.0.0.1",
        port=5000,
        debug=False
    )

from flask import Flask, render_template, jsonify
import serial
import threading
import time

app = Flask(
    __name__,
    template_folder="Templates",
    static_folder="Static"
)

# Dados recebidos do Arduino
dados = {
    "sensor": 0.0,
    "luz": 0.0,
    "pwm": 0
}

lock = threading.Lock()


def ler_arduino():

    while True:
        try:
            with serial.Serial("COM3", 9600, timeout=1) as arduino:

                print("Arduino conectado na COM3")
                time.sleep(2)

                while True:

                    linha = arduino.readline().decode(
                        "utf-8",
                        errors="ignore"
                    ).strip()

                    if not linha:
                        continue

                    try:
                        lux_texto, pwm_texto = linha.split(",")

                        lux = float(lux_texto)
                        pwm = int(pwm_texto)

                        with lock:
                            dados["sensor"] = lux
                            dados["luz"] = lux
                            dados["pwm"] = pwm

                        print(
                            f"Recebido: Lux={lux:.1f} | PWM={pwm}"
                        )

                    except (ValueError, TypeError):
                        print("Linha ignorada:", linha)

        except serial.SerialException as erro:

            print("Erro serial:", erro)
            time.sleep(3)


@app.route("/")
def pagina_principal():
    return render_template("index.html")


@app.route("/dados")
def enviar_dados():

    with lock:
        return jsonify(dados.copy())


if __name__ == "__main__":

    thread = threading.Thread(
        target=ler_arduino,
        daemon=True
    )

    thread.start()

    app.run(
        host="127.0.0.1",
        port=5000,
        debug=False
    )

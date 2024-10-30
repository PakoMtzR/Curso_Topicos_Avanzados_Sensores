import customtkinter as ctk
import serial
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Configurar el puerto serie
SERIAL_PORT = 'COM6'  # Cambia esto según tu puerto serie
BAUD_RATE = 115200

# Configurar la interfaz de customtkinter
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

class SensorBMP180App(ctk.CTk):
    APP_NAME = "BMP180 Dashboard"
    WIDTH = 850
    HEIGHT = 600
    YLIMIT_INF = 2450
    YLIMIT_SUP = 2480

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.title(SensorBMP180App.APP_NAME)
        self.geometry(f"{SensorBMP180App.WIDTH}x{SensorBMP180App.HEIGHT}")
        self.minsize(SensorBMP180App.WIDTH, SensorBMP180App.HEIGHT)

        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.grid_columnconfigure(0, weight=3)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        # Iniciar la bandera de ejecución del hilo
        self.running = True

        self.frame_data = ctk.CTkFrame(master=self, corner_radius=0)
        self.frame_data.grid(row=0, column=1, padx=0, pady=0, sticky="nsew")

        self.temp_label = ctk.CTkLabel(self.frame_data, font=("consolas", 20), text="Temperature[°C]: --")
        self.temp_label.pack(pady=(80, 10))

        self.pressure_label = ctk.CTkLabel(self.frame_data, font=("consolas", 20), text="Pressure[mbar]: --")
        self.pressure_label.pack(pady=10)

        self.altitude_label = ctk.CTkLabel(self.frame_data, font=("consolas", 20), text="Altitude[m]: --")
        self.altitude_label.pack(pady=10)

        # Iniciar la conexión serial
        self.serial_esp = serial.Serial(SERIAL_PORT, BAUD_RATE)

        # Iniciar el hilo de lectura del puerto serie
        self.serial_thread = threading.Thread(target=self.read_serial)
        self.serial_thread.daemon = True
        self.serial_thread.start()

        # Configurar la gráfica de altitud
        self.fig, self.ax = plt.subplots()
        self.altitude_data = []
        self.line, = self.ax.plot(self.altitude_data, label="Altitude [m]")
        self.ax.set_ylim(self.YLIMIT_INF, self.YLIMIT_SUP)  # Ajusta el rango según tus necesidades
        self.ax.set_title("Altitud sensor BMP180")
        self.ax.set_xlabel("Tiempo")
        self.ax.set_ylabel("Altitud [m]")

        # Agregar la gráfica a la interfaz
        self.canvas = FigureCanvasTkAgg(self.fig, master=self)
        self.canvas.get_tk_widget().grid(row=0, column=0, padx=40, pady=40, sticky="nsew")

        # Configurar la animación de la gráfica
        self.ani = animation.FuncAnimation(self.fig, self.update_plot, interval=1000, save_count=10)


    def read_serial(self):
        while self.running:
            try:
                data = self.serial_esp.readline().decode('utf-8').strip().split(',')
                print(data)  # Para depuración

                if len(data) == 3:
                    temp, pressure, altitude = data
                    self.temp_label.configure(text=f"Temperature[°C]: {temp}")
                    self.pressure_label.configure(text=f"Pressure[mbar]: {pressure}")
                    self.altitude_label.configure(text=f"Altitude[m]: {altitude}")

                    self.altitude_data.append(float(altitude))
                    if len(self.altitude_data) > 100:
                        self.altitude_data.pop(0)  # Mantener solo los últimos 100 datos
            except Exception as e:
                if self.running:  # Solo mostrar el error si el programa sigue en ejecución
                    print(f"Error reading serial data: {e}")

    def update_plot(self, frame):
        self.line.set_ydata(self.altitude_data)
        self.line.set_xdata(range(len(self.altitude_data)))
        self.ax.relim()
        self.ax.autoscale_view()
        self.canvas.draw()

    def on_closing(self):
        self.running = False  # Detener el hilo de lectura del puerto serie
        self.serial_esp.close()  # Cerrar la conexión serial
        self.quit()  # Utiliza quit en lugar de destroy para terminar el loop de tkinter


    def start(self):
        self.mainloop()

if __name__ == "__main__":
    app = SensorBMP180App()
    app.start()

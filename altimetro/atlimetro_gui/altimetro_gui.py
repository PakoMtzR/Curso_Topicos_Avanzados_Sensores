import tkinter as tk
import customtkinter as ctk
import serial
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# configureurar el puerto serie
SERIAL_PORT = 'COM6'  # Cambia esto según tu puerto serie
BAUD_RATE = 115200

class SensorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Sensor Data Display")

        # configureurar la interfaz de customtkinter
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("blue")

        self.frame = ctk.CTkFrame(root)
        self.frame.pack(pady=20, padx=20, fill="both", expand=True)

        self.temp_label = ctk.CTkLabel(self.frame, text="Temp[C]: --")
        self.temp_label.pack(pady=10)

        self.pressure_label = ctk.CTkLabel(self.frame, text="Pressure[mbar]: --")
        self.pressure_label.pack(pady=10)

        self.altitude_label = ctk.CTkLabel(self.frame, text="Altitude[m]: --")
        self.altitude_label.pack(pady=10)

        # Iniciar la bandera de ejecución del hilo
        self.running = True

        # Iniciar el hilo de lectura del puerto serie
        self.serial_thread = threading.Thread(target=self.read_serial)
        self.serial_thread.daemon = True
        self.serial_thread.start()

        # configureurar la gráfica de altitud
        self.fig, self.ax = plt.subplots()
        self.altitude_data = []
        self.line, = self.ax.plot(self.altitude_data, label="Altitude [m]")
        self.ax.set_ylim(2300, 2400)  # Ajusta el rango según tus necesidades
        self.ax.set_title("Altitud en Tiempo Real")
        self.ax.set_xlabel("Tiempo")
        self.ax.set_ylabel("Altitud [m]")

        # Agregar el lienzo de la gráfica a la interfaz
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.frame)
        self.canvas.get_tk_widget().pack(pady=10)

        # configureurar la animación de la gráfica
        self.ani = animation.FuncAnimation(self.fig, self.update_plot, interval=1000)

        # Manejar el cierre de la ventana
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

    def read_serial(self):
        with serial.Serial(SERIAL_PORT, BAUD_RATE) as ser:
            while self.running:
                try:
                    line = ser.readline().decode('utf-8').strip()
                    print(line)  # Para depuración
                    if "Temp[C]:" in line:
                        temp = line.split(": ")[1]
                        self.temp_label.configure(text=f"Temp[C]: {temp}")
                    elif "Pressure[mbar]:" in line:
                        pressure = line.split(": ")[1]
                        self.pressure_label.configure(text=f"Pressure[mbar]: {pressure}")
                    elif "Altitude[m]:" in line:
                        altitude = float(line.split(": ")[1])
                        self.altitude_label.configure(text=f"Altitude[m]: {altitude}")
                        self.altitude_data.append(altitude)
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
        self.running = False
        self.serial_thread.join()  # Esperar a que el hilo termine
        self.root.quit()  # Utiliza quit en lugar de destroy para terminar el loop de tkinter
        self.root.update()  # Asegúrate de actualizar la ventana para procesar cualquier evento pendiente

# Ejecutar la aplicación
if __name__ == "__main__":
    root = ctk.CTk()
    app = SensorApp(root)
    root.mainloop()

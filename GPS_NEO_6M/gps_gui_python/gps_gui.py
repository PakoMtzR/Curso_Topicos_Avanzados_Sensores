import customtkinter
from tkintermapview import TkinterMapView
import serial
import serial.tools.list_ports
import threading

customtkinter.set_default_color_theme("blue")

class App(customtkinter.CTk):

    APP_NAME = "GPS app"
    WIDTH = 850
    HEIGHT = 600

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.title(App.APP_NAME)
        self.geometry(str(App.WIDTH) + "x" + str(App.HEIGHT))
        self.minsize(App.WIDTH, App.HEIGHT)

        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.bind("<Command-q>", self.on_closing)
        self.bind("<Command-w>", self.on_closing)
        self.createcommand('tk::mac::Quit', self.on_closing)

        self.marker_list = []
        self.serial_connection = None  # Para almacenar el objeto de conexión serial
        self.closing = False  # Bandera para indicar si la ventana se está cerrando

        # ============ create two CTkFrames ============

        self.grid_columnconfigure(0, weight=0)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self.frame_left = customtkinter.CTkFrame(master=self, width=150, corner_radius=0, fg_color=None)
        self.frame_left.grid(row=0, column=0, padx=0, pady=0, sticky="nsew")

        self.frame_right = customtkinter.CTkFrame(master=self, corner_radius=0)
        self.frame_right.grid(row=0, column=1, rowspan=1, pady=0, padx=0, sticky="nsew")

        # ============ frame_left ============
        self.frame_left.grid_rowconfigure(12, weight=1)

        self.baud_rate_label = customtkinter.CTkLabel(self.frame_left, text="Baud Rate:", anchor="w")
        self.baud_rate_label.grid(row=0, column=0, padx=(20, 20), pady=(10, 0))
        self.baud_rate_option_menu = customtkinter.CTkOptionMenu(self.frame_left, values=["9600", "115200"])
        self.baud_rate_option_menu.grid(row=1, column=0, padx=(20, 20))

        self.ports_label = customtkinter.CTkLabel(self.frame_left, text="Ports:", anchor="w")
        self.ports_label.grid(row=2, column=0, padx=(20, 20), pady=(10, 0))
        self.ports_option_menu = customtkinter.CTkOptionMenu(self.frame_left)
        self.ports_option_menu.grid(row=3, column=0, padx=(20, 20))

        self.scan_ports_button = customtkinter.CTkButton(self.frame_left, text="Scan Ports", command=self.scan_ports)
        self.scan_ports_button.grid(row=4, column=0, padx=(20, 20), pady=(10, 0))

        self.connect_button = customtkinter.CTkButton(self.frame_left, text="Connect", fg_color="green", command=self.toggle_connection)
        self.connect_button.grid(row=5, column=0, padx=(20, 20), pady=(10, 0))

        self.date_label = customtkinter.CTkLabel(self.frame_left, text="[DATE]:", anchor="w")
        self.date_label.grid(row=7, column=0, padx=(20, 20), pady=(10, 0))

        self.time_label = customtkinter.CTkLabel(self.frame_left, text="[TIME]:", anchor="w")
        self.time_label.grid(row=8, column=0, padx=(20, 20))

        self.lat_label = customtkinter.CTkLabel(self.frame_left, text="[LAT]:", anchor="w")
        self.lat_label.grid(row=9, column=0, padx=(20, 20))

        self.lon_label = customtkinter.CTkLabel(self.frame_left, text="[LON]:", anchor="w")
        self.lon_label.grid(row=10, column=0, padx=(20, 20))

        self.speed_label = customtkinter.CTkLabel(self.frame_left, text="[SPEED]:", anchor="w")
        self.speed_label.grid(row=11, column=0, padx=(20, 20))
        
        self.map_label = customtkinter.CTkLabel(self.frame_left, text="Tile Server:", anchor="w")
        self.map_label.grid(row=13, column=0, padx=(20, 20), pady=(20, 0))
        self.map_option_menu = customtkinter.CTkOptionMenu(self.frame_left, values=["OpenStreetMap", "Google normal", "Google satellite"], command=self.change_map)
        self.map_option_menu.grid(row=14, column=0, padx=(20, 20))

        self.appearance_mode_label = customtkinter.CTkLabel(self.frame_left, text="Appearance Mode:", anchor="w")
        self.appearance_mode_label.grid(row=15, column=0, padx=(20, 20), pady=(10, 0))
        self.appearance_mode_optionemenu = customtkinter.CTkOptionMenu(self.frame_left, values=["Light", "Dark", "System"], command=self.change_appearance_mode)
        self.appearance_mode_optionemenu.grid(row=16, column=0, padx=(20, 20), pady=(0, 20))

        # ============ frame_right ============
        self.frame_right.grid_rowconfigure(0, weight=1)
        self.frame_right.grid_columnconfigure(0, weight=1)

        self.map_widget = TkinterMapView(self.frame_right, corner_radius=0)
        self.map_widget.grid(row=0, column=0, sticky="nswe", padx=(0, 0), pady=(0, 0))

        # Set default values
        self.scan_ports()
        #self.map_widget.set_address("Berlin")
        self.map_widget.set_position(20.118806, -98.801628)
        self.map_widget.set_zoom(12)
        self.map_option_menu.set("OpenStreetMap")
        self.appearance_mode_optionemenu.set("Dark")

    def scan_ports(self):
        # Obtener una lista actualizada de puertos COM disponibles
        available_ports = [port.device for port in serial.tools.list_ports.comports()]
        # Limpiar el menú de opciones
        self.ports_option_menu.destroy()
        # Recrear el menú de opciones con los puertos disponibles
        self.ports_option_menu = customtkinter.CTkOptionMenu(self.frame_left, values=available_ports)
        self.ports_option_menu.grid(row=3, column=0, padx=(20, 20))

    def toggle_connection(self):
        if self.serial_connection is None:
            # No hay una conexión serial, intenta establecer una
            port = self.ports_option_menu.get()
            baudrate = self.baud_rate_option_menu.get()
            try:
                self.serial_connection = serial.Serial(port, baudrate)
                self.connect_button.configure(text="Disconnect", fg_color="red")
                print("Conexión establecida.")
                # Iniciar un subproceso para leer del puerto serial
                threading.Thread(target=self.read_serial).start()
            except serial.SerialException as e:
                print(f"Error al conectar: {e}")
        else:
            # Hay una conexión serial, ciérrala
            if self.serial_connection.is_open:  # Agregamos este condicional para verificar si la conexión está abierta antes de intentar cerrarla
                self.serial_connection.close()
                self.serial_connection = None
                self.connect_button.configure(text="Connect", fg_color="green")
                print("Conexión cerrada.")
    
    def read_serial(self):
        while self.serial_connection is not None:
            data = self.serial_connection.readline().decode('utf-8').strip().split(',')
            print(data)
            self.date_label.configure(text=f"[DATE]: {data[0]}")
            self.time_label.configure(text=f"[TIME]: {data[1]}")
            self.lat_label.configure(text=f"[LAT]: {data[2]}")
            self.lon_label.configure(text=f"[LON]: {data[3]}")
            self.speed_label.configure(text=f"[SPEED]: {data[4]}")
            self.clear_marker()
            if ((data[2] != "0.000000") and (data[3] != "0.000000")):
                self.set_marker(float(data[2]), float(data[3]))
            if self.closing:
                break

    def set_marker(self, lat, lon):
        #current_position = self.map_widget.get_position()
        self.marker_list.append(self.map_widget.set_marker(lat, lon))   # lon+0.0001

    def clear_marker(self):
        for marker in self.marker_list:
            marker.delete()

    def change_appearance_mode(self, new_appearance_mode: str):
        customtkinter.set_appearance_mode(new_appearance_mode)

    def change_map(self, new_map: str):
        if new_map == "OpenStreetMap":
            self.map_widget.set_tile_server("https://a.tile.openstreetmap.org/{z}/{x}/{y}.png")
        elif new_map == "Google normal":
            self.map_widget.set_tile_server("https://mt0.google.com/vt/lyrs=m&hl=en&x={x}&y={y}&z={z}&s=Ga", max_zoom=22)
        elif new_map == "Google satellite":
            self.map_widget.set_tile_server("https://mt0.google.com/vt/lyrs=s&hl=en&x={x}&y={y}&z={z}&s=Ga", max_zoom=22)

    def on_closing(self, event=0):
        self.closing = True  # Establecer la bandera de cierre
        if self.serial_connection and self.serial_connection.is_open:  # Verificamos si hay una conexión serial y si está abierta antes de intentar cerrarla
            self.serial_connection.close()
        self.destroy()

    def start(self):
        self.mainloop()

if __name__ == "__main__":
    app = App()
    app.start()
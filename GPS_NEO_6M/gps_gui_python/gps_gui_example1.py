import tkinter

# Crear objeto Ventana
window = tkinter.Tk()

# Configuramos el tamaño de la ventana ("XxY")
window.geometry("500x200")

# Configuramos titulo de la ventana
title = tkinter.Label(window, text="hola jotos")
title.pack()

# Mostrar la ventana
window.mainloop()
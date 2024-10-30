clc, clear, close all;

% Establecemos la conexion serial
serialPort = 'COM10';
baudRate = 9600;
s = serialport(serialPort, baudRate);

% Grafico de la brujula
figure;                                         % Creamos la figura 
Marco = polarplot(0, 0, '-r', 'LineWidth', 2);  % Grafica tipo polar para simular una brujula
title('Brujula (Sensor LSM303DLHC)');           % Configuramos el titulo de la grafica
rlim([0 1]);                                    % limites de la grafica de 0 a 1
hold on;                                        

% Leer y graficar los datoS
while true
    % Verificamos si existen datos disponibles en el puerto serial
    if s.NumBytesAvailable > 0
        % Leemos los datos y convertimos a valor numerico
        data = readline(s);
        Grados = str2double(data);
        % Si no existen datos nulos, calculamos el angulo
        if ~isnan(Grados)
            angles = [Grados, 0, 0];                                 % Orientación en ángulos de Euler
            quats = quaternion(angles, 'eulerd', 'ZYX', 'frame');   % Calculamos los cuarterniones
            C = rad2deg(compassAngle(quats));                       % Obtener el ángulo de la brújula
            disp(C);                                                % Mostramos el angulo en la ventana de comando
            angle_rad = deg2rad(C);                                 % Conversion a radianes
            set(Marco, 'ThetaData', [0 angle_rad], 'RData', [0 1]); % Dibujamos flecha
            drawnow;   
        end
    end
    pause(0.1); % Frecuencia de actualizacion
end
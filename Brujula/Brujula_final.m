clc, close, clear all;

try
    s = serialport("COM6", 9600);
catch ME
    disp("Error al conectar al puerto serial: " + ME.message);
    return;
end

configureTerminator(s, "LF");
flush(s);   

% Inicializar la figura
figure;
title('Visualización 3D con Logo de MATLAB');
xlabel('X');
ylabel('Y');
zlabel('Z');
hold on;
grid on;
axis equal;

[X, Y, Z] = peaks(25); % Generar la superficie del logo de MATLAB
surfPlot = surf(X, Y, Z);
view(3); % Establecer la vista 3D

while true
    try
        if s.NumBytesAvailable > 0
            % Leer datos del puerto serial
            data = readline(s);
            values = str2double(split(data, ','));

            if length(values) == 6
                % Separar los valores del acelerómetro y magnetómetro
                accel_x = values(1);
                accel_y = values(2);
                accel_z = values(3);
                mag_x = values(4);
                mag_y = values(5);
                mag_z = values(6);

                % Calcular la inclinación
                pitch = atan2(-accel_x, sqrt(accel_y^2 + accel_z^2));
                roll = atan2(accel_y, accel_z);
                yaw = compassAngle([mag_x, mag_y, mag_z], [accel_x, accel_y, accel_z]);

                % Calcular cuaterniones
                cy = cos(yaw * 0.5);
                sy = sin(yaw * 0.5);
                cp = cos(pitch * 0.5);
                sp = sin(pitch * 0.5);
                cr = cos(roll * 0.5);
                sr = sin(roll * 0.5);

                q0 = cr * cp * cy + sr * sp * sy;
                q1 = sr * cp * cy - cr * sp * sy;
                q2 = cr * sp * cy + sr * cp * sy;
                q3 = cr * cp * sy - sr * sp * cy;

                % Crear la matriz de rotación a partir de los cuaterniones
                rotationMatrix = quat2rotm([q0, q1, q2, q3]);

                % Aplicar la rotación a los puntos de la superficie
                newXYZ = rotationMatrix * [X(:)'; Y(:)'; Z(:)'];
                newX = reshape(newXYZ(1, :), size(X));
                newY = reshape(newXYZ(2, :), size(Y));
                newZ = reshape(newXYZ(3, :), size(Z));

                % Actualizar la superficie en la figura
                set(surfPlot, 'XData', newX, 'YData', newY, 'ZData', newZ);
                drawnow;
            end
        end
    catch ME
        disp("Error al leer datos del puerto serial: " + ME.message);
    end
    pause(0.1); % Pausa para que no se trabe
end
clear s;

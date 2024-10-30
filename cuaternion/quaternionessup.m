% Configuración del puerto serial
try
    s = serialport("COM7", 9600);
catch ME
    disp("Error al conectar al puerto serial: " + ME.message);
    return;
end

configureTerminator(s, "LF");
flush(s);


[X, Y, Z] = peaks(25);% Generar la superficie del logo de MATLAB
surfPlot = surf(X, Y, Z);
axis([-10 10 -10 10 -10 10]);
hold on;
grid on;
xlabel('X');
ylabel('Y');
zlabel('Z');
title('Visualización 3D del Giroscopio con Logo de MATLAB');
view(3); % Establecer la vista 3D

% interfaz con gráfica 3D
while true
    try
        if s.NumBytesAvailable > 0
            % Leer datos del puerto serial
            data = readline(s);
            aag = str2double(split(data, ','));

            if length(aag) == 3
                % guardar por separado los angulos dados por el gyro
                % (programa gyro 3/8 de arduino)
                ang_x = aag(1);
                ang_y = aag(2);
                ang_z = aag(3);

                % conversion a rad
                ang_x = deg2rad(ang_x);
                ang_y = deg2rad(ang_y);
                ang_z = deg2rad(ang_z);

                % Calculo de cuaterniones con angulos del gyro 3/8 arduino
                % pero con formula de robotica col pues da con la formula
                % larga
                q0 = cos(ang_x/2) * cos(ang_y/2) * cos(ang_z/2) + sin(ang_x/2) * sin(ang_y/2) * sin(ang_z/2);
                q1 = sin(ang_x/2) * cos(ang_y/2) * cos(ang_z/2) - cos(ang_x/2) * sin(ang_y/2) * sin(ang_z/2);
                q2 = cos(ang_x/2) * sin(ang_y/2) * cos(ang_z/2) + sin(ang_x/2) * cos(ang_y/2) * sin(ang_z/2);
                q3 = cos(ang_x/2) * cos(ang_y/2) * sin(ang_z/2) - sin(ang_x/2) * sin(ang_y/2) * cos(ang_z/2);

                % Calcular los ángulos de Euler a partir de los cuaterniones
                yaw = atan2(2*(q0*q3 + q1*q2), 1 - 2*(q2*q2 + q3*q3));
                pitch = asin(2*(q0*q2 - q3*q1));
                roll = atan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2));

                % Convertir los ángulos de Euler a grados
                yaw = rad2deg(yaw);
                pitch = rad2deg(pitch);
                roll = rad2deg(roll);

                % Crear la matriz de rotación a partir de los ángulos de Euler
                rotationMatrix = eul2rotm(deg2rad([yaw, pitch, roll]), 'ZYX');

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

    pause(0.01); % Pausa para que no se trabe
end


clear s;

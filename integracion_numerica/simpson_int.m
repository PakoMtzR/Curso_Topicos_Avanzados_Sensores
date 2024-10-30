clc, clear, close all;

f = @(x) (2+sin(2*(x.^0.5)));
a = 1;
b = 6;
n = 1;

h = (b - a) / (2*n);
x = a:h:b;

% Evaluamos la función en los puntos x
y = f(x);

if n ~= 1
    % Calculamos la suma de los puntos interiores multiplicados por 4
    sum1 = sum(y(2:2:end-1));
    
    % Calculamos la suma de los puntos interiores multiplicados por 2
    sum2 = sum(y(3:2:end-2));
    
    % Aplicamos la fórmula de Simpson 1/3
    I = (h/3) * (y(1) + 4*sum1 + 2*sum2 + y(end));
else
    I = (h/3)*(y(1) + 4*y(2) + y(3));
end

disp(I)

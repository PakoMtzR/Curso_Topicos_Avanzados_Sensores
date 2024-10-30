function I = bart_simpson_un_tercio(f, a, b, n)
    % f: Función a integrar
    % a, b: Límites de integración (a-inferior, b-superior) 
    % n: Número de sub-intervalos

    h = (b - a) / (2*n);
    x = a:h:b;

    % Evaluamos la función en los puntos x
    y = f(x);

    % Calculamos la suma de los puntos interiores multiplicados por 4
    sum1 = sum(y(2:2:end-1));

    % Calculamos la suma de los puntos interiores multiplicados por 2
    sum2 = sum(y(3:2:end-2));

    % Aplicamos la fórmula de Simpson 1/3
    I = (h/3) * (y(1) + 4*sum1 + 2*sum2 + y(end));
end
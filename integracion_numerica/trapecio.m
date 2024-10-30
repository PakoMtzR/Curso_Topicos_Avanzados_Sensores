function F = trapecio(f,a,b,n)
% f = @(x) (2+sin(2*(x.^0.5)));
% a = 1;
% b = 6;
% n = 1;

% f: Función a integrar
% a, b: Límites de integración (a-inferior, b-superior) 
% n: Número de sub-intervalos

h = (b - a) / n;
i = 0;
F = 0;

for k = 1:n
    F = F + (h/2)*(f(a+h*i)+f(a+h*(i+1)));
    i = i+1;
end
end
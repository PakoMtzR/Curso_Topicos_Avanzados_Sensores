function F = simpson_13(f,a,b,n)
% f: Función a integrar
% a, b: Límites de integración (a-inferior, b-superior) 
% n: Número de sub-intervalos

h = (b - a) / (2*n);
i = 0;
F = 0;

for k = 1:n
    F = F + (h/3)*(f(a+h*i)+4*f(a+h*(i+1))+f(a+h*(i+2)));
    i = i+2;
end
end
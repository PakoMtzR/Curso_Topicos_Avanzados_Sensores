function F = simpson_38(f,a,b,n)
% f: Función a integrar
% a, b: Límites de integración (a-inferior, b-superior) 
% n: Número de sub-intervalos

h = (b - a) / (3*n);
i = 0;
F = 0;

for k = 1:n
    F = F + ((3*h)/8)*(f(a+h*i)+3*f(a+h*(i+1))+3*f(a+h*(i+2))+f(a+h*(i+3)));
    i = i+3;
end
end
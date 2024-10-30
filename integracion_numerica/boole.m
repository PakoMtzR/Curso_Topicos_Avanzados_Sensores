function F = boole(f,a,b,n)
% f: Función a integrar
% a, b: Límites de integración (a-inferior, b-superior) 
% n: Número de sub-intervalos

h = (b - a) / (4*n);
i = 0;
F = 0;

for k = 1:n
    F = F + ((2*h)/45)*(7*f(a+h*i)+32*f(a+h*(i+1))+12*f(a+h*(i+2))+32*f(a+h*(i+3))+7*f(a+h*(i+4)));
    i = i+4;
end
end
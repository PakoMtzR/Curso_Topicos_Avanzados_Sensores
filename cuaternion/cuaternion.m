clc, clear all, close all; 
s = serialport("/dev/cu.usbserial-8330",9600);
 n=350;
 uno=zeros(n,3);
 for i=1:n
data = readline(s);
uno(i,:)=str2num(data);
 end
% dos=[];
% tres=[]; 
%plot3(uno(:,1),uno(:,2),uno(:,3))
ang_yz=zeros(length(uno))
ang_xz=zeros(length(uno))
ang_xy=zeros(length(uno))
ang_xz = atand(uno(:,1)./uno(:,3))
ang_yz = atand(uno(:,2)./uno(:,3))
ang_xy = atand(uno(:,2)./uno(:,1))
if(ang_xz < 0) 
    ang_xz = ang_xz+ 360; 
end
if(ang_yz < 0) 
    ang_yz = ang_yz+360; 
end
if(ang_xy < 0) 
    ang_xy = ang_xy+360; 
end
%https://es.wikipedia.org/wiki/Formalización_de_la_rotación_en_tres_dimensiones
mag=sqrt((uno(:,1)).^2+(uno(:,2)).^2+(uno(:,3)).^2)
bx=uno(:,1)./mag; 
by=uno(:,2)./mag; 
bz=uno(:,3)./mag; 
q0=cosd(ang_xz/2);
q1=(sind(ang_xz/2)).*cosd(bx); 
q2=(sind(ang_yz/2)).*cosd(by); 
q3=(sind(ang_xy/2)).*cosd(bz); 
roll= atand((q0 .* q1 + q2 .* q3)/( 0.5 - (q1 .* q1 + q2 .* q3)));
pitch = asind(2.0 * (q0 .* q2 - q1 .* q3));
hold on 
plot(pitch)
plot(roll)
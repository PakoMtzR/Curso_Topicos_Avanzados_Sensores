clc, clear, close all;

s = serialport("COM6", 9600);
data = zeros(30,5);

for i = 1:30
    data(i,:) = str2num(readline(s));
end
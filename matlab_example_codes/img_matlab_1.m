clc, clear, close all;

img = imread("D:\UPIIH\Semestre_8\_Sistemas_Vision_Artificial\Recursos\gato_soldado.jpg");
img(20:40,40:60,:) = 0;
A = rgb2gray(img);
imshow(A)
% imshow(img)
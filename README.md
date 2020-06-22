# Seam-Carving
Implemented Seam Carving Technique using dynamic programming.

Programming Language : C++

Library Used : OpenCV

## Introduction

This project is implemented to retargeting the image to pase the content of image in a different size(smaller size). This process is achieved by avoiding non-uniform scaling to prevent properties of objects in scene and cropping of the content. 

[https://en.wikipedia.org/wiki/Seam_carving]

## Seam Carving

The idea of seam carving is to select horizontal or vertical seam that is a prixel thick cut through the image. Each seam will be from right to left or top to bottom through the image and after cut down the selected seam, image size(height or width) will reduce by one pixel. The aim to select most optimal seam is that the seam should be through the non-salient content of the image.

## Run the project

**There are four arguments**

1. Input file
2. Target width
3. Target height
4. Output file

```
g++ -o main main.cpp
g++ -o sc sc.cpp
./sc castle.jpg 1000 600 output.jpg
```

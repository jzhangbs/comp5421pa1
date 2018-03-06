# COMP5421 PA1
This project is for HKUST COMP5421 Project 1. It implements a simplified version of the system described in *Intelligent Scissors for Image Composition* by Eric Mortensen and William Barrett, published in the proceedings of SIGGRAPH 1995.

## Build
This project can be built automatically by Qt Creator. It was tested on Windows with VS2017 and Mac.

This project depends on OpenCV 3.4. Change the directories of dependencies in `comp5421pa1.pro`.

## How to use
Choose `File -> Open` to open an image.
Press `Zoom in`, `Zoom out` to zoom.

![](https://i.imgur.com/fM1gwalm.png)
![](https://i.imgur.com/GRxzPbum.png)

Click on the image to place the first seed.

![](https://i.imgur.com/Ckvb155m.png)

Keep click on the image to place the following seeds.

![](https://i.imgur.com/Yj4YABrm.png)

Click at the first seed to finish the contour. When the cursor is close to the first seed, it will anchor to the seed.

![](https://i.imgur.com/3nRmbDkm.png)

After completing a contour, click on the image to start the next contour.

![](https://i.imgur.com/fBcOiyim.png)

When dragging the cursor to draw, press `Esc` to
1. cancel drawing if the seed the first one.
2. set seed to the previous one otherwise.

When not drawing a contour, press `Esc` to delete the previous contour.

Press `Pixel node`, `Cost graph`, `Path tree` to visualize the computation.

![](https://i.imgur.com/9BOrpyKm.png)
![](https://i.imgur.com/meZW6wGm.png)
![](https://i.imgur.com/Nkc7UDgm.png)

Press `Seed snap` to enable seed snapping. When setting a seed, program will instead set the seed to a near point with large gradient. Press `Gradient` to show the gradient map.

![](https://i.imgur.com/yhrAmY1m.png)
![](https://i.imgur.com/hZFptprm.png)
![](https://i.imgur.com/uSwX7G6m.png)

Choose `File -> Save with contour` to save the image with contours.

Choose `File -> Save mask` to save the mask.

Choose `File -> Save with alpha channel` to save a PNG file with mask applied.

## Result
1. COMP 5421 Logo

![](https://i.imgur.com/NfhM0PFm.png)
![](https://i.imgur.com/wYAF7uxm.png)
![](https://i.imgur.com/IevRVqfm.png)

2. Lenna

![](https://i.imgur.com/IxrYx8mm.png)
![](https://i.imgur.com/4gTsnezm.png)
![](https://i.imgur.com/ntsMmzam.png)

3. Avater

![](https://i.imgur.com/v4mMDlNm.png)
![](https://i.imgur.com/YD3v7Rvm.png)
![](https://i.imgur.com/5inl2k5m.png)

## Contribution
Luqi Wang and Jingyang Zhang contributed equally to this project.

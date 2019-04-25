%main tomatos
clc
clear all
close all

I_color=imread('assorted_tomatoes.jpg');
% I_color=rgb2hsv(I_color);
%find grayscale image
I_gray=rgb2gray(I_color);
[hight, width]=size(I_gray);
%perform edge detection with Canny,do it separately for Grayscale image,Red channel,
%Green channel & Blue channel
I_edge=edge(I_gray,'Canny',[0.05,0.15]);
I_edgeRed=edge(I_color(:,:,1),'Canny',[0.05,0.15]);
I_edgeGreen=edge(I_color(:,:,2),'Canny',[0.05,0.15]);
I_edgeBlue=edge(I_color(:,:,3),'Canny',[0.05,0.15]);
%enhance edges with dilation (connectivity 4)
SE=[0 1 0; 1 1 1; 0 1 0];
I_edge = imdilate(I_edge,SE);
I_edgeRed = imdilate(I_edgeRed,SE);
I_edgeGreen = imdilate(I_edgeGreen,SE);
I_edgeBlue = imdilate(I_edgeBlue,SE);


%find circles in edge images: Grayscale,Red, Green & Blue
%channels, get circles centers and radiuses
[centersE,radiiE] = imfindcircles(I_edge,[30 200],'ObjectPolarity','dark','Sensitivity',0.9);%max(hight, width)]);
[centersR,radiiR] = imfindcircles(I_edgeRed,[30 200],'ObjectPolarity','dark','Sensitivity',0.91);
[centersG,radiiG] = imfindcircles(I_edgeGreen,[30 200],'ObjectPolarity','dark','Sensitivity',0.915);
[centersB,radiiB] = imfindcircles(I_edgeBlue,[30 200],'Sensitivity',0.9);

%Group the circles from all different edge images, and take care of
%overlappings
%first initialize by checking for overlaps in grayscale image circles
[centersE,radiiE]=Add_new_circles([],[],centersE,radiiE,I_color);
%add non-overlaping circles from all color edge images
[centers,radii]=Add_new_circles(centersE,radiiE,centersR,radiiR,I_color);
[centers,radii]=Add_new_circles(centers,radii,centersG,radiiG,I_color);
[centers,radii]=Add_new_circles(centers,radii,centersB,radiiB,I_color);

%for each circle find average color of its area in original image
colors=find_circle_color(I_color,centers,radii);

%Define Reference colors
Colors_label = {'Red'; 'Brown'; 'Green'; 'Yelow'};
Ref_Colors=[200,40,40; 140,42,42; 130,160,60; 200,155,20];

%Classify by matching each color in 'colors' to Reference colors
color_match = classify_color(Ref_Colors,colors);

%plot Graph of tomatoes color vs. radius
figure;
plot(color_match,radii,'o')
set(gca,'xtick',[1:4],'xticklabel',Colors_label)
title('Tomatoe color vs. radius - Without clustering')

%show final result with all circles and classifications
figure;
imshow(I_color);
hold on
viscircles(centers,radii,'EdgeColor','b');title(['Final Result- ' num2str(length(radii)) ' tomatoes found']);
hold on
for i=1:length(color_match)
    text(centers(i,1),centers(i,2),Colors_label(color_match(i)));
    text(centers(i,1)-25,centers(i,2)+15,['(',num2str(round(colors(i,1))),',',num2str(round(colors(i,2))),',',num2str(round(colors(i,3))),')']);
    hold on
end

%2. classify after clustering
%Perform clustering on colors of circles with k-means (k=4 in our case)
[idx,centroids] = kmeans(colors,4);
%Classify color by matching clusters centeroids to Reference colors
color_match = classify_color(Ref_Colors,centroids);
%correct class list
for i=1:length(idx)
    idx(i)=color_match(idx(i));
end
%Graph of tomatoes color vs. radius
figure;
plot(idx,radii,'o')
set(gca,'xtick',[1:4],'xticklabel',Colors_label)
title('Tomatoe color vs. radius - Using clustering')
figure;
imshow(I_color);
hold on
viscircles(centers,radii,'EdgeColor','b');title('Final Result');
hold on
for i=1:length(idx)
    text(centers(i,1),centers(i,2),Colors_label(idx(i)));
    text(centers(i,1)-25,centers(i,2)+15,['(',num2str(round(colors(i,1))),',',num2str(round(colors(i,2))),',',num2str(round(colors(i,3))),')']);
    hold on
end
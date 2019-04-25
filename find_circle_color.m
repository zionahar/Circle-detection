 function colors=find_circle_color(I,centers,radii)
%find_circle_color(I,centers,radii) Returns a list of average colors of circled
%areas in image I according to circles center coordinates and radiuses

%initialize
Number_of_circles=length(radii);
colors=zeros(Number_of_circles,3);
[hight, width]=size(I(:,:,1));
for i=1:Number_of_circles%for each circle
    %Get a binary circle image 
    [Binary_circle,num_pixels_in_tomato]=draw_white_circle(centers(i,:),radii(i),hight,width);
    %multiply binary tomato with original image per color channel
    single_tomato_image(:,:,1)=Binary_circle.*double(I(:,:,1));
    single_tomato_image(:,:,2)=Binary_circle.*double(I(:,:,2));
    single_tomato_image(:,:,3)=Binary_circle.*double(I(:,:,3));
    %for each color channel find average value inside the colored circle 
    colors(i,:)=sum(sum(single_tomato_image))./num_pixels_in_tomato;
    
    
end
end

function [Binary_circle,num_pixels_in_tomato]=draw_white_circle(center,radius,hight,width)
%Returns a binary image with size hight X width X 3 , for each color channel all pixels inside of a
%the circle specified with its center coordinats and radius are equal 1 and
%0 otherwise
Binary_circle=zeros(hight,width);
num_pixels_in_tomato=0;
x=1:1:width;
y=1:1:hight;
X=repmat((x-center(1)).^2,hight,1);
Y=repmat((y'-center(2)).^2,1,width);
Binary_circle(X+Y<(radius^2))=1;
num_pixels_in_tomato=numel(find(Binary_circle));
% for i=1:hight
%     for j=1:width
%         if ((j-center(1))^2+(i-center(2))^2<radius^2)
%             Binary_circle(i,j)=1;%fill with 1 each color channel if we inside the circle
%             num_pixels_in_tomato=num_pixels_in_tomato+1;
%         end
%     end
% end
end
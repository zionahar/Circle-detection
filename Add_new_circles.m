function [centers,radii]=Add_new_circles(centersA,radiiA,centersB,radiiB,I)
%adds non-overlapping circles parameters from list B to list A
if(isempty(radiiA))%initialize if we receive an empty list 
    if(~isempty(radiiB))
        radiiA(1,:)=radiiB(1,:);%copy first circle parameters to A
        centersA(1,:)=centersB(1,:);
    else%both lists are empty
        msg='empty input arguments';
        error(msg);
    end
end
if(isrow(radiiA))
    radiiA=radiiA';
end
max_color_dist=40;
Max_circle_distance=10; 
Max_radius_distance=20;
A_length=length(radiiA);
B_length=length(radiiB);
%Initialize output lists
centers=centersA;
radii=radiiA;
added_circles=0;
for i=1:B_length%for each circle in list B
    if(isrow(radii))
        radii=radii';
    end
    move_on=0;
    %find distance between the center of the candidate circle to all
    %current circle centers on list A
    xdist=centers(:,1)-centersB(i,1);
    ydist=centers(:,2)-centersB(i,2);
    center_distances=sqrt(xdist.^2+ydist.^2);
    %same for radius distances
    rad_distances=abs(radii-radiiB(i));
    %look for overlapings (we consider overlap when distance between centers<max(R_a,R_b))
    overlap_center_index=find(center_distances<max(radii,radiiB(i)));
    if(~isempty(overlap_center_index))%we found center overlap
       
        %check if one of the overlapping circles in the list is very similar to
       %the the new circle -  if so, dont add the circle and move to next circle in B 
        for j=1:length(overlap_center_index)
            if (center_distances(overlap_center_index(j))<Max_circle_distance && rad_distances(overlap_center_index(j))<Max_radius_distance)
                move_on=1;
            end
        end
        
         %check total overlap - if one of the circles in overlaping_list is containing the circle from B or
         %containd by him (it happens when d<|R_a-R_b| - leave only the
         %largest
         if(~move_on)
             for j=1:length(overlap_center_index)
                 if(center_distances(overlap_center_index(j))<rad_distances(overlap_center_index(j)))
                     move_on=1;
                     if(radii(overlap_center_index(j))<radiiB(i))%if the circle in B is larger
                         centers(overlap_center_index(j),:)=centersB(i,:);%Replace circle A with B
                         radii(overlap_center_index(j))=radiiB(i);
                     end
                 end
             end
         end
        
         %overlapping is not total but large enough it could be an elipce
        if(~move_on)
            D=0;
            for j=1:length(overlap_center_index) %for each overlaping circle
                %check average color inside the two circles.
                centers_to_check=[centers(overlap_center_index(j),:);centersB(i,:)];
                radiuses_to_check=[radii(overlap_center_index(j));radiiB(i)];
                colors=find_circle_color(I,centers_to_check,radiuses_to_check);
                % calculate L2 distance between the two average colors
                D(j) = sqrt(sum((colors(1,:) - colors(2,:)) .^ 2));
            end
            if (min(D)<max_color_dist) %the two overlaping circles has similar color - we decide its elipce
                %average the radius and centers
                idx=find(min(D));
                radii(overlap_center_index(idx))=mean([radii(overlap_center_index(idx)) radiiB(i)]);
                centers(overlap_center_index(idx),:)=(centers(overlap_center_index(idx),:)+centersB(i,:))./2;
            else %overlaping circles are with different colors - add the new circle to the list
                added_circles=added_circles+1;
                centers(A_length+added_circles,:)=centersB(i,:);
                radii(A_length+added_circles)=radiiB(i);
            end
        end
    else
        %There is no overlap at all so we can add the new circle to the list
        added_circles=added_circles+1;
        centers(A_length+added_circles,:)=centersB(i,:);
        radii(A_length+added_circles)=radiiB(i);
    end
end
end
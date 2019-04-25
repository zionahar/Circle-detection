function Idx = classify_color(Ref_Colors,colors_list)
%Returns the matching index in Ref_Colors for each color in colors_list 
Num_of_colors=length(colors_list);
Num_of_ref_colors=length(Ref_Colors);
D=zeros(length(colors_list),length(Ref_Colors));
Idx=zeros(length(colors_list),1);
for i=1:Num_of_colors
    for j=1:Num_of_ref_colors
        %calculate L2 norm distances between each color to each reference color
        D(i,j) = sqrt(sum((colors_list(i,:) - Ref_Colors(j,:)) .^ 2));
    end
    %Classify according to the minimum norm distance
    Idx(i)=find(D(i,:)==min(D(i,:)));
end


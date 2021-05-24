function compressedImage = read_circle(filename)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    imageID = fopen(filename);
    compressedImage = fread(imageID,'uint8');
    fclose(imageID);
end
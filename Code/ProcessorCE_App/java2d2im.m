function matImg = java2d2im(BufferedImage, type)
    %UNTITLED Summary of this function goes here
    %   Detailed explanation goes here
    
    nrows = BufferedImage.getHeight;
    ncols = BufferedImage.getWidth;
    if (type == 0)
        matImg = BufferedImage.getData.getPixels(0,0,ncols,nrows-1,[]);
        matImg = uint8(reshape(matImg,nrows-1,ncols)');
    else
        data = reshape(typecast(BufferedImage.getData.getDataStorage, 'uint8'), [], ncols, nrows);
        matImg = permute(data,[3 2 1]);
    end
end


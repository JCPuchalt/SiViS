function img = read_img(filename, width, height)
    compressedImage = read_circle(filename);
    img = decompressGray(compressedImage, width, height);
end


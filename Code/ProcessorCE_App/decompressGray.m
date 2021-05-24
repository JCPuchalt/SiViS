function img = decompressGray(compressedImage, width, height)
    img = im2uint8(mat2gray(vec2mat(decompressImage(compressedImage, width, height), width)));
end
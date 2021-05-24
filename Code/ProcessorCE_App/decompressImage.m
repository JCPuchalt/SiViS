% cmpr extension is 
function [Image] = decompressImage(compressedImage, width,height)
    
    r=height/2;  %972;
    y0=height/2; %972;
    k2=1;
    x0=width/2;
    r2=r*r;
    NUM = y0-r;
    
    if(NUM<0)
        NUM=0;
    end
    
    i1 = NUM*width;
    if (i1==0)
        i1=1;
    end
    
    %imagen_rectangular=zeros(width,height);
    for j=1:1:(height-1)
        xs=floor(x0-abs(sqrt(r2-(r-j)^2)));
        cx=floor(abs(sqrt(r2 - (r-j)^2)));
        NUM=j*width;
        N1=NUM+width;
        XN=NUM+xs;
        Xc=XN+2*cx;

        Image(i1:XN-i1) = 255;
        i1 = XN -1;
        Image(i1:i1+Xc-XN) = compressedImage(k2:k2+Xc-XN);
        i1=i1+Xc-XN;
        k2=k2+Xc-XN;
        Image(i1:N1-i1) = 255;
        i1 = N1 - 1;
    end
    
end
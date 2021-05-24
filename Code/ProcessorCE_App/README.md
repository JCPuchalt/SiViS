# ProcessorCE_App:
ProcessorCE_App is installed and running on Desktop Computer

# Requirements:
This demo was tested in Windows 10 with Matlab R2018b and Java V8 update 241
- Windows 10
- Matlab R2018b
- Java V8 update 241

# Clone lifespan repository:
```
git clone https://github.com/AntonioJoseSanchezSalmeron/lifespan.git
```

# Windows x64 installation:
- Run ```install.bat``` to configure properly library paths.

# Download images dataset:
This dataset includes all the images and the annotations for one assay (named Lifespan18). These images, saved in *.cmpr format, can be read by calling the function ```img = read_img(filename, width, height)```, which is included in this repository (see ```read_img.m``` macro). The amount of alive *C. elegans* per plate is annotated in the "conteoManual.xml" files.
- Download [Lifespan18.zip (8.53 GB)](https://active-vision.ai2.upv.es/wp-content/uploads/2020/01/Lifespan18.zip)
- Uncompress Lifespan18.zip
- Move uncompressed Lifespan18 folder into lifespan-mater folder, before running the ```lifespan.m``` macro.

# Run the automated lifespan demo in Matlab:
- Run ```lifespan.m``` macro. This macro will plot the final lifespan curves (one per condition) and will generate and save all the results. After running this macro, you can explore the results navigating to each plate folder in Lifespan18. Intermediate image processing results are saved in *.bmp and *.jpg format files and the automatic amounts of alive *C. elegans* per plate are recorded in the "conteoAutomatic.xml" files.

# Download demo results:
In case you don't want to spent time generating the intermediate image processing results, you can download these results and run ```postproces_Results.m``` macro to plot the final lifespan curves.
- Download [Lifespan18_Results.zip (0.17 GB)](https://active-vision.ai2.upv.es/wp-content/uploads/2020/01/Lifespan18_Results.zip)
- Uncompress Lifespan18_Results.zip
- Move uncompressed Lifespan18_Results folder into lifespan-mater folder, before running the ```postproces_Results.m``` macro to plot the final lifespan curves.


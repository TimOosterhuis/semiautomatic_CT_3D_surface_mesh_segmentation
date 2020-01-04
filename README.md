# Semi-automatic CT 3D surface mesh segmentation


## Background

This Semi-automatic CT scan surface mesh segmentation application was developed as part of my Master Thesis reasearch: "Fractured hip bone CT scan segmentation". 
The goal of this thesis research was to speed up the process of CT scan surface mesh segmentation for complex fractures, as manual fracture segment segmentation is time consuming.
To this end we designed a semi-automatic surface region growing lebeling method with manual seed point placement. 
The region growing algorithm uses mesh traversal with an added constraint on the maximum allowed angle between normals of adjacent vertices, so that the seeded regions only propagate through smooth surfaces. 
The mesh traversal based region growing algorithm has been embedded into this interactive bone CT scan 
ling application.

This application has also been used for research into the possibility of fast semi-automatic CT scan surface mesh segmentation of individual vertebrae in neck surgery planning. 

## Use instructions

To run the application start the executable in the build directory (SegmentationApp.exe).

The application consists of a window where the surface mesh of a CT scan can be displayed with its bone segments labeled with different colors.
To load a CT scan, select 'load scan' and browse to a directory containing the DICOM files you want to load.
For labeling the regions, there is a drop-down menu displaying the bone regions with their colors, with an option to add a new region. 
This menu is used to select regions for seed point placing. After selecting a region color, place seedpoints with the mouse. To grow the regions, select 'Extend Regions' to do a single growing step, or 'To Max' to grow regions automatically until there are no more unassigned vertices in the mesh.
In selection mode, clicking with the mouse places a seed point for the currently selected region. To correct a small error resulting from the region growing there is an option to toggle to eraser mode. In eraser mode, a click with the mouse clears the region labeling of a small area around the selected point. There is also an option to clear the currently selected region entirely or to clear the entire mesh. 
Track ball rotation and camera panning are possible in the mesh window, so that the mesh can be viewed by the user from all angles. 
Each bone region can also be set to be invisible. Only the visible regions are displayed. This allows the user to view the segmentation of parts of the mesh that are occluded by other parts, such as the socket of the hip bone. 
Lastly, it is possible to load another scan into the application without closing it or to change the Hounsfield threshold value and reload the current scan. 
The bone segmentation labelings are exported as individual meshes for each segmented fragment stored as wavefront (.obj) files. 

## Build instructions from source

It is also possible to rebuild the application from source, using QT-Creator. The included 'segmentation_app.pro' makefile is set up to be compiled with MSVC2017 64-bit in profile mode (not debug).  
Building this program from source with the 'segmentation_app.pro' makefile requires Windows and at least Qt version 4.0 or newer, and I recommend the version which I used which is Qt version 5.10.1. 


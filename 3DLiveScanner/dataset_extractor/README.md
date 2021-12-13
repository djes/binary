I tested it on Ubuntu but it should work on any Linux distribution. Here are commands I used to make it work (you have to change the paths):
```
sudo apt-get install cmake libpng-dev libturbojpeg-dev libglm-dev
cd /home/lubos/dataset_extractor/
cmake .
make
./dataset_extractor /home/lubos/dataset
```

It will generate a floorplan and create a PLY file for every frame (these files are very accurate), in the end program puts all frames together into output.ply file (there is accuracy lower because every frame has slightly different coordinate system). 

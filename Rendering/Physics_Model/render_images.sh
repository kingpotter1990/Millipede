# make sure there exit 3 folders: sim, render, temp

# bash render_images.sh | xargs -n 9 -P 32 echo
# bash render_images.sh | xargs -n 9 -P 32 time

max=93

for i in `seq 1 $max`
do
    F=`printf "%05d" $i`; 

sed "s/@@@/sim\/FRAME_$i.inc/" < scene.pov > temp/scene$i.pov

echo povray temp/scene$i.pov +Orender/$F.png +WT1 Antialias=On Display=Off +W800 +H600 +a0.0

done




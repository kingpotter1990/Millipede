

# make sure there exit 3 folders: sim, render, temp

# bash render_images.sh | xargs -n 9 -P 32 echo
# bash render_images.sh | xargs -n 9 -P 32 time

max=1500
min=1

for i in `seq $min $max`
do
    F=`printf "%08d" $i`; 

    sed "s/@@@/sim\/FRAME_$i.inc/" < scene.pov > temp/scene$i.pov

    echo povray temp/scene$i.pov +Orender/$F.png +WT1 Antialias=On Display=Off +W1600 +H1200 +a0.0

done




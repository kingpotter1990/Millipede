# make sure there exit 3 folders: sim, render, temp

min=1
max=93

minCX=2.2
minCY=3.3
minCZ=3.4
minLX=1.5
minLY=2.5
minLZ=3.5
minRX=1.7
minRY=2.7
minRZ=3.7

maxCX=3.2
maxCY=4.3
maxCZ=4.4
maxLX=2.5
maxLY=3.5
maxLZ=4.5
maxRX=2.7
maxRY=3.7
maxRZ=4.7


for i in `seq $min $max`
do
    a=`expr $i - $min`
    b=`expr $max - $min`
    let c=$(( $maxCX - $minCX ))
    let d=$(( $a * $c ))
    let e=$(( $d / $b ))

    echo $e
done




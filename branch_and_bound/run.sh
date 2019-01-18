
g++ $1

time=0
repeat=1000
for i in $(seq 0 $repeat);
do
    t=`./a.out | grep time | sed -n 's/Total time:\t//p' |sed -n 's/ ms//p'`
    time=`echo "scale=3; $time + $t" | bc`
done
echo "scale=6; $time / $repeat" | bc

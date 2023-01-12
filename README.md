steps for execution
connect to a certain site ex ssh lille.g5k
reserve a certain number of cores ex oarsub -I -l core=number
cd directory/to/scripts 
chmod +x script-gen.sh
./script-gen.sh linear/tree recordx

in local :
ssh site.g5k:directory/to/data .
cd directory/to/plots 
gnuplot gnuplot


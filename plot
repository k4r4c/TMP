#!/usr/bin/gnuplot --persist

message = "\
Usage: gnuplot -c plot <inputfolder> <client_timestamps> <server_timestamps> <outputfolder> <output_file_name> \n\
If one argument, default to: gnuplot -c plot <inputfolder> client_time server_time <inputfolder> time_graph\
"
if(ARGC < 1) print message; exit
inputfolder = ARG1
if(ARGC > 1 ) client = ARG2 ; else client = inputfolder."/client_time"
if(ARGC > 2 ) server = ARG3 ; else server = inputfolder."/server_time"
if(ARGC > 3 ) outputfolder = ARG4; else outputfolder=inputfolder
if(ARGC > 4 ) outut = ARG5; else output="time_graph"

print "Difference between ".client." and ".server.", output to ".inputfolder."/".output.".png"

#set term png
#set output inputfolder."/".output.".png"
unset label

# wxt
set terminal wxt size 350,262 enhanced font 'Verdana,10' persist
# png
#set terminal pngcairo size 350,262 enhanced font 'Verdana,10'
#set output 'absolute_data.png'

# color definitions
set border linewidth 1.5
set style line 1 lc rgb '#0060ad' pt 7 ps 1.5 lt 1 lw 2 # --- blue
unset key
# Axes
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror out scale 0.75
#    FIT_LIMIT  = 1e-6
#    f(x) = a*x + b
#    fit f(x) client  via a, b
##if(1==2){
#    g(x) = c*x + d
#    fit g(x) server via c, d
#}
set autoscale
set title "Jitter difference client-server"
plot '< paste '.client.' '.server u ($1-$2) with points ls 1; \
#plot '< paste '.client.' '.server every ::ARG3::ARG4 u ($1-$2) with points ls 1 
#plot '< paste '.client u ($1-a*$1+b) with points ls 1; \
#plot  f(x), client with points ls 1
#      g(x), 'server_time' with points ls 3 

#!/bin/bash


while [[ $# -gt 1 ]]
do
    key="$1"
    
case $key in
	-l|--low)
    LOW="$2"
    shift # past argument
    ;;
    -h|--high)
    HIGH="$2"
    shift # past argument
    ;;
    *)
            # unknown option
    ;;
esac
shift # past argument or value
done
echo LOW   = "${LOW}"
echo HIGH  = "${HIGH}"

if [[ -n $1 ]]; then
    echo "Last line of file specified as non-opt/last argument:"
    tail -1 $1
fi


for i in `seq $LOW $HIGH`;
do
    echo $i
    /afs/cern.ch/work/c/cpena/public/HggRazorLeptons/CMSSW_9_2_0_patch5/src/H4Analysis/bin/H4Reco cfg/H4Jun2017.cfg $i
done
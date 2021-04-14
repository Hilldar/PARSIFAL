#!/bin/sh

#################
# PARSIFAL:INIT #
#################

QUI=$PWD

if [ $# -eq 0 ]
then
    echo "Missing options!"
    echo "(run $0 -h for help)"
    echo ""
    exit 0
fi

##################
# PARSIFAL:BEGIN #
##################
start=`date +%s`
cd $PARSIFAL2
export TS_MAXCONN=10
TS_MAXCONN=10
ts -S 20
#################
# PARSIFAL:NAME #
#################
echo
echo " ###############"
echo " ## PARSIFAL2 ##"
echo " ###############"
echo

while getopts "MmehtSA" OPTION; do
    case $OPTION in
	e)
	    echo "Hello world"
	    ;;
	h)
            echo "Usage:"
            echo ""
	    echo "   -m     ->   make PARSIFAL2"
	    echo "   -M     ->   make clean all"
            echo "   -e     ->   to execute echo \"hello world\""
            echo "   -t     ->   run simulation test"
	    echo "   -S i   ->   run scan angle in i config"
	    echo "   -S i j ->   run i config and j angle (deg)"  
	    echo "   -A i   ->   analyze i config"
            ;;

	m)
	    make
	    ;;
	M)
	    make clean all
	    ;;
	t)
	    ./bin/Test
	    ;;
	S)
	    if [ -z $3 ]
	    then 
		./bin/Simulate $2
	    else
		if [ $2 -ge 0 ]
		then
                ./bin/Simulate $2 $3
		fi
	    fi
	    ;;
	A)
	    if [ $2 -ge 0 ]
	    then
		./bin/Analysis $2
		cat data/$2/summary.txt
	    fi
            ;;
    esac
done

######################
# PARSIFAL:TERMINATE #
######################
cd $QUI
echo "Duration: $((($(date +%s)-$start))) seconds"
echo

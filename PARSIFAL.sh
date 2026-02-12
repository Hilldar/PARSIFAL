#
# Authors:
# Riccardo Farinelli <rfarinelli@fe.infn.it>
# Lia Lavezzi        <lia.lavezzi@to.infn.it>
#
# All rights reserved
# For the licensing terms see $PARSIFAL/LICENSE
#
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
cd $PARSIFAL
#################
# PARSIFAL:NAME #
#################
echo
echo " ###############"
echo " ## PARSIFAL ##"
echo " ###############"
echo

while getopts "MmehSAO" OPTION; do
    case $OPTION in
	e)
	    echo "Hello world"
	    ;;
	h)
            echo "Usage:"
            echo ""
	    echo "   -m     ->   make PARSIFAL"
	    echo "   -M     ->   make clean all"
            echo "   -e     ->   to execute echo \"hello world\""
	    echo "   -S i   ->   run scan angle in i config"
	    echo "   -S i j ->   run i config and j angle (deg)"  
	    echo "   -A i   ->   analyze i config"
	    echo "   -O i j .>   open root file i config and j angle"
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
	    rm -r pdf/backup/*
	    mv pdf/* pdf/backup/.
	    if [ -z $3 ]
	    then 
		./bin/Simulate $2
		./bin/Analysis $2
	    else
		if [ $2 -ge 0 ]
		then
                    ./bin/Simulate $2 $3
		    ./bin/Analysis $2 $3
		fi
	    fi
	    ;;
	A)
	    if [ -z $3 ]
            then
                ./bin/Analysis $2
            else
                if [ $2 -ge 0 ]
                then
                    ./bin/Analysis $2 $3
                fi
            fi
	    echo
	    sed -n 1p run_list.txt; for i in {2..5}; do line=$(sed -n ${i}p run_list.txt); run=$(echo $line | sed 's/ .*//'); if [[ $2 = $run ]]; then sed -n ${i}p run_list.txt;fi; done
	    echo
            echo -e "Angle \t Qind MPV \t Qread \t \t RatioQ \t Size \t \t CCres \t \t TPCres \t T0res \t \t Eff"
            cat data/$2/summary.txt
            echo
            ;;
	O)
	    root -l data/$2/run_angle$3.root
    esac
done

######################
# PARSIFAL:TERMINATE #
######################
cd $QUI
echo "Duration: $((($(date +%s)-$start))) seconds"
echo

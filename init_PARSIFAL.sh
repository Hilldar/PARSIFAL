#!/bin/sh
bash_name="bash_PARSIFAL2"
echo "#!/bin/bash">$bash_name
echo "export PARSIFAL2_home=$PWD">>$bash_name
echo "export PARSIFAL2=$PARSIFAL2_home">>$bash_name
echo "export parsifal2=$PARSIFAL2">>$bash_name
echo "alias  PARSIFAL2='$PARSIFAL2_home/PARSIFAL2.sh'">>$bash_name
echo "alias  parsifal2='PARSIFAL2'">>$bash_name
echo "export PARSIFAL2_data=$PWD">>$bash_name
echo "export exe_parsifal2=$PWD/PARSIFAL2.sh">>$bash_name
echo "function cd_parsifal(){cd $PARSIFAL2;}">>$bash_name

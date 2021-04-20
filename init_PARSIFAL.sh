#!/bin/sh
bash_name="bash_PARSIFAL2"
echo "#!/bin/bash">$bash_name
echo "export PARSIFAL2_home=$PWD">>$bash_name
echo "export PARSIFAL2=$PWD">>$bash_name
echo "export parsifal2=$PWD">>$bash_name
echo "alias  PARSIFAL2='$PWD/PARSIFAL2.sh'">>$bash_name
echo "alias  parsifal2='PARSIFAL2'">>$bash_name
echo "export PARSIFAL2_data=$PWD/data">>$bash_name
echo "export exe_parsifal2=$PWD/PARSIFAL2.sh">>$bash_name
echo "function cd_parsifal(){">>$bash_name
echo "cd $PARSIFAL2;">>$bash_name
echo "}">>$bash_name

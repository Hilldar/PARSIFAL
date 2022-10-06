#
# Authors:
# Riccardo Farinelli <rfarinelli@fe.infn.it>
# Lia Lavezzi        <lia.lavezzi@to.infn.it>
#
# All rights reserved
# For the licensing terms see $PARSIFAL/LICENSE
#
#!/bin/sh
bash_name="bash_PARSIFAL"
echo "#!/bin/bash">$bash_name
echo "export PARSIFAL_home=$PWD">>$bash_name
echo "export PARSIFAL=$PWD">>$bash_name
echo "export parsifal=$PWD">>$bash_name
echo "alias  PARSIFAL='$PWD/PARSIFAL.sh'">>$bash_name
echo "alias  parsifal='PARSIFAL'">>$bash_name
echo "export PARSIFAL_data=$PWD/data">>$bash_name
echo "export exe_parsifal=$PWD/PARSIFAL.sh">>$bash_name
echo "function cd_parsifal(){">>$bash_name
echo "cd $PWD;">>$bash_name
echo "}">>$bash_name

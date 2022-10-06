/**
 Authors:
 Riccardo Farinelli <rfarinelli@fe.infn.it>
 Lia Lavezzi        <lia.lavezzi@to.infn.it>

 All rights reserved
 For the licensing terms see $PARSIFAL/LICENSE
**/

#ifndef PrintNTuple_h
#define PrintNTuple_h
namespace PARSIFAL{
  const bool PrintNTuple_Geo       = false;
  const bool PrintNTuple_Party     = false;
  const bool PrintNTuple_Ionio     = true;
  const bool PrintNTuple_Primary   = false;
  const bool PrintNTuple_Secondary = false;
  const bool PrintNTuple_Gain      = false; // CHECK not used so far, maybe not needed
  const bool PrintNTuple_Drift     = false; // CHECK not used so far, maybe not needed
  const bool PrintNTuple_Channel   = true;
  const bool PrintNTuple_Hit       = true;
  const bool PrintNTuple_Cluster1d = true;
}
#endif

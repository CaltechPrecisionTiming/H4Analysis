#!/bin/csh
set run = $1
echo "Run = $run"
mkdir -p /eos/cms/store/user/sixie/Timing/Testbeam_H4_062017/DataTree/${run}
scp cmsdaq@cms-h4-05:/data/raw/DataTree/${run}/\*.root /eos/cms/store/user/sixie/Timing/Testbeam_H4_062017/DataTree/${run}/

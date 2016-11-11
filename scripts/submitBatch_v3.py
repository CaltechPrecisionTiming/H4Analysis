#!/usr/bin/python2

import sys
import os
import commands
from commands import getstatusoutput
from commands import getoutput
import datetime
import argparse

# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

class customAction(argparse.Action):
    def __call__(self, parser, args, values, option_string=None):
        setattr(args, self.dest, values.split(','))

def get_comma_separated_args(self, arg_line):
    return arg_line.split(',')

# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

def lxbatchSubmitJob (run, path, cfg, outdir, queue, job_dir, dryrun, file_list, file_path):
    jobname = job_dir+'/H4Reco_'+queue+'_'+run+'.sh'
    #gitRepo = getoutput('git remote -v | grep origin | grep fetch | awk \'{print $2}\'')
    #pwd = os.getcwd()
    #work_directory = pwd.replace("scripts","")
    f = open (jobname, 'w')
    f.write ('#!/bin/sh' + '\n\n')
    #f.write ('git clone --recursive '+gitRepo+' \n')
    f.write('cp -r '+path+' ./H4Analysis \n')
    #f.write()
    f.write ('cd H4Analysis/ \n')
    f.write ('source scripts/setup.sh \n')
    f.write ('make -j 2 \n')
    f.write ('cp '+path+cfg+' job.cfg \n\n')
    #f.write ('cp '+path+'/ntuples/Template*.root ./ntuples/ \n\n')
    file_list = open (file_list,'r')
    f.write ('mkdir -p /tmp/H4Analysis_tmp/'+run+' \n')
    for file_this in file_list:
    	f.write ('/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select cp root://eoscms/'+file_path+file_this.rstrip()+" /tmp/H4Analysis_tmp/"+run+"/"+file_this.rstrip()+' \n')
    f.write ('bin/H4Reco job.cfg '+run+'\n\n')
    f.write ('cd ntuples \n')
    if "/eos/cms/" in outdir:
        f.write ('for file in *'+run+'.root; do /afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select cp $file '+outdir+'/$file; done\n')
    else:
        f.write ('cp *'+run+'.root '+outdir+'\n')
    f.write('rm -rf /tmp/H4Analysis_tmp/'+run+'\n')
    f.close ()
    getstatusoutput ('chmod 755 ' + jobname)
    if not dryrun:
        getstatusoutput ('cd '+job_dir+'; bsub -q ' + queue + ' ' + '-o '+ '/H4Reco_'+queue+'_'+run + '.log -u ' + os.environ['USER'] + '@cern.ch ' + jobname + '; cd -')

# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

def herculesSubmitJob (run, path, cfg, outdir, queue, job_dir, dryrun):
    jobname = job_dir+'/H4Reco_'+queue+'_'+run+'.sh'
    f = open (jobname, 'w')
    f.write ('#!/bin/sh' + '\n\n')
    f.write ('mkdir -p /gwpool/users/$USER/pool/'+run+'/ \n')
    f.write ('cd /gwpool/users/$USER/pool/'+run+'/ \n')
    f.write ('wget https://github.com/simonepigazzini/H4Analysis/archive/master.zip \n')
    f.write ('unzip master.zip \n')
    f.write ('cd H4Analysis-master/ \n\n')
    f.write ('cp '+path+'/ntuples/Template*.root ./ntuples/ \n')
    f.write ('cp '+path+cfg+' job.cfg \n')
    f.write ('source scripts/setup.sh \n')
    f.write ('make -j \n\n')
    f.write ('bin/H4Reco job.cfg '+run+'\n\n')
    f.write ('cp ntuples/*'+run+'.root '+outdir+'\n')
    f.write ('cd /gwpool/users/$USER/pool/ \n')
    f.write ('rm -r '+run+' \n')
    f.close ()
    getstatusoutput ('chmod 755 ' + jobname)
    if not dryrun:
        getstatusoutput ('cd '+job_dir+'; qsub -q ' + queue + ' ' + jobname + '; cd -')

# ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----

if __name__ == '__main__':

    parser = argparse.ArgumentParser (description = 'submit H4Reco step to lsf')
    parser.add_argument('-r', '--runs' , action=customAction, help='run to be processed, either list or file')
    parser.add_argument('-q', '--queue' , default = '1nh', help='batch queue (1nh)')
    parser.add_argument('-s', '--storage' , default = '/store/group/dpg_ecal/alca_ecalcalib/ECALTB_H4_Fall2015/', help='storage path')
    parser.add_argument('-v', '--version' , default = 'v1', help='production version')
    parser.add_argument('-c', '--cfg' , default = '../cfg/H4DAQ_base.cfg', help='production version')
    parser.add_argument('--dryrun' , action="store_true", default=False, help='do not submit the jobs, just create them')
    parser.add_argument('--batch' , default='lxbatch', help='batch system to use')
    parser.add_argument('--nf' , default="2", help='number of files per job')
    parser.add_argument('--inputDir' , default="/eos/cms/store/group/phys_susy/razor/Timing/Nov2016CERN/raw/", help='input directory')
   
    
    args = parser.parse_args ()

    ## check ntuple version
    stageOutDir = args.storage+'ntuples_'+args.version+'/'
    
    if args.batch == 'lxbatch':
        if getoutput('gfal-ls root://eoscms/'+stageOutDir) == "":
            print "ntuples version "+args.version+" directory on eos already exist! no jobs created."
            exit(0)
        getstatusoutput('cmsMkdir '+stageOutDir)    
    else:
        getstatusoutput('mkdir -p '+stageOutDir)    
    
    ## job setup
    local_path = getoutput('pwd')
    date = datetime.datetime.now().strftime("%d-%m-%Y")
    job_dir = local_path+"/"+date+"_ntuples_"+args.version
    getstatusoutput('mkdir -p '+job_dir)
    if local_path.endswith('scripts'):
        local_path = local_path[:-len('scripts')]

    if args.cfg.startswith('../'):
        args.cfg = args.cfg[len('../'):]
   
    if len(args.runs) == 1 and os.path.isfile(args.runs[0]):
        runs_file = open(args.runs[0], 'r')
        args.runs  = []
        if runs_file:
            for run in runs_file:
                args.runs.append(run.rstrip())
		os.system("eos ls "+args.inputDir+"/"+run.rstrip()+" > "+job_dir+"/files_run_"+run.rstrip()+".list")
		file_list = open (job_dir+"/files_run_"+run.rstrip()+".list", 'r')
		nfile_this = 0
		njob_this = 0
		if file_list:
			for file_this in file_list:
				njob_this =  int(nfile_this/int(args.nf))
				file_list_thisjob = open (job_dir+"/"+run.rstrip()+"_"+str(njob_this)+".list", 'a')
				#file_list_thisjob.write(args.inputDir+"/"+run.rstrip()+"/"+file_this)
				file_list_thisjob.write(file_this)
				nfile_this = nfile_this + 1
	
            	for ijob in range(0, njob_this+1):
			if args.batch == 'lxbatch':
				print 'submitting run: ', run, "  subjob ", str(ijob)
				lxbatchSubmitJob(run.rstrip()+"_"+str(ijob), local_path, args.cfg, stageOutDir, args.queue, job_dir, args.dryrun, job_dir+"/"+run.rstrip()+"_"+str(ijob)+".list", args.inputDir+"/"+run.rstrip()+"/")
			
	    
    ## create jobs
    
    #print 'submitting jobs to queue', args.queue
    #for run in args.runs:
    #    print 'submitting run: ', run
    #    if args.batch == 'lxbatch':
    #        lxbatchSubmitJob(run, local_path, args.cfg, stageOutDir, args.queue, job_dir, args.dryrun) 
    #    if args.batch == 'hercules':
    #        herculesSubmitJob(run, local_path, args.cfg, stageOutDir, args.queue, job_dir, args.dryrun) 

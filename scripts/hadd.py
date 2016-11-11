#!/usr/bin/env python

import subprocess, time, sys, os, shlex


if __name__ == "__main__":
	run_list_filename = sys.argv[1]
	
	pwd = os.getcwd()

	file_path = "~/eos/cms/store/group/phys_susy/razor/Timing/Nov2016CERN/ntuples_v3/"	
	job_file_path = "~/eos/cms/store/group/phys_susy/razor/Timing/Nov2016CERN/ntuples_v3/jobs/ntuples_v3/"	

	os.system("ls "+job_file_path+"* > tmp_list.list")
	jobs_file_list = open("tmp_list.list","r")
	if jobs_file_list:
		for job_file in jobs_file_list:
			file_size = os.path.getsize(job_file.rstrip())
			if(file_size < 33000000):
				print "[removing empty file] file size of "+ job_file.rstrip() + ":   "+ str(file_size)	
				os.system("rm "+job_file.rstrip())
	os.system("rm tmp_list.list")
	runs_file = open(run_list_filename, 'r')
        if runs_file:
            for run in runs_file:
		print "hadd "+file_path+"analysis_"+run.rstrip()+".root "+job_file_path+"analysis_"+run.rstrip()+"_*.root"
		os.system("hadd "+file_path+"analysis_"+run.rstrip()+".root "+job_file_path+"analysis_"+run.rstrip()+"_*.root")

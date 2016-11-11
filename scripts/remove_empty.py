#!/usr/bin/env python

import subprocess, time, sys, os, shlex


if __name__ == "__main__":
	run_list_filename = sys.argv[1]
	
	pwd = os.getcwd()

	file_path = "~/eos/cms/store/group/phys_susy/razor/Timing/Nov2016CERN/raw/"	

	runs_file = open(run_list_filename, 'r')
        if runs_file:
            for run in runs_file:
		os.system("ls "+file_path+run.rstrip()+"/* > tmp_list.list")
		file_list = open("tmp_list.list","r")
		if file_list:
			for job_file in file_list:
				file_size = os.path.getsize(job_file.rstrip())
				if(file_size < 10000000):
					print "[removing empty file] file size of "+ job_file.rstrip() + ":   "+ str(file_size)	
					os.system("rm "+job_file.rstrip())
		os.system("rm tmp_list.list")

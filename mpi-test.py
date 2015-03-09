# To run this file use the following command:
# python test.py <output_file> <num_processes> <num_iterations_of_barrier>


# printf statements should be like the following:
# unsigned long timestamp;    //Declare outside the loop

# The following should be inside the loop
# gettimeofday(&tv, NULL);
# timestamp = tv.tv_sec * 1000000 + tv.tv_usec;
# printf("Result %d After the barrier %d value %d at %lu\n",rank, i, val, timestamp)

import sys
try:
	filename = sys.argv[1]
	P = int(sys.argv[2])
	N = long(sys.argv[3])
	if N < 2 or P < 1:
	    print "Only 1 barrier or process is not sufficient"
	    sys.exit()
except:
	print "Invalid arguments"
	sys.exit()

dict = {}
f = open(filename, 'r')
for line in f:
	args = line.split()
	try:
	    dict[(long(args[5]), int(args[1]))] = [long(args[7]), args[9]];
	except:
	    print "Invalid statements in file"
	    print line
	    sys.exit()

for i in range(N-1):
    min_time = dict[(i+1, 0)][1]
    for j in range(1, P):
        if min_time > dict[(i+1, j)][1]:
        	min_time = dict[(i+1, j)][1]
    value = dict[(i, 0)][0]
    for j in range(P):
        if dict[(i, j)][0] != value:
            print "Value doesn't match"
            print [i, j, dict[(i, j)][0], value]
            sys.exit()
        if dict[(i,j)][1] > min_time:
            print "Time doesn't match"
            print [i, j, dict[(i,j)][1], min_time]
            sys.exit()
            
print "The barrier works"
f.close() 
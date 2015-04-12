import os, sys, time, BinProcLib, binQueries, pdb, csv
# Script Entry
args = sys.argv

t0=time.clock()
file_name = sys.argv[1]

path = os.path.dirname(os.path.abspath(file_name))
file_name = os.path.split(sys.argv[1])[-1]
basename = file_name[:-5]
TestNumber = file_name.split('-')[0]

TestHeader = []
# Try looking for the results.csv file and get the header.
try:
	TestRootPath= os.path.join(path,'..','..')
	TestResultsPath = os.path.join(TestRootPath,'WP2_5CIV_%s_results.csv'%TestNumber)
	
	resultscsv=open(TestResultsPath,'r')
	# Get the first few lines from the results file.
	for x in range(1,5):
		TestHeader.append(resultscsv.readline())
	resultscsv.close()
	TestName = "WP2_5CIV_%s" % (TestNumber)
except: 
	print 'Warning: WP2_5CIV_%s_results.csv not found.\nWill place results in current directory\n'%TestNumber
	TestRootPath= path
	TestName = "%s" % (TestNumber)
	TestHeader = "Test Name: %s\n"%(TestNumber)
	
TestHeader= ''.join(TestHeader)

# Try opening consecutive BIN files.
i=1
file_names=[]
while True:
	fullname =(basename + str(i) + ".bin")
	if os.path.isfile(os.path.join(path,fullname))==False: break
	file_names.append (fullname)
	print "Found file: " + fullname 
	i=i+1


# Create a database to store log data
database = BinProcLib.TestDB(TestRootPath,'%s.db'%TestNumber)

# Instantiate a BinLogFile Class from a given filename and load into db
# This is the first file
binFile = BinProcLib.BinLogFile(os.path.join(path,file_names[0]))

# Create the DB table. 
binFile.createTable(database,'RT_LOG')
binFile.insertData(database)

# Process the remaining files.
for file_name in file_names[1:]:
	binFile = BinProcLib.BinLogFile(os.path.join(path,file_name))
	binFile.insertData(database)

# Run Query
out_file_name="%s_AFDX_TXSummary.csv"%(TestName)
outfile = open(os.path.join(TestRootPath,out_file_name),'w')
outfile.write(TestHeader)

binQueries.checkSustained(database,'RT_LOG',outfile)
#binQueries.printSustained(database,outfile)

database.close()
outfile.close()


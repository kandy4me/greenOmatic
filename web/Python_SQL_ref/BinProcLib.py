import struct, string, sys, ctypes, os, mmap, pdb, csv, sqlite3, libxml2, codecs, re, collections

class BinLogFile:

	"""Class for binary log files"""
	
	SIGNAL_NAME_SIZE = 64   # Signal Names strings are 64 bytes long
	SIGNAL_TYPE_SIZE = 10   # Signal Type string are 64 bytes long
	DATA_TYPE_SIZE = 4	  # Data is 4 bytes (32 bit)
	HEADER_SIZE = 8		 # Header is 8 bytes
	file_name=''
	T=0					 # Sampling Period in micro-seconds
	T_s=0				 # Sampling Period in seconds
	N=0					 # number of signals in bin file
	name=''				 # file name obtained from file handle.
	no_dups=False
	
	signals=['empty']	   
	signal_types=['empty']
	signal_names = []
	sql_signal_types =[]
	header_offset = 0
	data_offset = 0
	data = []
	map = mmap.mmap(-1,1)
	parlist=''

	def __init__(self, file_name):
		
		self.file_name = file_name
		self.name=os.path.basename(self.file_name)
	
		try:
			self.file_handle = open(self.file_name,'rb')
			total_size=os.path.getsize(self.file_name)
			self.map = mmap.mmap(-1, total_size)
			# print "Opening file: "+ self.file_name 
	
		
			# Get the number of signals and Period of the RT_Log File
			content = self.file_handle.read(2*self.DATA_TYPE_SIZE)
			self.T,self.N = struct.unpack('II', content)
			self.T_s = self.T * 1e-6
			
			self.data_offset = self.HEADER_SIZE + self.N * (self.SIGNAL_NAME_SIZE + self.SIGNAL_TYPE_SIZE)	
			self.file_handle.seek(0)	# Rewind file since we read 2 bytes.
		
			# Allocate buffer and map the file to the memory buffer/
			buffer = self.file_handle.read(self.data_offset)
			self.map.write(buffer)
			buffer = self.file_handle.read()
			self.map.write(buffer)
		
			# Constants for iterating through the header.
			start = self.HEADER_SIZE
			end = self.data_offset - 1
			step = self.SIGNAL_TYPE_SIZE + self.SIGNAL_NAME_SIZE
			offsets = range (start, end, step)
			
			# Get list of signal names
			self.signal_names = [self.map[x:x+self.SIGNAL_NAME_SIZE].rstrip('\0') for x in offsets]
			self.signal_names.insert(0,"Count")	# Insert the Count signal.
		
			# Get list of Signal Types
			offsets = range (start+self.SIGNAL_NAME_SIZE, end+self.SIGNAL_NAME_SIZE, step)
			self.signal_types = [self.map[x:x+self.SIGNAL_TYPE_SIZE].rstrip('\0') for x in offsets]
			self.signal_types.insert(0,"INTEGER") # Insert the Count type.
			self.N += 1 # Increment N for the count variable.
		
			# Generate a string for the types used when decoding with struct.unpack
			unpack_str =[]
			self.sql_signal_types = []
			for string in self.signal_types:
				if string[0] == 'F': 
					unpack_str.append('f')
					self.sql_signal_types.append('DOUBLE')
				elif string[0] == 'B': 
					unpack_str.append('I') # need to pad 3 bytes (BOOLs are 4 bytes)
					self.sql_signal_types.append('TINYINT')
				#elif string[0] == 'B': unpack_str.append('?xxx') # need to pad 3 bytes (BOOLs are 4 bytes)
				elif string[0] == 'I':
					unpack_str.append('I')
					self.sql_signal_types.append('INT')
				else: print ('Error determining Signal Type: %s' % string)
			unpack_str = ''.join(unpack_str)
		
			
			# Insert TElapsed Column
			self.signal_names.insert(1,'TElapsed')
			self.signal_types.insert(1,'F')
			self.sql_signal_types.insert(1,'DOUBLE')
			
			# Look for Timestamp and if present, add the signal name
			# and type for the time at the begninning of the
			# respective lists.
			# Note: The type for time is 'T'
			try:
				hrs = self.signal_names.index('ConfigParameters.Time_Hours')
				min = self.signal_names.index('ConfigParameters.Time_Minutes')
				sec = self.signal_names.index('ConfigParameters.Time_Seconds')
				ms = self.signal_names.index('ConfigParameters.Time_ms')
			# TODO: Convert time stamp to a time type?
				#print '\nFound Time-Stamp Data!\n'
				INCLUDE_TIMESTAMP = True
				self.signal_names.insert(0,'Time')
				self.signal_types.insert(0,'T')
				self.sql_signal_types.insert(0,'TIME')
			
			except:	
				print 'Could not find Time-Stamp signals\n'
				INCLUDE_TIMESTAMP = False
		
			start = self.data_offset
			end = total_size
			step = (self.N * self.DATA_TYPE_SIZE)
			#offsets = xrange(start,end,step)
			# Create a pre-defined Struct Object such that data decoding of each row is faster.
			s = struct.Struct(unpack_str)
			
			# Decode each row and put it in a list of rows. Call the list of Rows Data
			self.data=[]
			
			# If timestamps are present, insert a column containing the
			# timestamp (as string) in the data table
			t_elapsed_col = self.signal_names.index('TElapsed')
			if INCLUDE_TIMESTAMP:
				for x in xrange(start,end,step):
					# unpack returns a tuple. Need to convert to list such that
					# duplicates can be removed.
					row = list(s.unpack_from(self.map,x))
					t_elapsed= self.T_s*row[0]
					row.insert(1,t_elapsed)
					# Generate time stamp string to insert at the beginning
					# of each row.
					timestamp = "{:02}:{:02}:{:02}.{:03}".format(row[hrs],row[min],row[sec],row[ms])
					row.insert(0,timestamp)
					
					#pdb.set_trace()
					self.data.append(row)
				
				# Must increment N since a col was inserted or 
				# other methods may break.
				self.N=self.N+2
	
				# If no time stamp is found, read in the data without
				# inserting anything.
				
			else:
				for x in xrange(start,end,step):
					# unpack returns a tuple. Need to convert to list such that
					# duplicates can be removed.
					row = list(s.unpack_from(self.map,x))
					t_elapsed= self.T_s*row[0]
					row.insert(t_elapsed_col,t_elapsed)
					self.data.append(row)
					#pdb.set_trace()
				self.N=self.N+1

		except IOError as e:
			print "\nCould not open file:" + self.file_name

	def getSQLFields(self):
			sql_table_string = (','.join(["'{}' {}".format(signame, sigtype) 
			for (signame, sigtype) in zip(self.signal_names, self.sql_signal_types)]))
			return sql_table_string

	def deDuplicate(self):
		duplicates=[]
		# Find duplicate Enteries
		for index, item in enumerate(self.signal_names):
			for x,y in enumerate(self.signal_names[index+1:]):
				if item == y:
					duplicates.append(x+index+1)
					print "Duplicate signal %s: %s" % (index,item)
		#pdb.set_trace()
		
		if (len(duplicates) > 0):
			duplicates.sort(reverse=True)
			# Remove duplicate entries
			[self.signal_names.pop(dup) for dup in duplicates]
			[self.signal_types.pop(dup) for dup in duplicates]
			
			# Remove duplicate signals from data 
			
			for x in xrange(0,len(self.data)):
				#pdb.set_trace()
				[self.data[x].pop(dup) for dup in duplicates]
				# Re-Tuple
				# self.data[x]=tuple(self.data[x])
			self.N=len(self.signal_names)
			# pdb.set_trace()
		else: 
			 pass
			 
		self.no_dups= True
		
	def createTable(self,database,table_name='RT_LOG'):
		# Remove any duplicate colums in data before creating table.
		if self.no_dups==False: self.deDuplicate()
		
		# Generate the SQL statement to insert a table with cols as columns
		cols = (','.join(["'{}' {}".format(signame, sigtype) 
		    for (signame, sigtype) in zip(self.signal_names, self.sql_signal_types)]))
		
		# Create the table called table_name with given cols. Make 'Count' primary Key.
		database.create_log_table(table_name,cols,'Count')
		
	def insertData(self,database,table_name='RT_LOG'):
		step = 999 # sqlite limit
		# Find count index
		count_index = self.signal_names.index('Count')
		if self.no_dups==False: self.deDuplicate()
		print "Inserting into table %s" % (table_name)
		# Insert data into table if the number of colums
		# is than the sqlite limit for host params.
		# sql is the SQL insert statement and data is the
		# list of row data
		
		if len(self.data[1]) < step:
			par_list=[]
			for x in xrange(0, len(self.data[0])):
				par_list.append('?')
			pars=','.join(par_list)
			

			sql = u"INSERT into %s VALUES (%s)" % (table_name,pars)
			data=self.data
			try:
				database.dbcursor.executemany(sql,data)

			except sqlite3.OperationalError as detail: 
				print "DB Insert error: %s" % detail 
			except sqlite3.ProgrammingError as detail: 
				print "DB Insert Error: %s" % detail 

		# Insert data into table if the number of colums
		# is greater than the sqlite limit for host params.
		# sql is the SQL insert statement and data is the
		# list of partial row data
		else:
			# data is split vertically at the limit(=step) index.
			# The first sub table is INSERTed into the SQL table
			# Subsequent tables (from left to right) are UPDATEd
			# into the SQL Table. UPDATE requires a place to update
			# the data into the table. this is why the count signal
			# is appened to the partial data rows
			
			for x in range(0,len(self.data[0]),step):
				# print "UPDATEing %s into table %s" % (self.name,table_name)
				signal_names = self.signal_names[x:x+step]
				set_str = (','.join(["'{}'=?".format(signame) for signame in signal_names]))
				cols = (','.join(["'{}'".format(signame) for signame in signal_names]))
				vals = (','.join(["?" for signame in signal_names]))
				data=[]
				
				# First partial table. INSERT as normal.
				if (x==0):
					[data.append(row[x:x+step]) for row in self.data]
					sql = u"INSERT into %s (%s) VALUES (%s)" % (table_name,cols,vals)
				
				# subsequent partial tables. Must UPDATE instead.
				elif (x > 0):
					# Add the count to the temporary data array we we can update the table.
					[data.append(row[x:x+step]+[row[count_index]]) for row in self.data]
					sql=u"UPDATE %s SET %s WHERE Count=?" % (table_name, set_str)
					#pdb.set_trace()
				# Finally execute the SQL Statement
				try:
					database.dbcursor.executemany(sql,data)

				except sqlite3.OperationalError as detail: 
					print "DB update error: %s" % detail 
				except sqlite3.ProgrammingError as detail: 
					print "DB update Error: %s" % detail 

		database.commit()		

	def __del__(self):

		self.file_handle.close()
		self.map.close()
	
class TestDB:
	def __init__(self, toolsetpath,db_name='RT_Log.db'):
		self.toolsetpath = toolsetpath
		self.dbcon = sqlite3.connect(os.path.join(toolsetpath, db_name))
		self.dbcon.row_factory = sqlite3.Row
		self.dbcursor = self.dbcon.cursor()

	def close(self):
		self.dbcon.commit()
		self.dbcon.close()
		
	def commit(self):
		self.dbcon.commit()
		
	def column_names(self, tablename='RT_LOG'):
		""" Pull out the list of valid columns. Column name is the
		# second data column in the pragma result. """
		columns = []
		self.dbcursor.execute("pragma table_info({0})".format(tablename))
		for row in self.dbcursor:
			columns.append(row[1])
		
		return columns
		
	def create_log_table(self,table_name,fields,key=''):
		
		if (key==''):
			sql = u"CREATE TABLE '%s' (%s)" % (table_name,fields)
		else:
			sql = u"CREATE TABLE '%s' (%s, PRIMARY KEY(%s))" % (table_name,fields,key)
		
		# Delete and redefine the tables
		try: 
			self.dbcursor.execute("DROP TABLE '%s'" % table_name)
		except: 
			pass
			
		try:
			self.dbcursor.execute(sql)
			self.commit()
		except sqlite3.OperationalError as detail: 
			print "DB Insert error: %s" % detail 
			
		except sqlite3.ProgrammingError as detail: 
			print "DB Insert Error: %s" % detail 
	
	

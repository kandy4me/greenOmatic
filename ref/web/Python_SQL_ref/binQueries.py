# -*- coding: utf-8 -*-
import pdb, sys
signal_queries = ( 
      #  Signal Changes State (old Query)
      # 
      #
	"""	SELECT x.Time 
				,x.Count
				,x.{1} as SignalName
				,x.{1} as FromValue
				,y.Time
				,y.Count
				,y.{1} as ToValue
		FROM {0} as x 
			,{0} as y
		WHERE 
			x.{1} != y.{1}
			AND
			x.Count = y.Count -1"""
			
      #  Signal Changes State
      # 
      #
	,"""
SELECT DISTINCT x.Time, x.TElapsed, x.Count, x.{1}
FROM {0} as x, {0} as y
WHERE (x.Count = 1)
OR ((x.{1}  != y.{1}) AND (x.Count = y.Count +1))
OR ((x.{1}  != y.{1}) AND (x.Count = y.Count -1))
ORDER BY x.Count
"""
	)

sustainedsignals =['H41_FS_FDS_Cockpit_Duct_Temp'
,'H42_FS_FDS_Cockpit_Duct_Temp'
,'H41_FS_FDS_Cockpit_Humidification_Heater_CTS_Temp'
,'H42_FS_FDS_Cockpit_Humidification_Heater_CTS_Temp'
,'H41_FS_FDS_Cockpit_Humidification_Heater_DTS_Temp'
,'H42_FS_FDS_Cockpit_Humidification_Heater_DTS_Temp'
#
,'ASCU1_Short_Range_Press_Schedule_1'
,'ASCU1_Short_Range_Press_Schedule_2'
,'ASCU2_Short_Range_Press_Schedule_1'
,'ASCU2_Short_Range_Press_Schedule_2'
#
,'Insufficient_Cooling_Performance_1'
,'Insufficient_Cooling_Performance_2'
#
,'TAPRV1_Closed_Indication_Invalid_1'
,'TAPRV1_Closed_Indication_Invalid_2'
,'TAPRV2_Closed_Indication_Invalid_1'
,'TAPRV2_Closed_Indication_Invalid_2'
,'TASOV_Closed_Indication_Invalid_1'
,'TASOV_Closed_Indication_Invalid_2'
#
,'CIV1_Closed_Indication_Invalid_1'
,'CIV1_Closed_Indication_Invalid_2'
,'CIV2_Closed_Indication_Invalid_1'
,'CIV2_Closed_Indication_Invalid_2'
#
,'FCV1_Closed_Indication_Invalid_1'
,'FCV1_Closed_Indication_Invalid_2'
,'FCV2_Closed_Indication_Invalid_1'
,'FCV2_Closed_Indication_Invalid_2'
#
,'FCV3_Closed_Indication_Invalid_1'
,'FCV3_Closed_Indication_Invalid_2'
,'FCV4_Closed_Indication_Invalid_1'
,'FCV4_Closed_Indication_Invalid_2'
#
,'CIV1_Failed_Open_1'
,'CIV1_Failed_Open_2'
,'CIV1_Failed_Closed_1'
,'CIV1_Failed_Closed_2'
#
,'CIV2_Failed_Open_1'
,'CIV2_Failed_Open_2'
,'CIV2_Failed_Closed_1'
,'CIV2_Failed_Closed_2'
#
,'H41_FS_FDS_Cabin_Zone1_Temp'
,'H41_FS_FDS_Cabin_Zone2_Temp'
,'H41_FS_FDS_Cabin_Zone3_Temp'
,'H41_FS_FDS_Cabin_Zone4_Temp'
,'H41_FS_FDS_Cabin_Zone5_Temp'
,'H41_FS_FDS_Cabin_Zone6_Temp'
,'H41_FS_FDS_Cabin_Zone7_Temp'
,'H41_FS_FDS_Cabin_Zone8_Temp'
#
,'H42_FS_FDS_Cabin_Zone1_Temp'
,'H42_FS_FDS_Cabin_Zone2_Temp'
,'H42_FS_FDS_Cabin_Zone3_Temp'
,'H42_FS_FDS_Cabin_Zone4_Temp'
,'H42_FS_FDS_Cabin_Zone5_Temp'
,'H42_FS_FDS_Cabin_Zone6_Temp'
,'H42_FS_FDS_Cabin_Zone7_Temp'
,'H42_FS_FDS_Cabin_Zone8_Temp'
#
,'Loss_of_CPIOM_H43_OHDS_Communication_1'
,'Loss_of_CPIOM_H44_OHDS_Communication_1'
,'Loss_of_cRDC_A01_OHDS_Communication_1'
,'Loss_of_cRDC_A02_OHDS_Communication_1'
#
,'PACK1_Minor_Fault_Present_1'
,'PACK1_Minor_Fault_Present_2'
,'PACK2_Minor_Fault_Present_1'
,'PACK2_Minor_Fault_Present_2'
#
,'PACK1_HX_Only_Cooling_Mode_1'
,'PACK1_HX_Only_Cooling_Mode_2'
,'PACK2_HX_Only_Cooling_Mode_1'
,'PACK2_HX_Only_Cooling_Mode_2'
#
,'PACK1_Major_Fault_Present_1'
,'PACK1_Major_Fault_Present_2'
,'PACK2_Major_Fault_Present_1'
,'PACK2_Major_Fault_Present_2'
#
,'Hot_Air1_Fault_1'
,'Hot_Air1_Fault_2'
,'Hot_Air2_Fault_1'
,'Hot_Air2_Fault_2'
#
,'Zone_Regulation_Fault_1'
,'Zone_Regulation_Fault_2'
#
,'Cabin_Zone1_Temp_Sensor_Not_Installed_1'
,'Cabin_Zone2_Temp_Sensor_Not_Installed_1'
,'Cabin_Zone3_Temp_Sensor_Not_Installed_1'
,'Cabin_Zone4_Temp_Sensor_Not_Installed_1'
,'Cabin_Zone5_Temp_Sensor_Not_Installed_1'
,'Cabin_Zone6_Temp_Sensor_Not_Installed_1'
,'Cabin_Zone7_Temp_Sensor_Not_Installed_1'
,'Cabin_Zone8_Temp_Sensor_Not_Installed_1'
#
,'Cabin_Zone1_Temp_Sensor_Not_Installed_2'
,'Cabin_Zone2_Temp_Sensor_Not_Installed_2'
,'Cabin_Zone3_Temp_Sensor_Not_Installed_2'
,'Cabin_Zone4_Temp_Sensor_Not_Installed_2'
,'Cabin_Zone5_Temp_Sensor_Not_Installed_2'
,'Cabin_Zone6_Temp_Sensor_Not_Installed_2'
,'Cabin_Zone7_Temp_Sensor_Not_Installed_2'
,'Cabin_Zone8_Temp_Sensor_Not_Installed_2'
#
,'CSAS1_Overheat_1'
,'CSAS2_Overheat_1'
,'CSAS1_Overheat_2'
,'CSAS2_Overheat_2'
#
,'CSAS1_Fault_1'
,'CSAS2_Fault_1'
,'CSAS1_Fault_2'
,'CSAS2_Fault_2'
#
,'CSAS1_RAOA_Fault_1'
,'CSAS2_RAOA_Fault_1'
,'CSAS1_RAOA_Fault_2'
,'CSAS2_RAOA_Fault_2'
#
,'CSAS1_RAO_Failed_Open_1'
,'CSAS2_RAO_Failed_Open_1'
,'CSAS1_RAO_Failed_Open_2'
,'CSAS2_RAO_Failed_Open_2'
#
,'CSAS1_Temp_Control_Valve_Failed_Open_1'
,'CSAS2_Temp_Control_Valve_Failed_Open_1'
,'CSAS1_Temp_Control_Valve_Failed_Open_2'
,'CSAS2_Temp_Control_Valve_Failed_Open_2'
#
,'CSAS1_Turbine_Valve_Failed_Open_1'
,'CSAS2_Turbine_Valve_Failed_Open_1'
,'CSAS1_Turbine_Valve_Failed_Open_2'
,'CSAS2_Turbine_Valve_Failed_Open_2'
#
,'CSAS_Minor_Fault_Present_1'
,'CSAS_Minor_Fault_Present_2'
#
,'PACK1_HX_Contaminated_1'
,'PACK2_HX_Contaminated_1'
,'PACK1_HX_Contaminated_2'
,'PACK2_HX_Contaminated_2'
#
,'CSAS1_HX_Contaminated_1'
,'CSAS2_HX_Contaminated_1'
,'CSAS1_HX_Contaminated_2'
,'CSAS2_HX_Contaminated_2']

# dboutput=[] # required to pass results of query to print func.
def checkSustained(database,table_name,outfile='checkSustained.csv'):
	'''Run query on a given table_name. database is the sqlite database object
	   with an open connection to a database containing table name.'''
	print "Running Query on %s " % table_name
	outfile.write("The follwing signals have changed state\n\n")
	
	# Run query for each signal.
	
	for signal in sustainedsignals:
		sys.stdout.write(".")
		sql=signal_queries[1].format(table_name,signal)
		database.dbcursor.execute(sql)
		queryresult=database.dbcursor.fetchall()
		# Print query result.
		if len(queryresult) > 0:
			outfile.write(u'\n%s,\n'%signal)
			outfile.write(',Time,TElapsed,SampleCount,Value\n')
			for row in queryresult:
				text_row = ','.join(['{}'.format(item) for item in row])
				outfile.write (',%s\n'%text_row)
		outfile.write('\n')
	# pdb.set_trace()
		
def printSustained(database,outfile):
	'''This function prints the results for the checkSustained query'''
	
	outfile.write("The follwing signals have changed state\n\n")
	for (x,signal) in enumerate(sustainedsignals):
		if len(dboutput[x]) > 0:
			outfile.write(u'\n%s,\n'%signal)
			outfile.write(',Time,TElapsed,Count,Time,Count,SignalValue,\n')
			for row in dboutput[x]:
				text_row = ','.join(['{}'.format(item) for item in row])
				outfile.write (',%s\n'%text_row)
			outfile.write('\n')

def printSustainedHTML(database,outfile):
	for (x,signal) in enumerate(sustainedsignals):
		outfile.write(u'\n%s,\n'%signal)
		outfile.write(',Time,TElapsed,Count,Time,Count,SignalValue,\n')
		for row in dboutput[x]:
			text_row = ','.join(['{}'.format(item) for item in row])
			outfile.write (',%s\n'%text_row)
		outfile.write('\n')


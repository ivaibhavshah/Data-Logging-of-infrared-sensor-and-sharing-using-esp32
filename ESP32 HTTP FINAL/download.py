import requests
from datetime import date
import pyodbc

IP_HOST = "192.168.1.5" 
# get date 
today = date.today()
year = today.year
month = today.month
day = (today.day)-1
print(day)
yesterday = str(year)+str(month)+str(day)
print(yesterday)
# get todays file
filename_download = str(yesterday)+".txt"
filename_download = filename_download.replace('-','')
filename_host = "download_"+filename_download
print(filename_host) 
URL = "http://"+IP_HOST+"?="+filename_host
payload = dict( submit=filename_host)
response = requests.post(URL,data=payload)
open(filename_download, "wb").write(response.content)

# Code to insert data into SQL server
cnxn = pyodbc.connect('DRIVER={SQL Server};SERVER=.\;DATABASE=master;UID=sa;PWD=knightmoon')
cursor = cnxn.cursor()
filename_download = 'D:/SEM 8/' + filename_download
qry = "BULK INSERT esp32_data FROM '"+filename_download+ "' WITH (FIRSTROW=2,FIELDTERMINATOR=',',ROWTERMINATOR='\n');"
print(qry)
cursor.execute(qry)
cnxn.commit()
print("Inserted")



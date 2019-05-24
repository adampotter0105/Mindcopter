import sqlite3
import datetime
visits_db = '__HOME__/drone/thrust.db'

maxDelay = 3

def request_handler(request):

    #Lat and lon
    if request['method'] is 'POST':
        if 'user' not in request['form'] or 'att' not in request['form'] or 'str' not in request['form']:
            return "You must enter an lat and lon value and a user"
            # return str(request)
        a = request["form"]

        strength = a['str']
        attention = a['att']
        meditation = a['med']

        user = a['user']
        date = datetime.datetime.now()

        #put in database
        conn = sqlite3.connect(visits_db)  # connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  # make cursor into database (allows us to execute commands)
        c.execute('''CREATE TABLE IF NOT EXISTS dated_table (user text, strength, attention, meditation, timestamp);''') # run a CREATE TABLE command
        c.execute('''INSERT into dated_table VALUES (?,?,?,?,?);''', (user, strength, attention, meditation, date))
        items = c.execute('''SELECT * FROM dated_table;''').fetchall()
        conn.commit()  # commit commands
        conn.close()  # close connection to database
        return "received values" # : {}:{}".format(items[-1][2], items[-1][4])
    
        
    if request['method'] is 'GET':
        #REQUEST
        conn = sqlite3.connect(visits_db)  #connect to that database (will create if it doesn't already exist)
        c = conn.cursor()  #make cursor into database (allows us to execute commands)
        four_seconds_ago = datetime.datetime.now()- datetime.timedelta(minutes=maxDelay) #create time for four seconds ago!
        c.execute('''CREATE TABLE IF NOT EXISTS dated_table (user text, strength, attention, meditation, timestamp);''')
        items = c.execute('''SELECT * FROM dated_table WHERE timestamp > ? ORDER BY timestamp DESC;''',(four_seconds_ago,)).fetchall()
        #items = c.execute('''SELECT * FROM dated_table ORDER BY timestamp DESC;''').fetchall()
        #items = c.execute('''SELECT * FROM dated_table;''').fetchall()
        conn.commit()  # commit commands
        conn.close()  # close connection to database
        return "{}:{}".format(items[0][2], items[0][4])

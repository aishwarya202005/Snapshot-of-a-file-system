import sqlite3

conn = sqlite3.connect('DatabaseAi.db')

c = conn.cursor()
# c.execute("""CREATE TABLE CAR_OWNERS(
# 	first text,
# 	last text,
# 	pay integer
# 	)""")

# c.execute("INSERT INTO CAR_OWNERS VALUES ('corey', 'Shivachandra',500 )")

c.execute("SELECT * FROM CAR_OWNERS")

print (c.fetchall())


conn.commit()

conn.close()ss

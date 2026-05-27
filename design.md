# Database sensor program
Make a small C++ database program, using sqlite as integrated database to store sensor data. 

## Program structure
To use the database, the program should read a setup sql file (do not use a compiled internal sql string). The database data should be filled with another db file. The interface is a simple TUI interface.

## Database structure
The tables look like this:
Sensors table:
| Sensor ID | Gateway id | Sensor name | location id | extra location info |
| --- | --- | --- | --- | --- |
| primary key | foreign key | text | foreign key | text |

Sensor type table:
| Type ID | Type name |
| --- | --- |
| primary key | text |

Sensor type link table (many to many)
| link id | type id | sensor id |
| --- | --- | --- |
| primary key | foreign key | foreign key |

Locations table
| location id | location gps | location info |
| --- | --- | --- |
| primary key | gps string | text |

Gateways table:
| Gateway ID | Gateway name | Gateway location |
| --- | --- | --- |
| primary key | text | gps string |

Sensor values:
| primary key | sensor id | timestamp | value |
| --- | --- | --- |
| primary key | foreign key | datetime | integer | 

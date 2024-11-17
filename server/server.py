from flask import Flask, request, jsonify, render_template
import sqlite3

app = Flask(__name__)

# Functie voor het maken van een databaseverbinding
def connect_db():
    return sqlite3.connect('weather_data.db')

# Route voor het ontvangen van data van de sensor en het opslaan in de database
@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    print("Data ontvangen:", data)
    if not data:
        return jsonify({"message": "No data received"}), 400

    try:
        conn = connect_db()
        cursor = conn.cursor()

        print("Database verbinding gemaakt")
        
        query = '''INSERT INTO weather_data 
                  (brightness, temperature, humidity, wind, latitude, longitude)
                  VALUES (?, ?, ?, ?, ?, ?)'''
        values = (
            data['brightness'],
            data['temperature'],
            data['humidity'],
            data['wind'],
            data['latitude'],
            data['longitude']
        )
        
        print("Query:", query)
        print("Waarden:", values)
        
        cursor.execute(query, values)
        
        print("Query uitgevoerd")
        
        conn.commit()
        print("Changes committed")
        
        conn.close()
        print("Database verbinding gesloten")

        return jsonify({"message": "Data opgeslagen"}), 200

    except Exception as e:
        print("Fout bij opslaan:", e)
        return jsonify({"message": "Error: " + str(e)}), 500

# Route voor het initialiseren van de database 
def init_db():
    conn = connect_db()
    cursor = conn.cursor()
    
    # Maak de weather_data tabel
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS weather_data (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        brightness INTEGER,
        temperature FLOAT,
        humidity INTEGER,
        wind FLOAT,
        latitude FLOAT,
        longitude FLOAT,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
    )
    ''')
    
    conn.commit()
    conn.close()


# Endpoint voor het ophalen van alle opgeslagen data (GET)
@app.route('/data', methods=['GET'])
def get_data():
    try:
        # Maak verbinding met de database
        conn = connect_db()
        cursor = conn.cursor()

        # Haal alle gegevens op uit de tabel
        cursor.execute('SELECT * FROM weather_data')
        rows = cursor.fetchall()

        # Definieer kolomnamen voor betere presentatie
        columns = ['id', 'brightness', 'temperature', 'humidity', 'wind', 'latitude', 'longitude', 'timestamp']

        # Zet rijen om in een lijst van dictionaries
        data = [dict(zip(columns, row)) for row in rows]

        conn.close()

        return jsonify(data), 200

    except Exception as e:
        return jsonify({"message": "Er ging iets mis: " + str(e)}), 500

# Route voor het ophalen van de kaartpagina 
@app.route('/')
def map_page():
    return render_template('map.html')

# Route voor het ophalen van de data per locatie
@app.route('/locations')
def get_locations():
    conn = connect_db()
    cursor = conn.cursor()
    cursor.execute('''
        SELECT latitude, longitude,
        AVG(brightness) as avg_brightness,
        AVG(wind) as avg_wind,
        AVG(temperature) as avg_temp,
        AVG(humidity) as avg_humidity,
        MAX(timestamp) as latest_timestamp
        FROM weather_data 
        GROUP BY latitude, longitude
    ''')
    locations = cursor.fetchall()
    conn.close()
    
    return jsonify([{
        'lat': row[0],
        'lng': row[1],
        'avg_brightness': row[2],
        'avg_wind': row[3],
        'avg_temp': row[4],
        'avg_humidity': row[5],
        'timestamp': row[6]
    } for row in locations])


if __name__ == "__main__":
    init_db()  # Initialize database
    app.run(host='0.0.0.0', port=5000)

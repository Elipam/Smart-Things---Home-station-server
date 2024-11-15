const express = require("express");
const mysql = require("mysql");
const bodyParser = require("body-parser");

const app = express();
app.use(bodyParser.json());

// Database-verbinding instellen
const db = mysql.createConnection({
  host: "localhost",
  user: "jouw_db_gebruiker",
  password: "jouw_db_wachtwoord",
  database: "ESPData",
});

db.connect((err) => {
  if (err) throw err;
  console.log("Verbonden met MySQL database!");
});

// Ontvangst van data via POST-request
app.post("/data", (req, res) => {
  const { temperature, humidity, latitude, longitude } = req.body;
  const query =
    "INSERT INTO sensor_data (temperature, humidity, latitude, longitude) VALUES (?, ?, ?, ?)";

  db.query(
    query,
    [temperature, humidity, latitude, longitude],
    (err, result) => {
      if (err) {
        res.status(500).send("Database error");
        return;
      }
      res.send("Gegevens opgeslagen");
    }
  );
});

app.get("/markers", (req, res) => {
  const query =
    "SELECT temperature, humidity, latitude, longitude FROM sensor_data";

  db.query(query, (err, results) => {
    if (err) {
      res.status(500).send("Database error");
      return;
    }
    res.json(results);
  });
});

// Server starten
const port = 3000;
app.listen(port, () => {
  console.log(`Server draait op poort ${port}`);
});

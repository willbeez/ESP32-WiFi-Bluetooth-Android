from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import send_from_directory

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///sensor_data.db'
db = SQLAlchemy(app)

class Device(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String, nullable=False, unique=True)
    data_points = db.relationship('SensorData', backref='device', lazy=True)

    def __repr__(self):
        return f'<Device {self.device_id}>'

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.Integer, db.ForeignKey('device.id'), nullable=False)
    key = db.Column(db.String, nullable=False)
    value = db.Column(db.String, nullable=False)
    timestamp = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    json_id = db.Column(db.Integer, nullable=False)  # Add this line

    def serialize(self):
        return {
            'id': self.id,
            'device_id': self.device.device_id,
            'key': self.key,
            'value': self.value,
            'timestamp': self.timestamp.strftime('%Y-%m-%d %H:%M:%S'),
            'json_id': self.json_id,  # Add this line
        }

    def __repr__(self):
        return f'<SensorData {self.id}>'

def setup_database():
    with app.app_context():
        db.create_all()

@app.route('/api/sensor_data', methods=['GET', 'POST'])
def sensor_data():
    if request.method == 'POST':
        data = request.get_json()

        # Ensure device_id is provided
        if 'device_id' not in data:
            return {'error': "Missing device_id in the data"}, 400

        device_id = data['device_id']

        # Find or create a device with the given device_id
        device = Device.query.filter_by(device_id=device_id).first()
        if device is None:
            device = Device(device_id=device_id)
            db.session.add(device)
            db.session.commit()

        try:
            timestamp = datetime.fromisoformat(data['timestamp'])
        except (KeyError, ValueError) as e:
            return {'error': f"Invalid or missing timestamp: {e}"}, 400

        # Get the highest json_id in the database
        highest_json_id = db.session.query(db.func.max(SensorData.json_id)).scalar()
        if highest_json_id is None:
            highest_json_id = 0

        try:
            for item in data['data']:
                key = item['key']
                value = item['value']
                new_sensor_data = SensorData(
                    device_id=device.id,
                    key=key,
                    value=str(value),
                    timestamp=timestamp,
                    json_id=highest_json_id + 1  # Increment the json_id
                )
                db.session.add(new_sensor_data)

            db.session.commit()
            return {'message': 'Data added successfully'}, 201

        except Exception as e:
            return {'error': f"An unexpected error occurred: {e}"}, 500

    elif request.method == 'GET':
        all_sensor_data = SensorData.query.order_by(SensorData.timestamp.desc()).all()
        return jsonify([data.serialize() for data in all_sensor_data])

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')


if __name__ == '__main__':
    setup_database()
    app.run(host='0.0.0.0', port=5000, debug=True)

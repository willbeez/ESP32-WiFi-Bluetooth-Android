from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import send_from_directory
from langchain import OpenAI, SQLDatabase, SQLDatabaseChain
import uuid

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///sensor_data.db'
db = SQLAlchemy(app)
openai_api_key = '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'
device_name_to_id = {}

aidb = SQLDatabase.from_uri(
        "sqlite:////home/william/Git/ESP32-WiFi-Bluetooth-Android/server/instance/sensor_data.db",
    sample_rows_in_table_info=10  # Change this value to the number of rows you want
)

questionllm = OpenAI(temperature=.7, openai_api_key=openai_api_key)
answerllm = OpenAI(temperature=0, openai_api_key=openai_api_key)

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String, db.ForeignKey('device.device_id'), nullable=False)
    key = db.Column(db.String, nullable=False)
    value = db.Column(db.String, nullable=False)
    timestamp = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    json_id = db.Column(db.Integer, nullable=False)

    device = db.relationship('Device', backref=db.backref('sensor_data', lazy=True))

    def serialize(self):
        return {
            'id': self.id,
            'device_id': self.device_id,
            'key': self.key,
            'value': self.value,
            'timestamp': self.timestamp.strftime('%Y-%m-%d %H:%M:%S'),
            'json_id': self.json_id,
        }

    def __repr__(self):
        return f'<SensorData {self.id}>'

class Device(db.Model):
    device_id = db.Column(db.String, primary_key=True)
    device_name = db.Column(db.String, nullable=False)
    update_timestamp = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    key = db.Column(db.String, nullable=False)
    value = db.Column(db.String, nullable=False)

    def serialize(self):
        return {
            'device_id': self.device_id,
            'device_name': self.device_name,
            'update_timestamp': self.update_timestamp.strftime('%Y-%m-%d %H:%M:%S'),
            'key': self.key,
            'value': self.value,
        }

    def __repr__(self):
        return f'<Device {self.device_id}>'


def setup_database():
    with app.app_context():
        db.create_all()


@app.route('/api/sensor_data', methods=['GET', 'POST'])
def sensor_data():
    if request.method == 'POST':
        data = request.get_json()

        device_name = data.get('device_name', '')

        if not device_name:
            return {'error': "Missing device_name in the data"}, 400

        if device_name in device_name_to_id:
            device_id = device_name_to_id[device_name]
        else:
            device_id = str(uuid.uuid4())
            device_name_to_id[device_name] = device_id

        try:
            timestamp = datetime.fromisoformat(data['timestamp'])
        except (KeyError, ValueError) as e:
            return {'error': f"Invalid or missing timestamp: {e}"}, 400

        highest_json_id = db.session.query(db.func.max(SensorData.json_id)).scalar()
        if highest_json_id is None:
            highest_json_id = 0

        try:
            for item in data['data']:
                key = item['key']
                value = item['value']
                new_sensor_data = SensorData(
                    device_id=device_id,
                    key=key,
                    value=str(value),
                    timestamp=timestamp,
                    json_id=highest_json_id + 1
                )
                db.session.add(new_sensor_data)

            db.session.commit()
            return {'message': 'Data added successfully'}, 201

        except Exception as e:
            return {'error': f"An unexpected error occurred: {e}"}, 500

    elif request.method == 'GET':
        all_sensor_data = SensorData.query.order_by(SensorData.timestamp.desc()).all()
        return jsonify([data.serialize() for data in all_sensor_data])

@app.route('/api/metadata', methods=['POST'])
def metadata():
    if request.method == 'POST':
        data = request.get_json()

        device_name = data.get('device_name', '')

        if device_name:
            device = Device.query.filter_by(device_name=device_name).first()
            if device:
                device_id = device.device_id
            else:
                device_id = str(uuid.uuid4())
        else:
            return {'error': "Missing device_name in the data"}, 400

        try:
            timestamp = datetime.fromisoformat(data['timestamp'])
        except (KeyError, ValueError) as e:
            return {'error': f"Invalid or missing timestamp: {e}"}, 400

        try:
            device = Device.query.get(device_id)
            if device:
                device.update_timestamp = timestamp
                print("updated timestamp")

            for item in data['metadata']:
                key = item['key']
                value = item['value']

                existing_metadata = Device.query.filter_by(device_id=device_id, key=key).first()
                if existing_metadata:
                    print("Found existing metadata")
                    existing_metadata.value = value
                else:
                    print("creating new metadata")
                    new_metadata = Device(
                        device_id=device_id,
                        device_name=device_name,
                        update_timestamp=timestamp,
                        key=key,
                        value=value
                    )
                    db.session.add(new_metadata)
                    print("Created new metadata")

            db.session.commit()
            return {'message': 'Metadata added or updated successfully'}, 201

        except Exception as e:
            return {'error': f"An unexpected error occurred: {e}"}, 500








@app.route('/api/response')
def get_response():
    table_info = aidb.get_table_info()

    generated_question = questionllm.generate(["What are some hidden real world inefficiencies we can find from querying the data in this database? \n\n" + table_info])

    db_chain = SQLDatabaseChain(llm=answerllm, database=aidb, verbose=True)
    response = db_chain.run(generated_question.generations[0])

    return jsonify({"response": response})


@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

if __name__ == '__main__':
    setup_database()
    app.run(host='0.0.0.0', port=80, debug=True)


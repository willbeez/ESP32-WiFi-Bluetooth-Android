from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime
from flask import send_from_directory
from langchain import OpenAI, SQLDatabase, SQLDatabaseChain
import uuid

openai_api_key = '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'
aidb = SQLDatabase.from_uri(
        "sqlite:////home/william/Git/ESP32-WiFi-Bluetooth-Android/server/instance/sensor_data.db",
    sample_rows_in_table_info=10  # Change this value to the number of rows you want
)
questionllm = OpenAI(temperature=1, openai_api_key=openai_api_key)
answerllm = OpenAI(temperature=0, openai_api_key=openai_api_key)


print(aidb.get_table_info())


def get_response():
    db_chain = SQLDatabaseChain(llm=questionllm, database=aidb, verbose=True)
    response = db_chain.run("what values do you think would be unhealthy for the plant?")

get_response()
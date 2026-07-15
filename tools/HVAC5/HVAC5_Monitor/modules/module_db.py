##save_data()
##save_alarm()
##load_history()
##load_alarm_history()

import  sqlite3
from datetime import datetime, timedelta

def save_to_db(data):
    conn = sqlite3.connect("hvac.db")
    cursor = conn.cursor()

    cursor.execute("""
    INSERT INTO hvac VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    """, (
        data["time"],
        data.get("T1"),
        data.get("T2"),
        data.get("T3"),
        data.get("T4"),
        data.get("T5"),
        data.get("T_SP"),
        data.get("Y1"),
        data.get("Y2"),
        data.get("n1"),
        data.get("B1"),
        data.get("air_bypass"),
        data.get("TEN"),
        data.get("Alarms"),
    ))

    conn.commit()
    conn.close()


def save_alarm(alarm):
    conn = sqlite3.connect("hvac.db")
    cursor = conn.cursor()

    cursor.execute("""
    INSERT INTO alarms (time, alarm_id, message, level, state)
    VALUES (?, ?, ?, ?, ?)
    """, (
        datetime.now().isoformat(),
        alarm["id"],
        alarm["name"],
        alarm["level"],
        "ON"
    ))

    conn.commit()
    conn.close()


def save_alarm_to_db(alarm, state):
    conn = sqlite3.connect("hvac.db")
    cursor = conn.cursor()

    cursor.execute("""
    INSERT INTO alarms (time, alarm_id, message, level, state)
    VALUES (?, ?, ?, ?, ?)
    """, (
        datetime.now().isoformat(),
        alarm["id"],
        alarm["name"],
        alarm["level"],
        state
    ))

    conn.commit()
    conn.close()

##ALARMS
##ALARM_MAP
##check_alarms()
##process_alarm_changes()

# Список словарей аварий
# или реестр аварий

ALARMS = [
    {
        "id": "T1_HIGH",
        "name": "Перегрев T1",
        "check": lambda d: d.get("T1", 0) > 30,
        "level": "HIGH"
    },
    {
        "id": "T1_LOW",
        "name": "Переохлаждение T1",
        "check": lambda d: d.get("T1", 0) < 5,
        "level": "LOW"
    },
    {
        "id": "ALARM",
        "name": "Общая авария",
        "check": lambda d: d.get("Alarms", 0) & 0x01,
        "level": "CRITICAL"
    },
    {
        "id": "n1_FAIL",
        "name": "Вентилятор П1 авария",
        "check": lambda d: d.get("Alarms", 0) & 0x02,
        "level": "CRITICAL"
    },
    {
        "id": "B1_FAIL",
        "name": "Вентилятор B1 авария",
        "check": lambda d: d.get("Alarms", 0) & 0x04,
        "level": "CRITICAL"
    },    
    {
        "id": "RECUPERATOR_FREEZE",
        "name": "Рекуператор замерз!",
        "check": lambda d: d.get("Alarms", 0) & 0x08,
        "level": "CRITICAL"
    },    
    {
        "id": "WARNING_FREEZE",
        "name": "Угроза заморозки рекуператора!",
        "check": lambda d: d.get("Alarms", 0) & 0x10,
        "level": "HIGH"
    },    
    {
        "id": "Y1_ALARM",
        "name": "Ошибка привода Y1",
        "check": lambda d: d.get("Alarms", 0) & 0x20,
        "level": "HIGH"
    },
    {
        "id": "Y4_ALARM",
        "name": "Ошибка привода Y4",
        "check": lambda d: d.get("Alarms", 0) & 0x40,
        "level": "HIGH"
    },    
    {
        "id": "MODBUS_FAULT",
        "name": "Ошибка датчика Modbus",
        "check": lambda d: d.get("Alarms", 0) & 0x100,
        "level": "HIGH"
    }    
]

########################################
# @brief возвращает список list активных аварий
# 
#
########################################
def check_alarms(data):
    active = []

    for alarm in ALARMS:
        try:
            if alarm["check"](data):
                active.append(alarm)
        except:
            pass

    return active


###################################################
# Критичность аварий
###################################################
def level_icon(level):
    if level == "CRITICAL":
        return "🔴 CRITICAL"
    elif level == "HIGH":
        return "🟠 HIGH"
    else:
        return "🟡 LOW"

###################################################
# Активность аварий
###################################################
def format_state(state):
    if state == "ON":
        return "🔴 АКТИВНА"
    else:
        return "🟢 СНЯТА"


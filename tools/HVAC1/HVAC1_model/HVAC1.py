import serial
import time
import struct
import module_model
import threading
#import queue
import argparse
import logging     # log files
import math
import ctypes
import numpy as np

import json, socket

import sys  #только на время отладки парсить:

from pymodbus.client import ModbusSerialClient

from dataclasses import dataclass
from datetime import datetime
from multiprocessing import Process, Queue, Manager, Array
import multiprocessing as mp


import os



ROWS = 5
COLS = 10



###################################
# @brief функция вычисления CRC16
###################################    
def crc16(data: bytes) -> int:
    """Вычисление CRC16 Modbus (стандартный алгоритм)"""
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            if crc & 0x0001:
                crc >>= 1
                crc ^= 0xA001
            else:
                crc >>= 1
    return crc

###################################
# @brief функция формирования ответа на запрос
###################################

def build_response(slave_id, function_code, start_address, quantity, REGS):
    """Формирует Modbus RTU ответ"""
    
    byte_count = quantity * 2

    values = REGS[slave_id - 1, start_address:start_address + quantity]
    
    payload = struct.pack("B", byte_count)
    for val in values:
        payload += struct.pack(">h", val)  # big-endian 16-bit
    response = struct.pack("BB", slave_id, function_code) + payload
    crc = crc16(response)
    if REGS[slave_id - 1, 9] == 0:
        response += struct.pack("<H", crc)  # little-endian CRC
    return response 

###################################
# @brief функция анализа принятого запроса
###################################
def parse_request(request: bytes):
    """Разбирает запрос Modbus RTU"""
    if len(request) < 8:
        return None  # Не хватает байт
    slave_id, function_code = request[0], request[1]
    if (slave_id > 5) or function_code != 0x03:
        return None
    start_addr = struct.unpack(">H", request[2:4])[0]
    quantity = struct.unpack(">H", request[4:6])[0]
    return start_addr, quantity, slave_id


#################################################
# @brief Функция для шапки логирования HVAC1
#################################################
def log_system_state_Head_HVAC1():
    # Заголовок таблицы (пишем только один раз)
    header = f"{' ':<42}" 
    logger.info(header)


    header = f"{'Дата':<12}{'Время':<10}{'M1':<4}" \
             f"{'Y1':<6}{'T1':<6}{'T2':<6}{'T3':<6}{'T4':<8}{'T5':<6}"\
             f"{'Mode':<6}{'Q_Summ':<14}"
    logger.info(header)

    header = f"{' ':<12}{' ':<10}{' ':<4}" \
             f"{' %':<6}{'°C':<6}{'°C':<6}{'°C':<6}{'°C':<8}{'°C':<6}"\
             f"{' ':<6}{'Дж':<14}"
         
    logger.info(header)

    header = f"{' ':<42}" 
    logger.info(header)

#################################################
# @brief Функция для логирования HVAC1
#################################################
def log_system_state_HVAC1(system: module_model.HeaterSystem):
    current_day = datetime.now().strftime("%Y:%m:%d")
    current_time = datetime.now().strftime("%H:%M:%S")
    row = f"{current_day:<12}" \
          f"{current_time:<10}" \
          f"{system.M1.status:<4}" \
          f"{system.water_valve.position:<6}" \
          f"{str(system.T1.value):<6}" \
          f"{str(system.T2.value):<6}" \
          f"{str(system.T3.value):<6}" \
          f"{str(round(system.T4.value, 2)):<8}" \
          f"{str(system.T5.value):<6}" \
          f"{str(system.mode.mode):<6}" \
          f"{str(round(system.Q_Summ,8)):<14}"            
    logger.info(row)

########################################################################
# @brief === ПОТОК ДЛЯ ОБРАБОТКИ КЛАВИШ ===
########################################################################
def keyboard_thread():
    print("Поток клавиатуры запущен. Команды:")
    print("  t  — увеличить уличную температуру")
    print("  g  — уменьшить уличную температуру")
    print("  e1 — не отвечает датчик T1")
    print("  f1 — ошибка CRC16 датчик T1")    
    print("  n1 — датчик T1 нормализовался")
    print("  wl — уменьшение теплоизбытков кВт")
    print("  wg — увеличение теплоизбытков кВт")    
    print("  q  — выход")

    while True:
        cmd = input(">>> ").strip()

        # отправляем команду в очередь main-процессу
        print(f"keyboard_thread Команда: {cmd}")
        q_cmd.put(cmd)

        if cmd == "q":
            break


###########################################################################################
## @brief Поток Modbus RTU Master.  Утилита читает регистры контроллера  и кладет их в очереди
###########################################################################################
def MASTER_producer(COM_port, baud, q_SP, q_mode, q_data, q_DI, q_DO, q_Alarms, q_AI, q_AO, q_NET, q_NET1, q_NET2, q_NET3, q_NET4, q_NET5, q_COILS):

    SP = [0] * 15
    mode = [0] * 5
    DI = 0
    DO = 0
    Alarms = 0
    AI = [0] * 15
    AO = [0] * 15
    NET0 = [0] * 8
    NET1 = [0] * 8
    NET2 = [0] * 8
    NET3 = [0] * 8
    NET4 = [0] * 8
    NET5 = [0] * 8
    NET0 = [0] * 8    
    COILS1 = [0] * 8


    
    print(f"Процесс MASTER_producer запущен, PID={os.getpid()}")
        
    client = ModbusSerialClient(
        #port='COM4',
        port=COM_port,
        baudrate=baud,
        parity='N',
        stopbits=1,
        bytesize=8,
        timeout=0.1
    )
    client.connect()

    print(f"Работаю с портом {COM_port}, скорость {baud}")

    # цикл чтения сигналов контроллера Уставок 0x20 - 0x25 DI, DO, AI[0,1,2], AO[0,1,2]
    while True:
        # чтение уставок
        try:
            result = client.read_holding_registers(address=0x20, count=6, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения SP:")             
                q_SP.put([0x1111,0x1111,0x1111,0x1111])
                continue
            
            print(f"[MASTER] Прочитал SP: {result.registers}")            
            q_SP.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса SP: {e}")
            q_SP.put([0x2222, 0x2222, 0x2222, 0x2222])
            continue

        time.sleep(0.3)    # 0.3 сек
       
        # чтение регистра mode 0x48, 0x49,0x4A,0x4B,0x4C
        try:
            result = client.read_holding_registers(address=0x48, count=5, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения mode:")             
                q_mode.put(0x1111)
                continue
            
            print(f"[MASTER] Прочитал mode: {result.registers}")            
            q_mode.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса mode: {e}")
            q_mode.put(0x2222)
            continue
        
        time.sleep(0.3)    # 0.3 сек
        
        # чтение DI
        try:
            result = client.read_holding_registers(address=0x0F, count=1, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения DI:")             
                q_DI.put(0x1111)
                continue
            
            print(f"[MASTER] Прочитал DI: {result.registers}")            
            q_DI.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса DI: {e}")
            q_DI.put(0x2222)
            continue
        
        time.sleep(0.3)    # 0.3 сек


        # чтение DO
        try:
            result = client.read_holding_registers(address=0x2F, count=1, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения DO:")             
                q_DO.put(0x1111)
                continue
            
            print(f"[MASTER] Прочитал DO: {result.registers}")            
            q_DO.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса DO: {e}")
            q_DO.put(0x2222)
            continue
        
        time.sleep(0.3)    # 0.3 сек        

        # чтение Alarms
        try:
            result = client.read_holding_registers(address=0x1F, count=1, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения Alarms:")             
                q_Alarms.put(0x1111)
                continue
            
            print(f"[MASTER] Прочитал Alarms: {result.registers}")            
            q_Alarms.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса Alarms: {e}")
            q_Alarms.put(0x2222)
            continue
        
        time.sleep(0.3)    # 0.3 сек    

        # чтение AI 0x10, 0x11, 0x12
        try:
            result = client.read_holding_registers(address=0x10, count=3, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения AI:")             
                q_AI.put([0x1111,0x1111,0x1111])
                continue
            
            print(f"[MASTER] Прочитал AI: {result.registers}")            
            q_AI.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса AI: {e}")
            q_AI.put([0x2222, 0x2222, 0x2222])
            continue

        time.sleep(0.3)    # 0.3 сек


        # чтение AO 0x30, 0x31, 0x32
        try:
            result = client.read_holding_registers(address=0x30, count=3, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения AO:")             
                q_AO.put([0x1111,0x1111,0x1111])
                continue
            
            print(f"[MASTER] Прочитал AO: {result.registers}")            
            q_AO.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса AO: {e}")
            q_AO.put([0x2222, 0x2222, 0x2222])
            continue

        time.sleep(0.3)    # 0.3 сек



        #--------  Читаем регистры цепей по выбору              

        # чтение контрольных точек  0xD0, 0xD7
        try:
            result = client.read_holding_registers(address=0xD0, count=0x08, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения NET0:")             
                q_NET.put([0,0,0,0,0,0,0,0])
                continue
            
            print(f"[MASTER] Прочитал NET0: {result.registers}")            
            q_NET.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса NET0: {e}")
            q_NET.put([0,0,0,0,0,0,0,0])
            continue

        time.sleep(0.3)    # 0.3 сек

        # чтение контрольных точек  0xD8, 0xDF 
        try:
            result = client.read_holding_registers(address=0xD8, count=0x08, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения NET1:")             
                q_NET1.put([0,0,0,0,0,0,0,0])
                continue
            
            print(f"[MASTER] Прочитал NET1: {result.registers}")            
            q_NET1.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса NET1: {e}")
            q_NET1.put([0,0,0,0,0,0,0,0])
            continue

        time.sleep(0.3)    # 0.1 сек
        
            
        # чтение контрольных точек  0xE0, 0xE7
        try:
            result = client.read_holding_registers(address=0xE0, count=0x08, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения NET2:")             
                q_NET2.put([0,0,0,0,0,0,0,0])
                continue
            
            print(f"[MASTER] Прочитал NET2: {result.registers}")            
            q_NET2.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса NET2: {e}")
            q_NET2.put([0,0,0,0,0,0,0,0])
            continue

        time.sleep(0.3)    # 0.3 сек

        # чтение контрольных точек   0xE8, 0xEF 
        try:
            result = client.read_holding_registers(address=0xE8, count=0x08, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения NET3:")             
                q_NET3.put([0,0,0,0,0,0,0,0])
                continue
            
            print(f"[MASTER] Прочитал NET3: {result.registers}")            
            q_NET3.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса NET3: {e}")
            q_NET3.put([0,0,0,0,0,0,0,0])
            continue

        time.sleep(0.3)    # 0.3 сек
        
        # чтение контрольных точек   0x140, 0x147 
        try:
            result = client.read_holding_registers(address=0x140, count=0x08, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения NET4:")             
                q_NET4.put([0,0,0,0,0,0,0,0])
                continue
            
            print(f"[MASTER] Прочитал NET4: {result.registers}")            
            q_NET4.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса NET4: {e}")
            q_NET4.put([0,0,0,0,0,0,0,0])
            continue

        time.sleep(0.3)    # 0.3 сек

        # чтение контрольных точек   0x148, 0x14F 
        try:
            result = client.read_holding_registers(address=0x148, count=0x08, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения NET5:")             
                q_NET5.put([0,0,0,0,0,0,0,0])
                continue
            
            print(f"[MASTER] Прочитал NET5: {result.registers}")            
            q_NET5.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса NET5: {e}")
            q_NET5.put([0,0,0,0,0,0,0,0])
            continue

        time.sleep(0.3)    # 0.3 сек


        # чтение Coils точек  0xB0, 0xB7
        try:
            result = client.read_holding_registers(address=0xB0, count=0x08, slave=0x80)
            if  result.isError():                
                print(f"[MASTER] Ошибка чтения Coils1:")             
                q_COILS.put([0,0,0,0,0,0,0,0])
                continue
            
            print(f"[MASTER] Прочитал Coils1: {result.registers}")            
            q_COILS.put(result.registers)

        except Exception as e:
            print(f"[MASTER] Ошибка запроса Coils1: {e}")
            q_COILS.put([0,0,0,0,0,0,0,0])
            continue
           
            
        time.sleep(1)    # 1 сек
        
############################################################################
#  @brief Функция потока имитирующего сеть датчиков ModbusRTU T1, T2, T3, T4, T5
############################################################################
def SLAVE_producer(port_SLAVE, baud, shared_REGS, queue_T1, queue_T2, queue_T3, queue_T4, queue_T5):

    REGS = np.frombuffer(shared_REGS.get_obj(), dtype=np.int16).reshape((ROWS, COLS))
    
    print(f"Процесс SLAVE_producer запущен, PID={os.getpid()}")

    # Настройка COM-порта
    ser = serial.Serial(
#        port='COM13',       
        port = port_SLAVE,
        baudrate=baud,
        bytesize=8,
        parity='N',
        stopbits=1,
        timeout=0.01  # Быстрая реакция
    )

    print("Modbus RTU slave запущен (без modbus-tk)")

    buffer = bytearray()

    MAX_INTERCHAR_TIMEOUT = 0.1  # 3.5 мс для 9600 бод
    interval = 0
    
    while True:
##        print(f"Работаю с портом {port_SLAVE}, скорость {baud}")
        queue_T1.put("test")

        last_byte_time = time.time()
        last_byte_time_period = last_byte_time


        # прием пакета    
        if ser.in_waiting:                   # если пришел хотя бы один байт :  TRUE 
##            last_byte_time = time.time()     # Пример в секундах: 1782734062.45321
##            interval = now - last_byte_time  # интервал в секундах между проходами программы
##            last_byte_time = now

##            if interval > MAX_INTERCHAR_TIMEOUT:
##                # Пауза слишком большая — это новый пакет!
##                if buffer:
##                    print(f"Пакет получен (разрыв после {interval*1000:.2f} мс): {buffer.hex().upper()}")
##                    buffer.clear()
                   
            buffer += ser.read(ser.in_waiting) # чтение байта
            if len(buffer) >= 8:
                # Проверка CRC  пакета AA 0x03 RR RR NN NN CRC16
                packet = buffer[:8]
                received_crc = struct.unpack("<H", packet[6:8])[0]
                calc_crc = crc16(packet[:6])
                if received_crc == calc_crc:
                    parsed = parse_request(packet)   # функция анализа принятого запроса 
                    if parsed:
                        start_addr, qty, slave_id = parsed
                        if REGS[slave_id - 1][8] == 0:
                            response = build_response(slave_id, 0x03, start_addr, qty, REGS)   # функция формирования ответа на запрос 
                            ser.write(response)
                            print(f"Ответ отправлен: {response.hex().upper()}")
                        else:
                            print(f"Ответа нет: REGS[{slave_id - 1}][8] = {REGS[slave_id - 1][8]}")

                buffer.clear()

                
        if interval > MAX_INTERCHAR_TIMEOUT:
            # Пауза слишком большая — это новый пакет!
            if buffer:
                print(f"Пакет получен (разрыв после {interval*1000:.2f} мс): {buffer.hex().upper()}")
                buffer.clear()

        
        time.sleep(0.01)


######################################################################################################################
#    
#  @brief основной процесс программы
#
#         main
#
#######################################################################################################################

if __name__ == "__main__":

    mp.set_start_method("spawn")  # порождать новый
    mp.freeze_support()           # для связывания дочерних процессов
    mp.set_start_method("spawn", force=True)
    
    # Заменяем sys.argv на тестовые значения
    #sys.argv = ['Process_Test.py', '--master', 'COM4', '--slave', 'COM5']

    parser = argparse.ArgumentParser(description="Modbus COM port config")
    parser.add_argument("--master", required=True, help="COM-порт для Master (например, COM3)")
    parser.add_argument("--slave", required=True, help="COM-порт для Slave (например, COM5)")
    
    args = parser.parse_args()
    print(f"master: {args.master}")
    print(f"slave: {args.slave}")    

    last_byte_time = time.time()
    last_byte_time_period = last_byte_time

    MAX_INTERCHAR_TIMEOUT = 0.1  # 3.5 мс для 9600 бод  ???
    PERIOD_TASK = 3  # 3 s
    
    # очереди чтения из контроллера
    q_SP = Queue()
    q_mode = Queue()
    q_data = Queue()
    q_DI = Queue()
    q_DO = Queue()
    q_Alarms = Queue()
    q_AI = Queue()
    q_AO = Queue()
    q_NET = Queue()
    q_NET1 = Queue()
    q_NET2 = Queue()
    q_NET3 = Queue()
    q_NET4 = Queue()
    q_NET5 = Queue()
    q_COILS = Queue()

    # очереди для отдачи данных в контроллер
    queue_T1 = Queue()
    queue_T2 = Queue()
    queue_T3 = Queue()
    queue_T4 = Queue()
    queue_T5 = Queue()

    # --- Очередь команд от клавиатуры ---
    q_cmd = Queue()

    # обявляем массив shared_REGS общий для нескольких процессов
    # 'h' = signed short = int16_t
    shared_REGS = Array('h', ROWS * COLS, lock=True)
    arr_main = np.frombuffer(shared_REGS.get_obj(), dtype=np.int16).reshape((ROWS, COLS))

    # Создаём систему отопления
    Heater_System = module_model.HeaterSystem(
        M1 = module_model.Pamp(),
        water_valve=module_model.Damper_0_100(),
        T1 = module_model.Temperature(),
        T2 = module_model.Temperature(),
        T3 = module_model.Temperature(),
        T4 = module_model.Temperature(),
        T5 = module_model.Temperature(),
        T_out1 = module_model.Temperature(),      # Минимальная температура на улице
        T_water1 = module_model.Temperature(),    # Максимальная температура теплоносителя
        T_out2 = module_model.Temperature(),
        T_water2 = module_model.Temperature(),           
        SetPoint = module_model.Temperature(),
        mode = module_model.Mode(),
        Theat_water = module_model.Temperature(),
        S = 200,    # м^2
        m_In = 2.5,  # масса подаваемогой воды в секунду  2.5 кг / сек
        K = 3,      # (м^2 * °C) / Вт
        m = 500,     # кг
        V = 420,
        Heat_excess = 0,   # избыток тепла в помещении кВт
        C_air = 1005,              # 1005 Дж / (кг * К)
        C_water = 4186,              # 4186 Дж / (кг * К)    
        dt = 3,              # 3 sec
        E = 0.9,            # КПД теплообменника 0.7
        V_In_outside = 0,   # объем воздуха подаваемого с улицы
        V_In_Summ  = 0,     # Суммарный объем поданого воздуха с улицы м3
        Q_Summ = 0,         # Суммарная энергия закчаная через теплообменник в Дж
        W = 0               # мощьность расходуемая на нагрев воздуха       
    )          



    # ввод начальных значений системы HVAC
    while True:
        T5 = module_model.ask_float("введите Т ул: ", -5, -30, 30)
        T4 = module_model.ask_float("введите Т зоны: ", 22, 10, 30)
        T3 = module_model.ask_float("введите Т теплоносителя: ", 70, 40, 90)
        Heater_System.m = 1.2 * module_model.ask_float("Объем помещения: ", 5000, 200, 10000)
        Heater_System.S = round(pow(Heater_System.m/1.2, 2/3) * 6, 2)
        print(f"Площадь стен [м^2] = {Heater_System.S}")
        Heater_System.Heat_excess  = module_model.ask_float("Теплоизбытки в помещении кВт: ", 0, 0, 100)
    
        Heater_System.m_In = module_model.ask_float("Проток воды м^3 / c: ", 2.5, 1, 5)
        Heater_System.E = module_model.ask_float("Коэффициент эффективности теплообменника ε: ", 0.9, 0.7, 1)   

    
        # Спрашиваем действие
        action = input("Нажмите ENTER для запуска или 'r' для повторного ввода   ")
        if action == "r":
            continue  # возвращаемся к началу цикла (как будто goto)
        else:
            print(">>> Запускаем программу с этими параметрами...")
            break

    # Работаем с объектом Heater_System
    Heater_System.M1.status = 0
    Heater_System.water_valve.position = 0
    
    #  определяем начальные температуры
    Heater_System.T1.value = T4
    Heater_System.T2.value = T4
    Heater_System.T3.value = 70
    Heater_System.T4.value = T4
    Heater_System.T5.value = T5
    Heater_System.SetPoint.value = 10
    
    Heater_System.mode.mode = 3
    Heater_System.Theat_water.value = 70        


    #########################################################################################
    print("Logging initialized")

    # Создаём логгер
    logger = logging.getLogger("ventilation_logger")
    logger.setLevel(logging.INFO)

    # 1️Handler для табличного лога
    file_handler_table = logging.FileHandler("HVAC1_table.log", encoding="utf-8")
    file_handler_table.setLevel(logging.INFO)
    formatter_table = logging.Formatter("%(message)s")  # только сообщение
    file_handler_table.setFormatter(formatter_table)

    # 2️Handler для ошибок
    file_handler_error = logging.FileHandler("HVAC1_error.log", encoding="utf-8")
    file_handler_error.setLevel(logging.ERROR)
    formatter_error = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s")
    file_handler_error.setFormatter(formatter_error)

    # Добавляем обработчики к логеру
    logger.addHandler(file_handler_table)   
    logger.addHandler(file_handler_error)

    #########################################################################################
    

    # Заголовок таблицы (пишем только один раз)
    log_system_state_Head_HVAC1()


   
    # процесс  читает из контроллера
    Master_COM = Process(
        target=MASTER_producer,
        args=(args.master, 38400, q_SP, q_mode, q_data, q_DI, q_DO, q_Alarms, q_AI, q_AO, q_NET, q_NET1, q_NET2, q_NET3, q_NET4, q_NET5, q_COILS)
    )
    
    Master_COM.start()

    print("Главный процесс запущен, PID=", os.getpid())

    # процесс  имитирует сеть датчиков
    Slave_COM = Process(
        target=SLAVE_producer,
        args=(args.slave, 38400, shared_REGS, queue_T1, queue_T2, queue_T3, queue_T4, queue_T5)
    )

    Slave_COM.start()

    # Запуск потока клавиатуры
    t_key = threading.Thread(target=keyboard_thread, daemon=True)
    t_key.start()

    # заносим эти значения в регистры на выдачу контроллеру   
    arr_main[0][0] = int(Heater_System.T1.value * 10)
    arr_main[1][0] = int(Heater_System.T2.value * 10)
    arr_main[2][0] = int(Heater_System.T3.value * 10)
    arr_main[3][0] = int(Heater_System.T4.value * 10)
    arr_main[4][0] = int(Heater_System.T5.value * 10)

    SP = [0] * 15
    mode = [0] * 5
    DI =[0] * 1
    DO = [0] * 1
    Alarms = [0] * 1
    AI = [0] * 15
    AO = [0] * 15
    NET0 = [0] * 8
    NET1 = [0] * 8
    NET2 = [0] * 8
    NET3 = [0] * 8
    NET4 = [0] * 8
    NET5 = [0] * 8
    NET0 = [0] * 8    
    COILS1 = [0] * 8
    
    # Цикл процесса Main
    while True:

        #переодические операции
        now_period = time.time()
        interval_period = now_period - last_byte_time_period
        if interval_period > PERIOD_TASK:   # 3s
            for i in range(5):
                print(f"arr_main[{i}][0]: {arr_main[i][0]}")

##            arr_main[0][0] += 1
##            print("Main:", arr_main[0][0])
##            print("now_period:", now_period)
##            print("last_byte_time_period:", last_byte_time_period)
##            print("interval_period:", interval_period)

            # чтение из процесса Master
            #получение данных от рабочего потока Уст.1 0x20 - 0x2E 
            if not q_SP.empty():
                SP = q_SP.get_nowait()
                print(f"Get SP_queue: {SP}")
                SP0_type = type(SP[0])

            #получение от рабочего потока mode 0x48            
            if not q_mode.empty():
                mode = q_mode.get_nowait()
                print(f"Read queue_data  mode = : {mode}")

            #получение от рабочего потока DI 0x0F
            if not q_DI.empty():
                DI = q_DI.get_nowait()
                print(f"Read DI_queue: {DI}")                

            #получение от рабочего потока DO 0x2F
            if not q_DO.empty():
                DO = q_DO.get_nowait()
                print(f"Read DO_queue: {DO}")

            #получение от рабочего потока Alarms 0x1F
            if not q_Alarms.empty():
                Alarms = q_Alarms.get_nowait()
                print(f"Read Alarms_queue: {Alarms}")

            #получение от рабочего потока AI 0x10, 0x11, 0x12
            if not q_AI.empty():
                AI = q_AI.get_nowait()
                print(f"Get AI_queue: {AI}")
                AI0_type = type(AI[0])

            #получение от рабочего потока AO 0x30, 0x31, 0x32
            if not q_AO.empty():
                AO = q_AO.get_nowait()
                print(f"Get AO_queue: {AO}")
                AO0_type = type(AO[0])

            #получение от рабочего потока NET 0xD0 ... 0xD7
            if not q_NET.empty():
                NET0 = q_NET.get_nowait()
                print(f"Get NET_queue: {NET0}")

            #получение от рабочего потока NET 0xD8 ... 0xDF
            if not q_NET1.empty():
                NET1 = q_NET1.get_nowait()
                print(f"Get NET_queue1: {NET1}")

            #получение от рабочего потока NET 0xE0 ... 0xE7
            if not q_NET2.empty():
                NET2 = q_NET2.get_nowait()
                print(f"Get NET_queue2: {NET2}")

            #получение от рабочего потока NET 0xE8 ... 0xEF
            if not q_NET3.empty():
                NET3 = q_NET3.get_nowait()
                print(f"Get NET_queue3: {NET3}")

            #получение от рабочего потока NET 0xF0 ... 0xF7
            if not q_NET4.empty():
                NET4 = q_NET4.get_nowait()
                print(f"Get NET_queue4: {NET4}")

            #получение от рабочего потока NET 0xF8 ... 0xFF
            if not q_NET5.empty():
                NET5 = q_NET5.get_nowait()
                print(f"Get NET_queue5: {NET5}")                

            #получение от рабочего потока Coils 0xB0 ... 0xB7
            if not q_COILS.empty():
                COILS1 = q_COILS.get_nowait()
                print(f"Get COILS_queue1: {COILS1}")


            # чтение из потока  Команд клавиатуры
            if not q_cmd.empty():
                

                cmd = q_cmd.get()
                print(f"MAIN получил команду: {cmd}")

                if cmd == "t":
                    Heater_System.T5.value -=  1
                elif cmd == "g":
                    Heater_System.T5.value += 1
                elif cmd == "e1":
                    arr_main[0][8] = 1
                elif cmd == "e2":
                    arr_main[1][8] = 1
                elif cmd == "e3":
                    arr_main[2][8] = 1
                elif cmd == "e4":
                    arr_main[3][8] = 1
                elif cmd == "e5":
                    arr_main[4][8] = 1
                elif cmd == "f1":
                    arr_main[0][9] = 1
                elif cmd == "f2":
                    arr_main[1][9] = 1
                elif cmd == "f3":
                    arr_main[2][9] = 1
                elif cmd == "f4":
                    arr_main[3][9] = 1
                elif cmd == "f5":
                    arr_main[4][9] = 1                   
                elif cmd == "n1":
                    arr_main[0][8] = 0
                    arr_main[0][9] = 0
                elif cmd == "n2":
                    arr_main[1][8] = 0
                    arr_main[1][9] = 0                    
                elif cmd == "n3":
                    arr_main[2][8] = 0
                    arr_main[2][9] = 0
                elif cmd == "n4":
                    arr_main[3][8] = 0
                    arr_main[3][9] = 0
                elif cmd == "n5":
                    arr_main[4][8] = 0
                    arr_main[4][9] = 0                    
                elif cmd == "wl":
                    if Heater_System.Heat_excess == 0:
                        Heater_System.Heat_excess = 0
                    else:
                        Heater_System.Heat_excess -= 1
                elif cmd == "wg":
                        Heater_System.Heat_excess += 1                                       
                 
            last_byte_time_period = now_period

            # заносим считанные и вычисленные параметры в структуру Fancoil
            Heater_System.M1.status = DI[0] & 1
            Heater_System.water_valve.position = AI[0]
            Heater_System.mode.mode = mode[0]
            Heater_System.T_out1.value = SP[0]#-40 #SP[0]
            Heater_System.T_water1.value = SP[1]#80 #SP[1]
            Heater_System.T_out2.value = SP[2]#8 #SP[2]
            Heater_System.T_water2.value = SP[3]#40 #SP[3]   


            # ++++ вычисляем температуры
            module_model.TemperatureCalc_Water_SP(Heater_System)
            module_model.TemperatureCalc_HeaterSystem(Heater_System)   # расчет температур подаваемой воды и обратки
            module_model.TemperatureHallCalc_Heater(Heater_System)     # расчет температуры в помещении



            # обновляем регстры температур
            arr_main[0][0] = int(Heater_System.T1.value * 10)
            arr_main[1][0] = int(Heater_System.T2.value * 10)
            arr_main[2][0] = int(Heater_System.T3.value * 10)
            arr_main[3][0] = int(Heater_System.T4.value * 10)
            arr_main[4][0] = int(Heater_System.T5.value * 10)

            
            # Логируем текущее состояние
            log_system_state_HVAC1(Heater_System)

#---------------------------------------------------------------------------
            # пишем в сокет
            sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

            print("Модель запущена. Отправка данных каждые 3 секунды...")
                               
            data = {
                "T1": Heater_System.T1.value,
                "T2": Heater_System.T2.value,
                "T3": Heater_System.T3.value,
                "T4": Heater_System.T4.value,
                "T5": Heater_System.T5.value,
                "T_SP": Heater_System.SetPoint.value,
                "co2": 0,
                "co2_SP": 0,
                "air_damper": 0,#Fancoil.air_damper.position,
                "water_damper": Heater_System.water_valve.position,
                "M1": Heater_System.M1.status * 100,
                "V_In_Summ": 0, #Fancoil.V_In_Summ,       # Суммарный объем поданого воздуха с улицы м3
                "Q_Summ": 0, #Fancoil.Q_Summ,          # Суммарная энергия закчаная через теплообменник в Дж
                "mode": Heater_System.mode.mode,   # режим
                "V_room": Heater_System.m,                # объем помещения
                "Heat_excess": 0, # Fancoil.Heat_excess, # избыток тепла
                "men": 0, # Fancoil.men,                  # количество людей
                "V_In_outside": 0, #Fancoil.V_In_outside,       # объем подаваемого с улицы воздуха
                "W": 0 # Fancoil.W                      # мощьность расходуемая на изменение температуры воздуха
            }

            for i in range(8):
                data[f"R{i}"] = NET0[i]   

            for i in range(8):
                data[f"R{i+8}"] = NET1[i]

            for i in range(8):
                data[f"R{i+16}"] = NET2[i]    

            for i in range(8):
                data[f"R{i+24}"] = NET3[i]

            for i in range(8):
                data[f"R{i+112}"] = NET4[i]
                
            for i in range(8):
                data[f"R{i+120}"] = NET5[i]
                
            for i in range(8):
                data[f"C{i}"] = COILS1[i]
                
            for i in range(3):
                data[f"AI{i}"] = AI[i]
                
            for i in range(3):
                data[f"AO{i}"] = AO[i]                 
                
            msg = json.dumps(data).encode("utf-8")
            sock.sendto(msg, ("localhost", 9999))  # по строкам, для удобства
            print("Отправлено:", msg)
#--------------------------------------------------------------------------            
       
        time.sleep(0.01)
    
 
    print("Главный процесс завершает работу.")


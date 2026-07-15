#!module_model.py

from dataclasses import dataclass

@dataclass
class Fan:
    status: int = 0   # 1 = "ON";  0 = "OFF"

@dataclass
class Damper:
    status: int = 0     # 1 = "ON";  0 = "OFF"

@dataclass
class Damper_0_100:
    """
    @brief Класс задвижки
    """
    position: float = 0     # 0–100 %

@dataclass
class Filter:
    status: int = 0   # 1 = "Засорен";  0 = "Чистый"

@dataclass
class Recuperator:
    status: int = 0   # 1 = "Чистый";  0 = "Замерз!"

@dataclass
class Mode:
    mode: int = 0   # 0 = "ручной";  1 = "лето";  2 = "переходный";  3 =  "зима"

    
@dataclass
class Temperature:
    value: float = 0     # –55 -   +125 градусов

@dataclass
class CO2:
    value: float = 400     # 400 -   10000 ppm


@dataclass
class Pamp:
    status: int = 0   # 1 = "ON";  0 = "OFF"    
    
@dataclass
class VentilationSystem:
    supply_fan: Fan
    exhaust_fan: Fan
    supply_damper: Damper
    exhaust_damper: Damper
    water_damper: Damper_0_100
    TEN: Damper_0_100
    bypass_damper: Damper_0_100
    filter_In:   Filter
    filter_Out:   Filter
    recuperator: Recuperator
    SetPoint:   Temperature    # Уставка температуры
    T1:   Temperature
    T2:   Temperature
    T3:   Temperature
    T4:   Temperature
    T5:   Temperature
    Theat_water:  Temperature  # температура теплоносителя зимой
    Tcool_water:  Temperature  # температура теплоносителя летом
    mode:  Mode
    S:    float                # Площадь стен помещения   200 м^2
    K:    float                # тепловое сопротивление 3 ( м^2 * K ) / Вт
    m:    float                # масса  воздуха в помещении
    Heat_excess:  float        # избыток тепла в помещении кВт
    m_In:    float             # масса подаваемого воздуха в секунду  0.5  кг / c
    V_In:    float            # объем  подаваемого воздуха в секунду  0.5  м^3 / c
    C_air:    float              # 1005 Дж / (кг * К)
    dt:    float              # 3 sec
    E:     float              # КПД рекуператора 0.7
    V:     float 
    V_In_outside:  float    # объем воздуха подаваемого с улицы
    V_In_Summ:     float    # Суммарный объем поданого воздуха с улицы м3
    Q_Summ:        float    # Суммарная энергия закчаная через теплообменник в Дж
    W:           float      # мощьность расходуемая на нагрев воздуха
    men:        float       # количество людей

@dataclass
class HeaterSystem:
    M1: Pamp
    water_valve: Damper_0_100
    T1:   Temperature
    T2:   Temperature
    T3:   Temperature
    T4:   Temperature
    T5:   Temperature
    T_out1: Temperature      # Минимальная температура на улице
    T_water1: Temperature    # Максимальная температура теплоносителя
    T_out2: Temperature
    T_water2: Temperature    
    SetPoint:   Temperature
    Theat_water:  Temperature  # температура теплоносителя зимой
    mode:  Mode
    S:    float                # Площадь стен помещения   200 м^2
    K:    float                # тепловое сопротивление 3 ( м^2 * K ) / Вт
    m:    float                # масса  воздуха в помещении
    V:    float                # Объем  воздуха в помещении
    Heat_excess:  float        # избыток тепла в помещении кВт
    m_In:    float             # масса подаваемой воды в секунду  0.0025  м^3 / c
    C_air:    float             # 1005 Дж / (кг * К)
    C_water:    float           # 4186 Дж / (кг * К)
    dt:    float               # 3 sec
    E:     float               # КПД теплообменника 0.9
    V_In_outside:  float    # объем воздуха подаваемого с улицы
    V_In_Summ:     float    # Суммарный объем поданого воздуха с улицы м3
    Q_Summ:        float    # Суммарная энергия закчаная через теплообменник в Дж
    W:           float      # мощьность расходуемая на нагрев воздуха

@dataclass
class FancoilSystem:
    supply_fan: Fan
    water_damper: Damper_0_100   # Y3
    air_damper: Damper_0_100  # Y1
    filter_In:   Filter
    T1:   Temperature
    T2:   Temperature
    T3:   Temperature
    T4:   CO2
    T5:   Temperature
    T_SP: int
    dT_SP: int                 # зона нечуствительности уставки к изменениям температуры
    CO2_SP: int
    Theat_water:  Temperature  # температура теплоносителя зимой
    Tcool_water:  Temperature  # температура теплоносителя летом
    mode:  Mode
    answer_modbus: int         # регистр  датчик ответил = 1,  датчик неответил = 0
    S:    float                # Площадь стен помещения   200 м^2
    K:    float                # тепловое сопротивление 3 ( м^2 * K ) / Вт
    m:    float                # масса  воздуха в помещении
    V:    float                # объем  воздуха в помещении
    Heat_excess:  float        # избыток тепла в помещении кВт
    m_In:    float                # масса подаваемого воздуха в секунду  0.5  кг / c
    V_In:    float              # объем подаваемого воздуха в секунду  0.5  м^3 / c
    C_air:    float              # 1005 Дж / (кг * К)
    C_water:    float           # 4186 Дж / (кг * К)
    dt:    float              # 3 sec
    E:     float              # КПД теплообменника 0.9
    men:   float              # количество людей в помещении
    m_In_outside:  float    # масса воздуха подаваемого с улицы
    
    V_In_outside:  float    # объем воздуха подаваемого с улицы
    V_In_Summ:     float    # Суммарный объем поданого воздуха с улицы м3
    Q_Summ:        float    # Суммарная энергия закчаная через теплообменник в Дж
    W:           float      # мощьность расходуемая на нагрев воздуха
    
    CO2_gen: float          # кол-во СО2 генерируемое  одним человеком в минуту 0.3 л / (мин * чел)
    V_CO2:   float          # объем СО2 в помещении
    dCO2_men: float
    CO2_ppm:  float         # концентрация в помещении
    dQ: float
    net1: int               # отладочная информация 0x09
    net2: int               # 0x0A
    net3: int               # 0x0B
    net4: int               # 0x0C
    net5: int               # 0x0D
    
    
###################################
# @brief функция вычисления температуры на выходе пластинчатого рекуператора воздух/воздух
#
# @param VentilationSystem структура вентиляционной системы
# @return VentilationSystem структура вентиляционной системы
#  
################################### 
def Recuperator_air_air_Calc(nB: VentilationSystem):
    if nB.supply_damper.status == 1 and nB.exhaust_damper.status == 1 and nB.supply_fan.status == 1 and nB.exhaust_fan.status == 1:
        T1 = nB.T5.value + 0.95*(nB.T3.value - nB.T5.value)  # расчетное значение тем-ры
        T4 = T1                                              # расчетное значение тем-ры
    elif nB.supply_damper.status == 0 and nB.exhaust_damper.status == 1 and nB.supply_fan.status == 0 and nB.exhaust_fan.status == 1:
        T1 = nB.T1.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
    else:
        T1 = nB.T3.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
        
    # стремление текущей температуры к расчетной
    if nB.T1.value < T1:
        nB.T1.value = nB.T1.value + 1
    if nB.T1.value > T1:
        nB.T1.value = nB.T1.value - 1 

    if nB.T4.value < T4:
        nB.T4.value = nB.T4.value + 1
    if nB.T4.value > T4:
        nB.T4.value = nB.T4.value - 1         


###################################
#  функция вычисления температуры
#  на выходе пластинчатого рекуператора
#  воздух/воздух с учетом байпаса
#  
###################################
def Recuperator_air_air_bypass_Calc(nB: VentilationSystem):
    if nB.supply_damper.status == 1 and nB.exhaust_damper.status == 1 and nB.supply_fan.status == 1 and nB.exhaust_fan.status == 1:
        Tc_out_core = nB.T5.value + nB.E * (nB.T3.value - nB.T5.value)  # расчетное значение тем-ры
#        T4 = nB.T3.value -  ((100 - nB.bypass_damper.position)/ 100) * (Tc_out_core -  nB.T5.value)    # расчетное значение тем-ры
        T4 = nB.T3.value -  ((100 - nB.bypass_damper.position)/ 100) * Tc_out_core                      # расчетное значение тем-ры
        T1 = ((100 - nB.bypass_damper.position) / 100) * Tc_out_core + (1 - ((100 - nB.bypass_damper.position) / 100)) * nB.T5.value
        
        
    elif nB.supply_damper.status == 0 and nB.exhaust_damper.status == 1 and nB.supply_fan.status == 0 and nB.exhaust_fan.status == 1:
        T1 = nB.T3.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
        
    elif nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and nB.exhaust_fan.status == 0:
        T1 = nB.T5.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
        
    else:
        T1 = nB.T3.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
        
    # стремление текущей температуры к расчетной
    if nB.T1.value < T1:
        nB.T1.value = nB.T1.value + 1
    if nB.T1.value > T1:
        nB.T1.value = nB.T1.value - 1 

    if nB.T4.value < T4:
        nB.T4.value = nB.T4.value + 1
    if nB.T4.value > T4:
        nB.T4.value = nB.T4.value - 1        

###################################
#  функция вычисления температуры
#  на выходе пластинчатого рекуператора
#  воздух/воздух с учетом байпаса
#  новая правильная версия
###################################
def Recuperator_air_air_bypass_Calc_new(nB: VentilationSystem):
    if nB.supply_damper.status == 1 and nB.exhaust_damper.status == 1 and nB.supply_fan.status == 1 and nB.exhaust_fan.status == 1:
#        Tc_out_core = nB.T5.value + nB.E * (nB.T3.value - nB.T5.value)  # расчетное значение тем-ры
#        T4 = nB.T3.value -  ((100 - nB.bypass_damper.position)/ 100) * (Tc_out_core -  nB.T5.value)    # расчетное значение тем-ры
#        T4 = nB.T3.value -  ((100 - nB.bypass_damper.position)/ 100) * Tc_out_core                      # расчетное значение тем-ры
#        T1 = ((100 - nB.bypass_damper.position) / 100) * Tc_out_core + (1 - ((100 - nB.bypass_damper.position) / 100)) * nB.T5.value

        # bypass = доля притока, минующая рекуператор (0..1)
        b = nB.bypass_damper.position / 100
        f = 1.0 - b  # доля через ядро
        T_c_in = nB.T5.value
        T_h_in = nB.T3.value
        # теплоёмкости пропорциональны расходам -> C_c/C_h = f/1
        # холодный выход из ядра (с учётом эффективности eps)
        T_c_out_core = T_c_in + nB.E * (T_h_in - T_c_in)
        # энергия: C_c*(T_c_out_core - T_c_in) = C_h*(T_h_in - T_h_out)
        # нормируем на C_h -> f*(T_c_out_core - T_c_in) = (T_h_in - T_h_out)
        T4 = T_h_in - f * (T_c_out_core - T_c_in)
        # смешиваем приток после ядра и байпас
        T1 = f * T_c_out_core + b * T_c_in
        
        
    elif nB.supply_damper.status == 0 and nB.exhaust_damper.status == 1 and nB.supply_fan.status == 0 and nB.exhaust_fan.status == 1:
        T1 = nB.T3.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
        
    elif nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and nB.exhaust_fan.status == 0:
        T1 = nB.T5.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
        
    else:
        T1 = nB.T3.value                                     # расчетное значение тем-ры
        T4 = nB.T3.value
        
    # стремление текущей температуры к расчетной
    if nB.T1.value < T1:
        nB.T1.value = nB.T1.value + 1
    if nB.T1.value > T1:
        nB.T1.value = nB.T1.value - 1 

    if nB.T4.value < T4:
        nB.T4.value = nB.T4.value + 1
    if nB.T4.value > T4:
        nB.T4.value = nB.T4.value - 1        


###################################
#  функция вычисления температуры
#  на теплообменник
#  вода/воздух 
#  
###################################
def HeatExcanger_water_air_Calc(nB: VentilationSystem):
     
    if nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and ((nB.mode.mode == 3) or (nB.mode.mode == 0)):  # mode = heat or mode = hand
        if nB.T1.value + (20*nB.water_damper.position)/100 < nB.Theat_water.value:
            T2 = nB.T1.value + (20*nB.water_damper.position)/100                         # расчетное значение тем-ры
            print(f"T2_Heat_plus {T2}")
        else:
            T2 = nB.Theat_water.value
            print(f"T2_Heat_minus {T2}")
                
    elif nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and nB.mode.mode == 1:  # mode = cool
        if nB.T1.value - (10*nB.water_damper.position)/100 > nB.Tcool_water.value:
            T2 = nB.T1.value - (10*nB.water_damper.position)/100                         # расчетное значение тем-ры
            print(f"T2_Cool_plus {T2}")
        else:
            T2 = nB.Tcool_water.value
            print(f"T2_Cool_minus {T2}")

    elif nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and nB.mode.mode == 2:  # mode = fan
        T2 = nB.T1.value                         # расчетное значение тем-ры
                                        
    else:                                                                                   # приточный вентилятор остановлен
        T2 = nB.T3.value                       # расчетное значение тем-ры

        
    # стремление текущей температуры к расчетной
    if nB.T2.value < T2:
        nB.T2.value = nB.T2.value + 1
    if nB.T2.value > T2:
        nB.T2.value = nB.T2.value - 1 


###################################
#  функция вычисления температуры
#  на выходе канального кондиционера летом
#  
#  
###################################
def HeatExcanger_Freon_air_Calc(nB: VentilationSystem):
     
    if nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and nB.mode.mode == 1:  # mode = лето
        print(f"T3: {nB.T3.value}")
        print(f"SetPoint: {nB.SetPoint.value}")
        if nB.T3.value  > nB.SetPoint.value:
            if nB.T1.value - 10 > 12:
                T2 = nB.T1.value - 10    # расчетное значение тем-ры
                print(f"Cool conditioner {T2}")
            else:
                T2 = 12
                print(f"Cool conditioner {T2}")
        else:
            T2 = nB.T1.value
                    
        # стремление текущей температуры к расчетной
        if nB.T2.value < T2:
            nB.T2.value = nB.T2.value + 1
        if nB.T2.value > T2:
            nB.T2.value = nB.T2.value - 1

    elif nB.supply_fan.status == 0:
        T2 = nB.T1.value

        if nB.T2.value < T2:
            nB.T2.value = nB.T2.value + 1
        if nB.T2.value > T2:
            nB.T2.value = nB.T2.value - 1
        
        

        
###################################
#  функция вычисления температуры
#  на выходе электрокалорифера 
#  
#  
###################################
def HeatExchanger_TEN_air_Calc(nB: VentilationSystem):
           
##    if nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and ((nB.mode.mode == 3) or (nB.mode.mode == 0)):  # mode = heat or mode = hand
##        if nB.T1.value + (50*nB.water_damper.position)/100 < nB.Theat_water.value:
##            T2 = nB.T1.value + (50*nB.water_damper.position)/100                         # расчетное значение тем-ры
##            print(f"T2_Heat_plus {T2}")
##        else:
##            T2 = nB.Theat_water.value
##            print(f"T2_Heat_minus {T2}")
##
##    elif nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and nB.mode.mode == 2:  # mode = fan
##        T2 = nB.T1.value                         # расчетное значение тем-ры
##                                        
##    else:                                                                                   # приточный вентилятор остановлен
##        T2 = nB.T3.value                       # расчетное значение тем-ры
##
##        
##    # стремление текущей температуры к расчетной
##    if nB.T2.value < T2:
##        nB.T2.value = nB.T2.value + 1
##    if nB.T2.value > T2:
##        nB.T2.value = nB.T2.value - 1 

    if nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and ((nB.mode.mode == 3) or (nB.mode.mode == 0)):  # mode = heat or mode = hand
        T2 = nB.T1.value + (50*nB.TEN.position)/100                         # расчетное значение тем-ры
    elif nB.supply_damper.status == 1 and nB.supply_fan.status == 1 and nB.mode.mode == 2:  # mode = fan
        T2 = nB.T1.value                         # расчетное значение тем-ры                                        
    else:                                                                                   # приточный вентилятор остановлен
        T2 = nB.T3.value                       # расчетное значение тем-ры

        
    # стремление текущей температуры к расчетной
    if nB.T2.value < T2:
        nB.T2.value = nB.T2.value + 1
    if nB.T2.value > T2:
        nB.T2.value = nB.T2.value - 1 

        
###################################
#  функция вычисления температуры
#  на выходе теплообменника 
#  вода/воздух Фанкойла
#  
###################################
def HeatExchanger_Fancoil_water_air_Calc(fancoil: FancoilSystem):
           
    if fancoil.supply_fan.status == 1 and ((fancoil.mode.mode == 3) or (fancoil.mode.mode == 0)):  # mode = heat or mode = hand
        if fancoil.T1.value + (20*fancoil.water_damper.position)/100 < fancoil.Theat_water.value:
            T2 = fancoil.T1.value + (20*fancoil.water_damper.position)/100                         # расчетное значение тем-ры
            print(f"T2_Heat_plus {T2}")
        else:
            T2 = fancoil.Theat_water.value
            print(f"T2_Heat_minus {T2}")
                
    elif fancoil.supply_fan.status == 1 and fancoil.mode.mode == 1:  # mode = cool
        if fancoil.T1.value - (10*fancoil.water_damper.position)/100 > fancoil.Tcool_water.value:
            T2 = fancoil.T1.value - (10*fancoil.water_damper.position)/100                         # расчетное значение тем-ры
            print(f"T2_Cool_plus {T2}")
        else:
            T2 = fancoil.Tcool_water.value
            print(f"T2_Cool_minus {T2}")

    elif  fancoil.supply_fan.status == 1 and fancoil.mode.mode == 2:  # mode = fan
        T2 = fancoil.T1.value                         # расчетное значение тем-ры
                                        
    else:                                                                                   # приточный вентилятор остановлен
        T2 = fancoil.T3.value                       # расчетное значение тем-ры

        
    # стремление текущей температуры к расчетной
    if fancoil.T2.value < T2:
        fancoil.T2.value = fancoil.T2.value + 1
    if fancoil.T2.value > T2:
        fancoil.T2.value = fancoil.T2.value - 1 
        
###################################
#  функция вычисления температуры
#  на выходе теплообменника 
#  вода/воздух Фанкойла
#  
###################################
def HeatExchanger_Fancoil_TEN_air_Calc(fancoil: FancoilSystem):
           
    if fancoil.supply_fan.status == 1:  # mode = heat or mode = hand
        if fancoil.T1.value + (30*fancoil.water_damper.position)/100 < fancoil.Theat_water.value:
            T2 = fancoil.T1.value + (30*fancoil.water_damper.position)/100                         # расчетное значение тем-ры
            print(f"T2_Heat_plus {T2}")
        else:
            T2 = fancoil.Theat_water.value
            print(f"T2_Heat_minus {T2}")
                                                        
    else:                                                                                   # приточный вентилятор остановлен
        T2 = fancoil.T3.value                       # расчетное значение тем-ры

        
    # стремление текущей температуры к расчетной
    if fancoil.T2.value < T2:
        fancoil.T2.value = fancoil.T2.value + 1
    if fancoil.T2.value > T2:
        fancoil.T2.value = fancoil.T2.value - 1

        
###################################
#  Fancoil система
#
#  функция вычисления температуры T1
#  при смешении уличного и рециркуляционного воздуха 
#  
#  
###################################
def Air_Damper_Fancoil_Temperature_Calc(fancoil: FancoilSystem):
           
##    if fancoil.supply_fan.status == 1 and ((fancoil.mode.mode == 3) or (fancoil.mode.mode == 0)):  # mode = heat or mode = hand
##        if fancoil.T1.value + (20*fancoil.water_damper.position)/100 < fancoil.Theat_water.value:
##            T2 = fancoil.T1.value + (20*fancoil.water_damper.position)/100                         # расчетное значение тем-ры
##            print(f"T2_Heat_plus {T2}")
##        else:
##            T2 = fancoil.Theat_water.value
##            print(f"T2_Heat_minus {T2}")
##                
##    elif fancoil.supply_fan.status == 1 and fancoil.mode.mode == 1:  # mode = cool
##        if fancoil.T1.value - (10*fancoil.water_damper.position)/100 > fancoil.Tcool_water.value:
##            T2 = fancoil.T1.value - (10*fancoil.water_damper.position)/100                         # расчетное значение тем-ры
##            print(f"T2_Cool_plus {T2}")
##        else:
##            T2 = fancoil.Tcool_water.value
##            print(f"T2_Cool_minus {T2}")
##
##    elif  fancoil.supply_fan.status == 1 and fancoil.mode.mode == 2:  # mode = fan
##        T2 = fancoil.T1.value                         # расчетное значение тем-ры
##                                        
##    else:                                                                                   # приточный вентилятор остановлен
##        T2 = fancoil.T3.value                       # расчетное значение тем-ры

    T1 = (fancoil.air_damper.position * fancoil.T5.value)/100 + ((100 - fancoil.air_damper.position) * fancoil.T3.value)/100 
        
    # стремление текущей температуры к расчетной
    if fancoil.T1.value < T1:
        fancoil.T1.value = fancoil.T1.value + 1
    if fancoil.T1.value > T1:
        fancoil.T1.value = fancoil.T1.value - 1 
        
#-------------------------------------------------

###################################
#  Fancoil система
#
#  функция вычисления концентрации СО2 
#  в зависимости от количества людей и объема подаваемого воздуха 
#  
#  
###################################
##def Air_Damper_Fancoil_CO2_Calc(fancoil: FancoilSystem):
##
##    # Генерация СО2 людьми за 3 сек  [м3]
##    fancoil.dCO2_men = ((fancoil.CO2_gen * 3) / 60) *  fancoil.men
##    fancoil.V_CO2 = fancoil.V_CO2 + fancoil.dCO2_men
##
##    if fancoil.supply_fan.status == 1:
##        fancoil.V_In = (fancoil.air_damper.position * fancoil.V * 3) / 100  # на столько объем увеличился
##        fancoil.dQ = fancoil.V_In * 400 * 0.000001 # Добавилось СО2 с улицы
##    else:
##        fancoil.V_In  = 0
##        fancoil.dQ = 0
##
##    fancoil.T4.value = int((fancoil.V_CO2 + fancoil.dQ) * 1000000 / (fancoil.V + fancoil.V_In))

    
        
        
##    if fancoil.supply_fan.status == 1:
##        fancoil.V_In = (fancoil.air_damper.position * fancoil.V * 3) / 100
##        fancoil.dQ = fancoil.V_In * (fancoil.T4.value - 400) * 0.000001
##    else:
##        fancoil.dQ = 0

    
    #fancoil.T4.value = int(fancoil.T4.value + ((fancoil.dCO2_men -  fancoil.dQ) * 1000000) / fancoil.V )  
##    fancoil.T4.value = int(fancoil.T4.value + (fancoil.dCO2_men * 1000000 -  fancoil.dQ * 1000000) / fancoil.V )  

    # расчет концентрации СО2 в помещении в ppm
    # fancoil.T4.value = int((fancoil.V_CO2 * 1000000)/ (fancoil.m / 1.2))

    #fancoil.T4.value = int(fancoil.V_CO2 * 1000000 / fancoil.V)
    #fancoil.T4.value = fancoil.T4.value + 1


#-------------------------------------------------

###################################
#  Fancoil система
#
#  функция вычисления концентрации СО2 
#  в зависимости от количества людей и объема подаваемого воздуха 
#  v.2
#  
###################################
def Air_Damper_Fancoil_CO2_Calc(fancoil: FancoilSystem):

    # Генерация СО2 людьми за 3 сек  [м3]
    fancoil.dCO2_men = ((fancoil.CO2_gen * 3) / 60) *  fancoil.men
    fancoil.V_CO2 = fancoil.V_CO2 + fancoil.dCO2_men

    value_ppm1 = (fancoil.V_CO2 * 1000000) / fancoil.V   # концентрация СО2 в ppm сгенерированная людьми

    # Подача воздуха с улицы с концентрацией 400 ppm за 3 сек  [м3]
    if fancoil.supply_fan.status == 1:
        V_ppm2 = (fancoil.air_damper.position * fancoil.V_In * 3) / 100  # объем воздуха поданный с улицы за 3 сек с концентрацией ppm2
        V_ppm1 = fancoil.V - V_ppm2                                      # объем воздуха оставшийся в помещении через 3 сек с концентрацией ppm1
    else:
        V_ppm2 = 0
        V_ppm1 = fancoil.V

        
    # вычисление текущей концентрации
    fancoil.CO2_ppm = (V_ppm2 * 400 + V_ppm1 * value_ppm1)/fancoil.V
    fancoil.T4.value = int((V_ppm2 * 400 + V_ppm1 * value_ppm1)/fancoil.V)

    fancoil.V_CO2 = (fancoil.CO2_ppm * fancoil.V)/1000000  # новый объем СО2 с учетом проветривания 

#-------------------------------------------------
#-------------------------------------------------

###################################
#  Fancoil система
#
#  функция вычисления общеего объема поданого  наружного воздуха 
#   
#  v.1
#  
###################################
def Air_In_Summ(fancoil: FancoilSystem):

    # Подача воздуха с улицы[м3]
    if fancoil.supply_fan.status == 1:
        V_In = (fancoil.air_damper.position * fancoil.V_In * 3) / 100  # объем воздуха поданный с улицы за 3 сек
        fancoil.V_In_outside = V_In * 1200

    else:
        V_In = 0
        fancoil.V_In_outside = 0

        
    # вычисление сумарно поданого наружного воздуха
    fancoil.V_In_Summ = fancoil.V_In_Summ + V_In

#-------------------------------------------------

###################################
#  nB1 система
#
#  функция вычисления общеего объема поданого  наружного воздуха 
#   
#  v.1
#  
###################################
def Air_In_Summ_nB1(nB: VentilationSystem):

    # Подача воздуха с улицы[м3]
    if nB.supply_fan.status == 1  and nB.supply_damper.status == 1:
        V_In = nB.V_In * 3     # объем воздуха поданный с улицы за 3 сек

    else:
        V_In = 0
    
    # вычисление сумарно поданого наружного воздуха
    nB.V_In_Summ = nB.V_In_Summ + V_In

#-------------------------------------------------

    
#-------------------------------------------------

###################################
#  Fancoil система
#
#  функция вычисления затраченой энергии на подогрев  воздуха 
#   
#  v.1
#  
###################################
def Q_Summ(fancoil: FancoilSystem):

    # вычисление dT для (зима + руч) или лето
    if (fancoil.mode.mode == 3) or (fancoil.mode.mode == 0):  # mode = heat or mode = hand
        dT = fancoil.T2.value - fancoil.T1.value
    else:
        dT = fancoil.T1.value - fancoil.T2.value
        
    # Затраченная энергия за 3 сек [Дж]
    if fancoil.supply_fan.status == 1:
        Q = fancoil.V_In * 1.2 * fancoil.C_air * dT * 3    # объем воздуха поданный с улицы за 3 сек с концентрацией ppm2
        fancoil.W = Q / 3   # мощьность расходуемая на изменение температуры воздуха
    else:
        Q = 0
        fancoil.W = 0   # мощьность расходуемая на изменение температуры воздуха

        
    # вычисление сумарно затраченой энергии на подогрев воздуха
    fancoil.Q_Summ = fancoil.Q_Summ + Q

#-------------------------------------------------
###################################
#  nB система
#
#  функция вычисления затраченой энергии на подогрев  воздуха 
#   
#  v.1
#  
###################################
def Q_Summ_nB(nB: VentilationSystem):

    # вычисление dT для (зима + руч) или лето
    if (nB.mode.mode == 3) or (nB.mode.mode == 0):  # mode = heat or mode = hand
        dT = nB.T2.value - nB.T1.value
    else:
        dT = nB.T1.value - nB.T2.value
        
    # Затраченная энергия за 3 сек [Дж]
    if nB.supply_fan.status == 1:
        Q = nB.V_In * 1.2 * nB.C_air * dT * 3    # объем воздуха поданный с улицы за 3 сек с концентрацией ppm2
        nB.W = Q / 3   # мощьность расходуемая на изменение температуры воздуха
    else:
        Q = 0
        nB.W = 0   # мощьность расходуемая на изменение температуры воздуха

        
    # вычисление сумарно затраченой энергии на подогрев воздуха
    nB.Q_Summ = nB.Q_Summ + Q
#    nB.Q_Summ = nB.Q_Summ + 1
#-------------------------------------------------


    
###################################
# функция вычисления утечки температуры
# (энергии) из помещения
###################################
def leak_Q(T, S, m, K):
    Q = (S * T)/K
    dT = (Q * 30)/(m * 1006)
    return dT


###################################
#  функция вычисления температуры
#  (энергии) в помещении за счет 
#  подачи теплого воздуха
#  
###################################
#def Add_TemperatureToHall(nB: VentilationSystem):


###################################
#  функция вычисления температуры
#  (энергии) утекающей из помещения  
#  через сены
#       S * dTул     Дж
#   Q = --------    ----  [Вт]
#         K          с
#
#                   Q * 3 сек
#   dTпом = ------------------------
#             m * 3 сек * 1005 Дж / (кг * °C)
#
#
#   Ушедшую энергию подсчитываем каждые 3 сек
#
###################################
def Leak_TemperatureFromHall(nB: VentilationSystem):
    Q = ((nB.T5.value - nB.T3.value) * nB.S) / nB.K  #  Дж / c [Вт]

    dT = (Q * 3) / (nB.m * 3 * 1005)

    nB.T3.value = nB.T3.value + dT


###################################
#  ПВ  Система

#  функция вычисления температуры
#  в помещении при подаче теплого воздуха 
#  
#            -S * dTул     
#   Q_loss =  --------  * dt  [Дж]
#                K         
#
#                   Q * 3 сек
#   dTпом = ------------------------
#             m * 3 сек * 1005 Дж / (кг * °C)
#
#
#   Ушедшую энергию подсчитываем каждые 3 сек
#
###################################
def TemperatureHallCalc(nB: VentilationSystem):
    Q_in = nB.m_In * nB.dt * nB.C_air * (nB.T2.value - nB.T3.value)  #  Дж приток энергии с теплым воздухом

    Q_loss = (-1 * nB.S * (nB.T3.value - nB.T5.value) * nB.dt) / nB.K   # Дж  утечка энергии через стены

    Q_excess = nB.Heat_excess * nB.dt * 1000                            # Дж  теплоизбытки в помещении за 3 сек

    Q_total = Q_in + Q_loss + Q_excess                                           

    dT = Q_total / (nB.m * nB.C_air)                                   # прирост температуры в помещении

    nB.T3.value = nB.T3.value + dT                                   # вычисление новой температуры в помещении


###################################
#  Фанкоил
#
#  функция вычисления температуры
#  в помещении при подаче теплого воздуха 
#  
#            -S * dTул     
#   Q_loss =  --------  * dt  [Дж]
#                K         
#
#                   Q * 3 сек
#   dTпом = ------------------------
#             m * 3 сек * 1005 Дж / (кг * °C)
#
#
#   Ушедшую энергию подсчитываем каждые 3 сек
#
###################################
def TemperatureHallCalc_Fancoil(fancoil: FancoilSystem):
    Q_in = fancoil.m_In * fancoil.dt * fancoil.C_air * (fancoil.T2.value - fancoil.T3.value)  #  Дж приток энергии с теплым воздухом

    Q_loss = (-1 * fancoil.S * (fancoil.T3.value - fancoil.T5.value) * fancoil.dt) / fancoil.K   # Дж  утечка энергии через стены

    Q_excess = fancoil.Heat_excess * fancoil.dt * 1000                            # Дж  теплоизбытки в помещении за 3 сек

    Q_total = Q_in + Q_loss + Q_excess                                           

    dT = Q_total / (fancoil.m * fancoil.C_air)                                   # прирост температуры в помещении

    fancoil.T3.value = fancoil.T3.value + dT                                   # вычисление новой температуры в помещении
    

###################################
#  функция вычисления температуры
#  в помещении при подаче горячей воды 
#  
#            -S * dTул     
#   Q_loss =  --------  * dt  [Дж]
#                K         
#
#                   Q * 3 сек
#   dTпом = ------------------------
#             m * 3 сек * 1005 Дж / (кг * °C)
#
#
#   Ушедшую энергию подсчитываем каждые 3 сек
#
###################################
def TemperatureHallCalc_Heater(M_heater: HeaterSystem):
    Q_in = M_heater.m_In * M_heater.dt * M_heater.C_water * (M_heater.T2.value - M_heater.T1.value)  #  Дж приток энергии с горячей водой

    Q_loss = (-1 * M_heater.S * (M_heater.T4.value - M_heater.T5.value) * M_heater.dt) / M_heater.K   # Дж  утечка энергии через стены

    Q_excess = M_heater.Heat_excess * M_heater.dt * 1000                            # Дж  теплоизбытки в помещении за 3 сек

    Q_total = Q_in + Q_loss + Q_excess                                           

    dT = Q_total / (M_heater.m * M_heater.C_air)                                   # прирост температуры в помещении

    M_heater.T4.value = M_heater.T4.value + dT                                   # вычисление новой температуры в помещении


###################################
#  функция вычисления температур
#  в системе отопления 
#  
#
#
#   подсчитываем каждые 3 сек
#
###################################
def TemperatureCalc_HeaterSystem(M_heater: HeaterSystem):
    if M_heater.M1.status == 1:
        T2 = ((M_heater.T3.value - M_heater.T4.value)*M_heater.water_valve.position)/100 + M_heater.T4.value
        if (M_heater.T2.value - 10) > M_heater.T4.value:
            T1 = M_heater.T2.value - 10
        else:
            T1 = M_heater.T4.value
    else:
        T2 = M_heater.T4.value
        T1 = M_heater.T4.value         
        
    # стремление текущей температуры к расчетной
    if M_heater.T2.value < T2:
        M_heater.T2.value = M_heater.T2.value + 1
    if M_heater.T2.value > T2:
        M_heater.T2.value = M_heater.T2.value - 1

    if M_heater.T1.value < T1:
        M_heater.T1.value = M_heater.T1.value + 1
    if M_heater.T1.value > T1:
        M_heater.T1.value = M_heater.T1.value - 1 

###################################
# функция вычисления уставки теплоносителя в зависимости от
# уличной температуры
# REG(SCALER->y) = ( (REG(SCALER->x) - MODBUS_REG(SCALER->x1)) * (REG(SCALER->y2) - REG(SCALER->y1)) ) / (REG(SCALER->x2) - REG(SCALER->x1)) + REG(SCALER->y1);
###################################
def TemperatureCalc_Water_SP(M_heater: HeaterSystem):
    if M_heater.T5.value > M_heater.T_out2.value:
        M_heater.SetPoint.value = M_heater.T_water2.value
    elif M_heater.T5.value < M_heater.T_out1.value:
        M_heater.SetPoint.value = M_heater.T_water1.value
    else:
        M_heater.SetPoint.value = (((M_heater.T5.value - M_heater.T_out1.value) * (M_heater.T_water2.value - M_heater.T_water1.value))/
            (M_heater.T_out2.value - M_heater.T_out1.value) + M_heater.T_water1.value)
        
    
###################################
# функция вычисления Т1
###################################
def calculate_T1(T):
    T = T + 2
    return T

###################################
#  функция ввода float
###################################
def ask_float(prompt, default, min_val=None, max_val=None):
    """
    @brief Запрашивает число у пользователя.
    Если введено пусто, абракадабра или значение вне диапазона,
    возвращает default.

    @param prompt введеная строка
    """
    raw = input(f"{prompt} (по умолчанию {default}): ")
    try:
        if raw.strip() == "":
            return default
        val = float(raw)
        if (min_val is not None and val < min_val) or (max_val is not None and val > max_val):
            print(f"Введено {val}, но допустимо только {min_val}..{max_val}. Беру {default}.")
            return default
        return val
    except ValueError:
        print(f"Неверный ввод '{raw}'. Беру {default}.")
        return default


###################################
# функция преобразования в int16
###################################
def to_int16(value):
    return value - 0x10000 if value & 0x8000 else value



    

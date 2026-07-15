##draw_hvac()
##draw_control_panel()
##draw_alarm_panel()

import streamlit as st
from modules import module_assets
from modules import module_udp

mode_list = ["Ручной", "Лето", "Переходный", "Зима"]

def draw_hvac(data):
    col_left, col_right = st.columns([1, 2])

    # ===== ЛЕВАЯ ПАНЕЛЬ (УПРАВЛЕНИЕ) =====
    with col_left:

        st.subheader("Управление")

        col1, col2 = st.columns([3,1])

        with col1:
            #st.subheader("Уставка:")
            st.markdown(f"<span style = 'font-size: 20px; color: black; font-weight: bold;'>Уставка [°C]:</span>",unsafe_allow_html = True)
            st.markdown(f"<span style = 'font-size: 20px; color: black; font-weight: bold;'>Режим:</span>",unsafe_allow_html = True)
            st.markdown(f"<span style = 'font-size: 20px; color: black; font-weight: bold;'>Объем помещения [M^3]:</span>",unsafe_allow_html = True)
            st.markdown(f"<span style = 'font-size: 20px; color: black; font-weight: bold;'>Избыток тепла [кВт]:</span>",unsafe_allow_html = True)
            st.markdown(f"<span style = 'font-size: 20px; color: black; font-weight: bold;'>Расходуемая мощность [кВт]:</span>",unsafe_allow_html = True)
            st.markdown(f"<span style = 'font-size: 20px; color: black; font-weight: bold;'>Суммарная энергия поданая через теплобменник [кДж]:</span>",unsafe_allow_html = True)
        with col2:
            if not data:
                st.info("Жду данные ...")
            else:
                T_SP = data.get("T_SP", None)
                mode = data.get("mode", None)
                V_room = data.get("V_room", None)
                Heat_excess = data.get("Heat_excess", None)
                W = data.get("W", None)/1000
                Q_Summ = data.get("Q_Summ", None)/1000
                st.markdown(f"<span style = 'font-size: 20px; color: blue; font-weight: bold;'>{T_SP}</span>",unsafe_allow_html = True)
                st.markdown(f"<span style = 'font-size: 20px; color: black; color: blue; font-weight: bold;'>{mode_list[mode]}</span>",unsafe_allow_html = True)
                st.markdown(f"<span style = 'font-size: 20px; color: black; color: blue; font-weight: bold;'>{V_room}</span>",unsafe_allow_html = True)
                st.markdown(f"<span style = 'font-size: 20px; color: black; color: blue; font-weight: bold;'>{Heat_excess}</span>",unsafe_allow_html = True)
                st.markdown(f"<span style = 'font-size: 20px; color: black; color: blue; font-weight: bold;'>{W}</span>",unsafe_allow_html = True)
                st.markdown(f"<span style = 'font-size: 20px; color: black; color: blue; font-weight: bold;'>{Q_Summ}</span>",unsafe_allow_html = True)

        
##        temp_setpoint = st.number_input("Уставка", value=22)
##        mode = st.selectbox("Режим", ["HEAT", "FAN", "COOL"])
##
##        enabled = st.toggle("Ручной режим")
##        temp = st.slider("Температура", 18, 40, 22)
##
##        progress = st.progress(70)
##
##        st.metric("Температура", "22 °C", "+1.2")
        
        data = st.session_state.data
         
    

##        if st.session_state.active_alarms:
##            st.error("🚨 Есть аварии")
##        else:
##            st.success("✅ Всё нормально")
##
##        for alarm in st.session_state.active_alarms:
##            st.warning(f"{alarm['name']}")        
        

##        col_btn1, col_ind1, col_btn2, col_ind2 = st.columns([4,1,4,1])
##
##
##        color1 = "green"
##
##        
##        with col_btn1:
##            
##            if st.button("▶ Пуск",key="btn_start", type="secondary", use_container_width=True):
##                st.session_state.running = True
##                module_udp.send_udp({
##                    "temp_setpoint": temp_setpoint,
##                    "mode": mode
##                })
##
##            st.markdown("</div>", unsafe_allow_html=True)
##
##        with col_ind1:
##            
##
##            st.markdown(f"""
##            <div style='
##                width:20px;
##                height:20px;
##                background:{color1};
##                border-radius:50%;
##                margin-top:10px;
##            '></div>
##            """, unsafe_allow_html=True)
##            
##        with col_btn2:
##            if st.button("⏹ Стоп", key="btn_stop",type="secondary", use_container_width=True):
##                module_udp.send_udp({
##                    "temp_setpoint": temp_setpoint,
##                    "mode": mode
##                })
##                
##        with col_ind2:
##            color2 = "red" 
##
##            st.markdown(f"""
##            <div style='
##                width:20px;
##                height:20px;
##                background:{color2};
##                border-radius:50%;
##                margin-top:10px;
##            '></div>
##            """, unsafe_allow_html=True)
    
    # ===== ПРАВАЯ ПАНЕЛЬ (МНЕМОСХЕМА) =====
    with col_right:
        st.subheader("Состояние системы")

        if data:
            T1 = round(data.get("T1", 22.0))
            T2 = round(data.get("T2", 22.0))
            T3 = round(data.get("T3", 22.0))
            T4 = round(data.get("T4", 22.0))
            T5 = data.get("T5", 18.0)
            fan_n1 = data.get("n1", 0)
            fan_B1 = data.get("B1", 0)
            TEN = data.get("DO", 0) & 0x01
            Y3 = data.get("TEN", 10)
            Y4 = data.get("air_bypass", 20)
            nB_filter = data.get("DI", 0) & 0x04
            Y1 = data.get("Y1", 0)         # клапан подачи воздуха
            Y2 = data.get("Y2", 0)         # клапан вытяжной
            
            Alarm_General = data.get("Alarms", 0) & 0x01 # общая авария
            fan_n1_Alarm = data.get("Alarms", 0) & 0x02   # Авария вентилятора 
            fan_B1_Alarm = data.get("Alarms", 0) & 0x04
            Recuperator_Alarm = data.get("Alarms", 0) & 0x08  # Рекуператор замерз
            Y1_Alarm = data.get("Alarms", 0) & 0x20    # вария клапана Y4 air_bypass
            Y4_Alarm = data.get("Alarms", 0) & 0x40    # вария клапана Y4 air_bypass         

            fan_img = module_assets.fan_300x300_base64
            filter_img = module_assets.filter_Bad_pic_base64 if nB_filter else module_assets.filter_Ok_pic_base64
            Y1_img_On_Off = module_assets.Y_air_On_pic_base64 if Y1 else module_assets.Y_air_Off_pic_base64
            Y1_img        = module_assets.Y_air_Alarm_pic_base64 if Y1_Alarm else Y1_img_On_Off
            Y2_img = module_assets.Y_air_On_pic_base64 if Y2 else module_assets.Y_air_Off_pic_base64
            Y4_img = module_assets.Y_air_bypass_Alarm_pic_base64 if Y4_Alarm else module_assets.Y_air_bypass_Ok_pic_base64

            fan_n1_img = module_assets.fan_Alarm_base64 if fan_n1_Alarm else module_assets.fan_Norm_base64
            fan_B1_img = module_assets.fan_Alarm_base64 if fan_B1_Alarm else module_assets.fan_Norm_base64
            Recuperator_img = module_assets.Recuperator_Alarm_pic_base64 if Recuperator_Alarm else module_assets.Recuperator_pic_base64

            
            Alarm_General_txt = "Ошибка !" if Alarm_General else "        "
            
            animation_n1 = "rotate 4s linear infinite" if fan_n1 else "none"
            animation_B1 = "rotate 4s linear infinite" if fan_B1 else "none"

            clFan_n1_ind = 'lime' if fan_n1 else 'green'
            clFan_B1_ind = 'lime' if fan_B1 else 'green'
            clTEN_ind = 'lime' if TEN else 'green'

            
            st.markdown(f"""
            <div style='position: relative; width: 1000px;'>
                <!-- ФОН -->
                <img src='data:image/png;base64,{module_assets.airduct_HVAC5}' style='width: 100%;'>
                <!-- Вентилятор П1 Авария/Норма -->
                <img src='data:image/png;base64,{fan_n1_img}' style='
                    position: absolute;
                    top: 228px;
                    left: 356px;
                    width: 168px;
                '>       
                <!-- Вентилятор В1 Авария/Норма -->
                <img src='data:image/png;base64,{fan_B1_img}' style='
                    position: absolute;
                    top: 59px;
                    left: 356px;
                    width: 168px;
                '>       
                <div style='
                    position: absolute;
                    top: 25px;
                    left: 430px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    B1
                </div>
                <!-- ВЕНТИЛЯТОР ПРИТОКА АНИМАЦИЯ -->
                <img src='data:image/png;base64,{fan_img}' style='
                    position:absolute;
                    top:226px;
                    left:353px;
                    width:170px;
                    animation: {animation_n1};
                '>
                <div style='
                    position: absolute;
                    top: 392px;
                    left: 430px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    П1
                </div>
                <!-- ВЕНТИЛЯТОР ВЫТЯЖКИ АНИМАЦИЯ -->
                <img src='data:image/png;base64,{fan_img}' style='
                    position:absolute;
                    top:57px;
                    left:353px;
                    width:170px;
                    animation: {animation_B1};
                '>                
                <!-- Рекуператор Авария/Норма -->
                <img src='data:image/png;base64,{Recuperator_img}' style='
                    position: absolute;
                    top: 57px;
                    left: 173px;
                    width: 184px;
                '>    
                <!-- Клапан Y1 -->
                <img src='data:image/png;base64,{Y1_img}' style='
                    position: absolute;
                    top: 58px;
                    left: 105px;
                    width: 37px;
                '>
                <div style='
                    position: absolute;
                    top: 25px;
                    left: 105px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    Y1
                </div>
                <!-- Клапан Y2 -->
                <img src='data:image/png;base64,{Y2_img}' style='
                    position: absolute;
                    top: 227px;
                    left: 105px;
                    width: 37px;
                '>                
                <div style='
                    position: absolute;
                    top: 392px;
                    left: 105px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    Y2
                </div>
                <!-- Клапан Y4 air_bypass -->
                <img src='data:image/png;base64,{Y4_img}' style='
                    position: absolute;
                    top: 59px;
                    left: 242px;
                    width: 35px;
                '>           
                <!-- ФИЛЬТР -->
                <img src='data:image/png;base64,{filter_img}' style='
                    position: absolute;
                    top: 60px;
                    left: 525px;
                    width: 34px;
                '>
                <img src='data:image/png;base64,{filter_img}' style='
                    position: absolute;
                    top: 226px;
                    left: 140px;
                    width: 34px;
                '>
                <!-- ТЕМПЕРАТУРА T1-->
                <div style='
                    position: absolute;
                    top: 362px;
                    left: 280px;
                    color: blue;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    {T1} °C
                </div>
                <div style='
                    position: absolute;
                    top: 392px;
                    left: 280px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    T1
                </div>
                <!-- ТЕМПЕРАТУРА T2-->
                <div style='
                    position: absolute;
                    top: 300px;
                    left: 797px;
                    color: blue;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    {T2} °C
                </div>
                <div style='
                    position: absolute;
                    top: 330px;
                    left: 797px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    T2
                </div>
                <!-- ТЕМПЕРАТУРА T3-->
                <div style='
                    position: absolute;
                    top: 130px;
                    left: 797px;
                    color: blue;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    {T3} °C
                </div>                
                <div style='
                    position: absolute;
                    top: 160px;
                    left: 797px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    T3
                </div>
                <!-- ТЕМПЕРАТУРА T4-->
                <div style='
                    position: absolute;
                    top: 362px;
                    left: 200px;
                    color: blue;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    {T4} °C
                </div>
                <div style='
                    position: absolute;
                    top: 392px;
                    left: 200px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    T4
                </div>
                <!-- ТЕМПЕРАТУРА T5-->
                <div style='
                    position: absolute;
                    top: 25px;
                    left: 10px;
                    color: blue;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    {T5} °C
                </div>                
                <div style='
                    position: absolute;
                    top: 0px;
                    left: 10px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    T5
                </div>
                <!-- Y3 ТЭН -->
                <div style='
                    position: absolute;
                    top: 417px;
                    left: 640px;
                    color: green;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    {Y3} %
                </div>        
                <div style='
                    position: absolute;
                    top: 392px;
                    left: 630px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    ТЭН
                </div>
                <!-- Y4 air_bypass -->
                <div style='
                    position: absolute;
                    top: 25px;
                    left: 240px;
                    color: green;
                    font-size: 20px;
                    font-weight: bold;
                '>
                   {Y4} %
                </div>        
                <div style='
                    position: absolute;
                    top: 0px;
                    left: 240px;
                    color: gray;
                    font-size: 20px;
                    font-weight: bold;
                '>
                    Y4
                </div>
                <!-- ИНДИКАТОР П1-->
                <div style='
                    position:absolute;
                    top:235px;
                    left:497px;
                    width:20px;
                    height:20px;
                    background-color:{clFan_n1_ind};
                    border-radius:50%;
                '></div>                
                <!-- ИНДИКАТОР B1-->
                <div style='
                    position:absolute;
                    top:63px;
                    left:497px;
                    width:20px;
                    height:20px;
                    background-color:{clFan_B1_ind};
                    border-radius:50%;
                '></div>                
                <!-- ИНДИКАТОР ТЭН-->
                <div style='
                    position:absolute;
                    top:235px;
                    left:665px;
                    width:20px;
                    height:20px;
                    background-color:{clTEN_ind};
                    border-radius:50%;
                '></div>           
                <!-- ОБЩАЯ ОШИБКА! -->
                <div style='
                    position: absolute;
                    top: 0px;
                    left: 380px;
                    color: red;
                    font-size: 20px;
                    font-weight: bold;
                '>
                   {Alarm_General_txt} .
                </div>        
            </div>
            <style>
            @keyframes rotate {{
                from {{ transform: rotate(0deg); }}
                to {{ transform: rotate(360deg); }}
            }}
            </style>            
            """, unsafe_allow_html=True)        

        else:
            st.info("Жду данные...")

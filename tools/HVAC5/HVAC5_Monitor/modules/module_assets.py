import base64


def img_to_base64(path):

    with open(path, "rb") as f:
        return base64.b64encode(
            f.read()
        ).decode()


# ---- картинки ----
airduct_HVAC5 = img_to_base64("assets/airduct_HVAC5.png")

fan_on_base64 = img_to_base64(
    "assets/Fan_Alarm.png"
)

fan_off_base64 = img_to_base64(
    "assets/fan_normal.png"
)

filter_pic_base64 = img_to_base64(
    "assets/filter_Ok_33x168.png"
)

filter_Ok_pic_base64 = img_to_base64(
    "assets/filter_Ok_33x168.png"
)

filter_Bad_pic_base64 = img_to_base64(
    "assets/filter_Bad_33x168.png"
)

fan_300x300_base64 = img_to_base64("assets/fan_300x300.png")
fan_Alarm_base64 = img_to_base64("assets/fan_Alarm_540x540.png")
fan_Norm_base64 = img_to_base64("assets/fan_Norm_540x540.png")

Y_air_Off_pic_base64 = img_to_base64(
    "assets/Y_air_Off_37_170.png"
)
Y_air_On_pic_base64 = img_to_base64(
    "assets/Y_air_On_37_170.png"
)
Y_air_bypass_Ok_pic_base64 = img_to_base64(
    "assets/Y_air_bypass_Ok_35_91.png"
)
Y_air_bypass_Alarm_pic_base64 = img_to_base64(
    "assets/Y_air_bypass_Alarm_35_91.png"
)
Y_air_Alarm_pic_base64 = img_to_base64(
    "assets/Y_air_Alarm_37_170.png"
)
Recuperator_pic_base64 = img_to_base64(
    "assets/Recuperator_184x340.png"
)
Recuperator_Alarm_pic_base64 = img_to_base64(
    "assets/Recuperator_Alarm_184x340.png"
)

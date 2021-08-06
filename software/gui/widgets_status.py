import PySide6.QtGui
from PySide6 import QtCore, QtWidgets, QtGui


class StatusWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.label_status = QtWidgets.QLabel("0 0 0 0 | 0 0 0 0", alignment=QtCore.Qt.AlignLeft)

        self.label_pwm_h = QtWidgets.QLabel("High beam PWM:", alignment=QtCore.Qt.AlignLeft)
        self.label_pwm_l = QtWidgets.QLabel("Low beam PWM:", alignment=QtCore.Qt.AlignLeft)
        self.label_pwm_r = QtWidgets.QLabel("Rear lights PWM:", alignment=QtCore.Qt.AlignLeft)

        self.separator = QtWidgets.QFrame()
        self.separator.setFrameShape(QtWidgets.QFrame.VLine)
        self.separator.setFrameShadow(QtWidgets.QFrame.Sunken)

        self.edit_pwm_h = QtWidgets.QLineEdit("0", alignment=QtCore.Qt.AlignRight, readOnly=True)
        self.edit_pwm_l = QtWidgets.QLineEdit("0", alignment=QtCore.Qt.AlignRight, readOnly=True)
        self.edit_pwm_r = QtWidgets.QLineEdit("0", alignment=QtCore.Qt.AlignRight, readOnly=True)

        self.layoutv_main = QtWidgets.QVBoxLayout(self)
        self.layouth_main = QtWidgets.QHBoxLayout()
        self.layoutv_main.addWidget(QtWidgets.QLabel("Status", alignment=QtCore.Qt.AlignCenter))
        self.layoutv_main.addLayout(self.layouth_main)

        self.layout_pwm = QtWidgets.QGridLayout()  # self.align_main
        self.layout_raw = QtWidgets.QVBoxLayout()  # self.align_main
        self.layout_debug = QtWidgets.QHBoxLayout()  # self.align_raw

        self.layout_raw.setAlignment(QtCore.Qt.AlignCenter)

        self.layouth_main.addLayout(self.layout_pwm)
        self.layouth_main.addWidget(self.separator)
        self.layouth_main.addLayout(self.layout_raw)

        self.layout_pwm.setColumnMinimumWidth(1, 4)
        self.layout_pwm.addWidget(self.label_pwm_h, 0, 0)
        self.layout_pwm.addWidget(self.label_pwm_l, 1, 0)
        self.layout_pwm.addWidget(self.label_pwm_r, 2, 0)

        self.layout_pwm.addWidget(self.edit_pwm_h, 0, 2)
        self.layout_pwm.addWidget(self.edit_pwm_l, 1, 2)
        self.layout_pwm.addWidget(self.edit_pwm_r, 2, 2)

        self.lights_state_widget = ByteStateWidget(label="STATE")
        self.readings_state_widget = ByteStateWidget(label="VOLT |  LUX")
        self.label_debug = QtWidgets.QLabel("Debug value:", alignment=QtCore.Qt.AlignLeft)
        self.edit_debug = QtWidgets.QLineEdit("0", alignment=QtCore.Qt.AlignRight, readOnly=True)
        self.layout_raw.addWidget(self.lights_state_widget)
        self.layout_raw.addWidget(self.readings_state_widget)
        self.layout_raw.addLayout(self.layout_debug)
        self.layout_debug.addWidget(self.label_debug)
        self.layout_debug.addWidget(self.edit_debug)

    @QtCore.Slot()
    def incoming_data(self, data: list):
        self.edit_pwm_h = data[2]
        self.edit_pwm_l = data[3]
        self.edit_pwm_r = data[4]
        self.lights_state_widget.set_byte_state(data[5])
        self.readings_state_widget.set_byte_state(data[6])
        self.edit_debug = data[7]


class FlagWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.setFixedHeight(12)
        self.setFixedWidth(12)

        self.set_state_color = QtCore.Qt.darkGreen
        self.reset_state_color = QtCore.Qt.gray
        self.cur_color = self.reset_state_color

        self.painter = QtGui.QPainter()
        #self.painter.setRenderHint(QtGui.QPainter.Antialiasing)

        self.reset_flag()

        self.blur = QtWidgets.QGraphicsBlurEffect()
        self.blur.setBlurHints(QtWidgets.QGraphicsBlurEffect.PerformanceHint)
        self.blur.setBlurRadius(2)
        self.setGraphicsEffect(self.blur)

    def paintEvent(self, event: PySide6.QtGui.QPaintEvent) -> None:
        super().paintEvent(event)
        self.painter.begin(self)
        self.painter.setBrush(self.cur_color)
        self.painter.drawRoundedRect(2, 2, 10, 10, 2, 2)
        self.painter.end()

    def set_flag(self):
        self._change_color(self.set_state_color)
        pass

    def reset_flag(self):
        self._change_color(self.reset_state_color)
        pass

    def switch_flag(self, state: bool):
        if state:
            self.set_flag()
        else:
            self.reset_flag()

    def _change_color(self, color):
        #p = self.palette()
        #p.setColor(self.backgroundRole(), color)
        #self.setPalette(p)
        self.cur_color = color
        self.update()


class ByteStateWidget(QtWidgets.QWidget):
    def __init__(self, label=""):
        super().__init__()
        self.flags = [FlagWidget() for i in range(8)]
        self.label_top = QtWidgets.QLabel(label, alignment=QtCore.Qt.AlignCenter)

        self.layout_v = QtWidgets.QVBoxLayout(self)
        self.layout_h = QtWidgets.QHBoxLayout()

        for flag in self.flags:
            self.layout_h.addWidget(flag)

        self.layout_v.addWidget(self.label_top)
        self.layout_v.addLayout(self.layout_h)

    def set_byte_state(self, state):
        for n in range(8):
            is_set = state & 1 << n != 0
            self.flags[n].switch_flag(is_set)
        pass

from PySide6 import QtCore, QtWidgets, QtGui


class MainConfigWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.layoutv_main = QtWidgets.QVBoxLayout(self)
        self.layouth_main = QtWidgets.QHBoxLayout()
        self.pwm_config = PWMConfigWidget()
        self.param_config = ParametersWidget()
        self.separator = QtWidgets.QFrame()
        self.pwm_test = PWMTestWidget()
        self.separator.setFrameShape(QtWidgets.QFrame.VLine)
        self.separator.setFrameShadow(QtWidgets.QFrame.Sunken)

        self.layouth_main.addWidget(self.pwm_config)
        self.layouth_main.addWidget(self.separator)
        self.layouth_main.addWidget(self.param_config)

        self.layoutv_main.addLayout(self.layouth_main)
        self.layoutv_main.addWidget(QtWidgets.QSplitter(QtCore.Qt.Vertical))
        self.layoutv_main.addWidget(self.pwm_test)


class PWMConfigWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.beam_low_day = ExtendedSlider()
        self.beam_high_day = ExtendedSlider()
        self.beam_rear_day = ExtendedSlider()

        self.beam_low = ExtendedSlider()
        self.beam_high = ExtendedSlider()
        self.beam_rear = ExtendedSlider()

        self.beam_rear_brake = ExtendedSlider()

        self.label_normal = QtWidgets.QLabel("Normal mode", alignment=QtCore.Qt.AlignCenter)
        self.label_daylights = QtWidgets.QLabel("Daylights mode", alignment=QtCore.Qt.AlignCenter)

        self.form_main = QtWidgets.QFormLayout(self)
        self.form_main.addRow(self.label_normal)
        self.form_main.addRow("Low beam power:", self.beam_low)
        self.form_main.addRow("High beam power:", self.beam_high)
        self.form_main.addRow("Rear lights power:", self.beam_rear)

        self.form_main.addRow(self.label_daylights)
        self.form_main.addRow("Low beam power:", self.beam_low_day)
        self.form_main.addRow("High beam power:", self.beam_high_day)
        self.form_main.addRow("Rear lights power:", self.beam_rear_day)


class PWMTestWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.beam_pwm = ExtendedSlider()
        self.radio_beam_low = QtWidgets.QRadioButton("Low beam")
        self.radio_beam_high = QtWidgets.QRadioButton("High beam")
        self.radio_beam_rear = QtWidgets.QRadioButton("Rear lights")
        self.button_test = QtWidgets.QPushButton("Enable Testmode")

        self.layout_main = QtWidgets.QVBoxLayout(self)
        self.layout_main.addWidget(QtWidgets.QLabel("PWM test", alignment=QtCore.Qt.AlignCenter))
        self.layout_main.addWidget(self.beam_pwm)
        self.layout_radio = QtWidgets.QHBoxLayout()
        self.layout_radio.addWidget(self.radio_beam_low)
        self.layout_radio.addWidget(self.radio_beam_high)
        self.layout_radio.addWidget(self.radio_beam_rear)
        self.layout_main.addLayout(self.layout_radio)
        self.layout_main.addWidget(self.button_test)


class ParametersWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.label_options = QtWidgets.QLabel("Options", alignment=QtCore.Qt.AlignCenter)
        self.checkb_hybrid = QtWidgets.QCheckBox()
        self.checkb_soft = QtWidgets.QCheckBox()

        self.label_ss = QtWidgets.QLabel("Soft Start", alignment=QtCore.Qt.AlignCenter)
        self.edit_ss_delay = QtWidgets.QLineEdit("30", maximumWidth=96, alignment=QtCore.Qt.AlignCenter)
        self.slider_ss_step = ExtendedSlider(max=20, min=1)

        self.label_ad = QtWidgets.QLabel("Adaptive mode", alignment=QtCore.Qt.AlignCenter)
        self.slider_ad_day = ExtendedSlider(max=15, min=0)
        self.slider_ad_low = ExtendedSlider(max=15, min=0)
        self.slider_ad_high = ExtendedSlider(max=15, min=0)
        self.edit_ad_hyst = QtWidgets.QLineEdit("2000", maximumWidth=96, alignment=QtCore.Qt.AlignCenter)

        self.form_main = QtWidgets.QFormLayout(self, alignment=QtCore.Qt.AlignRight)
        self.form_main.setFormAlignment(QtCore.Qt.AlignRight)
        self.form_main.addRow(self.label_options)
        self.form_main.addRow("Hybrid high-low beam:", self.checkb_hybrid)
        self.form_main.addRow("SoftStart feature:", self.checkb_soft)
        self.form_main.addRow(self.label_ss)
        self.form_main.addRow("SoftStart delay, ms:", self.edit_ss_delay)
        self.form_main.addRow("SoftStart stepping:", self.slider_ss_step)
        self.form_main.addRow(self.label_ad)
        self.form_main.addRow("Day-mode threshold:", self.slider_ad_day)
        self.form_main.addRow("Low-beam-mode threshold:", self.slider_ad_low)
        self.form_main.addRow("High-beam-mode threshold:", self.slider_ad_high)
        self.form_main.addRow("Mode change hysteresis, ms:", self.edit_ad_hyst)


class ExtendedSlider(QtWidgets.QWidget):
    def __init__(self, max=255, min=0):
        super().__init__()
        self.layout_h = QtWidgets.QHBoxLayout(self)

        self.slider = QtWidgets.QSlider()
        self.slider.setOrientation(QtCore.Qt.Horizontal)
        self.slider.setMinimum(min)
        self.slider.setMaximum(max)
        self.slider.setMinimumSize(100, 5)
        self.ed = QtWidgets.QLineEdit("0")
        self.ed.setReadOnly(True)
        self.ed.setMaximumWidth(64)
        self.ed.setAlignment(QtCore.Qt.AlignCenter)

        self.layout_h.addWidget(self.slider)
        self.layout_h.addWidget(self.ed)
        self.layout_h.setContentsMargins(0, 4, 0, 0)

        self.slider.valueChanged.connect(self.set_editor_val)

    @QtCore.Slot()
    def set_editor_val(self, val: int):
        self.ed.setText(str(val))

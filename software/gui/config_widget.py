from PySide6 import QtCore, QtWidgets, QtGui


class ConfigWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.layout_main = QtWidgets.QHBoxLayout(self)
        self.pwm_config = PWMConfigWidget()
        self.param_config = ParametersWidget()
        self.separator = QtWidgets.QFrame()
        self.separator.setFrameShape(QtWidgets.QFrame.VLine)
        self.separator.setFrameShadow(QtWidgets.QFrame.Sunken)

        self.layout_main.addWidget(self.pwm_config)
        self.layout_main.addWidget(self.separator)
        self.layout_main.addWidget(self.param_config)


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
        self.form_normal = QtWidgets.QFormLayout()
        self.form_normal.addRow("Low beam power:", self.beam_low)
        self.form_normal.addRow("High beam power:", self.beam_high)
        self.form_normal.addRow("Rear lights power:", self.beam_rear)

        self.label_daylights = QtWidgets.QLabel("Daylights mode", alignment=QtCore.Qt.AlignCenter)
        self.form_daylights = QtWidgets.QFormLayout()
        self.form_daylights.addRow("Low beam power:", self.beam_low_day)
        self.form_daylights.addRow("High beam power:", self.beam_high_day)
        self.form_daylights.addRow("Rear lights power:", self.beam_rear_day)

        self.layout_v = QtWidgets.QVBoxLayout(self)

        self.layout_v.addWidget(self.label_normal)
        self.layout_v.addLayout(self.form_normal)
        self.layout_v.addWidget(self.label_daylights)
        self.layout_v.addLayout(self.form_daylights)

        self.test_slider = ExtendedSlider()
        self.layout_v.addWidget(self.test_slider)


class ParametersWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()
        self.layout_v = QtWidgets.QVBoxLayout(self)

        self.label_options = QtWidgets.QLabel("Options", alignment=QtCore.Qt.AlignCenter)
        self.checkb_hybrid = QtWidgets.QCheckBox()
        self.checkb_soft = QtWidgets.QCheckBox()
        self.form_options = QtWidgets.QFormLayout(alignment=QtCore.Qt.AlignRight)
        self.form_options.addRow("Hybrid high-low beam:", self.checkb_hybrid)
        self.form_options.addRow("SoftStart feature:", self.checkb_soft)
        self.layout_v.addWidget(self.label_options)
        self.layout_v.addLayout(self.form_options)

        self.label_ss = QtWidgets.QLabel("Soft Start", alignment=QtCore.Qt.AlignCenter)
        self.edit_ss_delay = QtWidgets.QLineEdit("30", maximumWidth=48)
        self.slider_ss_step = ExtendedSlider(max=20, min=1)
        self.form_ss = QtWidgets.QFormLayout(alignment=QtCore.Qt.AlignRight)
        self.form_ss.addRow("SoftStart delay, ms:", self.edit_ss_delay)
        self.form_ss.addRow("SoftStart stepping:", self.slider_ss_step)
        self.layout_v.addWidget(self.label_ss)
        self.layout_v.addLayout(self.form_ss)

        self.label_ad = QtWidgets.QLabel("Adaptive mode", alignment=QtCore.Qt.AlignCenter)
        self.slider_ad_day = ExtendedSlider(max=15, min=0)
        self.slider_ad_low = ExtendedSlider(max=15, min=0)
        self.slider_ad_high = ExtendedSlider(max=15, min=0)
        self.edit_ad_hyst = QtWidgets.QLineEdit("2000", maximumWidth=48)
        self.form_ad = QtWidgets.QFormLayout()
        self.form_ad.setFormAlignment(QtCore.Qt.AlignLeft)
        self.form_ad.addRow("Day-mode threshold:", self.slider_ad_day)
        self.form_ad.addRow("Low-beam-mode threshold:", self.slider_ad_low)
        self.form_ad.addRow("High-beam-mode threshold:", self.slider_ad_high)
        self.form_ad.addRow("Mode change hysteresis, ms:", self.edit_ad_hyst)
        self.layout_v.addWidget(self.label_ad)
        self.layout_v.addLayout(self.form_ad)


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
        self.ed.setMaximumWidth(48)
        self.ed.setAlignment(QtCore.Qt.AlignCenter)

        self.layout_h.addWidget(self.slider)
        self.layout_h.addWidget(self.ed)
        self.layout_h.setContentsMargins(0, 4, 0, 0)

        self.slider.valueChanged.connect(self.set_editor_val)

    @QtCore.Slot()
    def set_editor_val(self, val: int):
        self.ed.setText(str(val))

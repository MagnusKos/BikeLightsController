import PySide6.QtGui
from PySide6 import QtCore, QtWidgets, QtGui


class ConnectionWidget(QtWidgets.QWidget):
    def __init__(self):
        super().__init__()

        self.layout_main = QtWidgets.QHBoxLayout(self)
        self.layout_left = QtWidgets.QVBoxLayout()
        self.layout_right = QtWidgets.QVBoxLayout()

        self.list_ports = QtWidgets.QListWidget()
        self.button_refresh = QtWidgets.QPushButton()
        self.button_connect = QtWidgets.QPushButton()
        self.checkb_auto = QtWidgets.QCheckBox()
        self.spin_timeout = QtWidgets.QSpinBox(minimum=1, maximum=20, suffix=" s", prefix="Timeout: ")

        self.button_refresh.setText("Refresh")
        self.button_connect.setText("Connect")
        self.checkb_auto.setText("Automatic connection")

        self.layout_left.addWidget(self.list_ports)
        self.layout_left.addWidget(self.button_connect)

        self.layout_right.addWidget(self.checkb_auto)
        self.layout_right.addWidget(self.spin_timeout)
        self.layout_right.addWidget(self.button_refresh)

        self.layout_main.addLayout(self.layout_left)
        self.layout_main.addLayout(self.layout_right)
import sys
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QFile

from gui.ui_window_main import Ui_MainWindow
from gui.widgets_config import MainConfigWidget
from gui.widgets_status import StatusWidget
from gui.widgets_connection import ConnectionWidget


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.tab_conn.layout().addWidget(ConnectionWidget())
        self.ui.tab_conn.layout().addWidget(StatusWidget())

        self.ui.tab_params_main.layout().addWidget(MainConfigWidget())

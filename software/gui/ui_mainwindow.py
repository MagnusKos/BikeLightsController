# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'main_window.ui'
##
## Created by: Qt User Interface Compiler version 6.1.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import *  # type: ignore
from PySide6.QtGui import *  # type: ignore
from PySide6.QtWidgets import *  # type: ignore


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(800, 640)
        MainWindow.setMinimumSize(QSize(800, 640))
        MainWindow.setBaseSize(QSize(800, 640))
        MainWindow.setTabShape(QTabWidget.Rounded)
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.main_vlayout = QVBoxLayout(self.centralwidget)
        self.main_vlayout.setObjectName(u"main_vlayout")
        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tabWidget.setTabPosition(QTabWidget.West)
        self.tabWidget.setElideMode(Qt.ElideNone)
        self.tabWidget.setUsesScrollButtons(False)
        self.tab_conn = QWidget()
        self.tab_conn.setObjectName(u"tab_conn")
        self.conn_vlayout = QVBoxLayout(self.tab_conn)
        self.conn_vlayout.setObjectName(u"conn_vlayout")
        self.tabWidget.addTab(self.tab_conn, "")
        self.tab_params_main = QWidget()
        self.tab_params_main.setObjectName(u"tab_params_main")
        self.params_main_vlayout = QVBoxLayout(self.tab_params_main)
        self.params_main_vlayout.setObjectName(u"params_main_vlayout")
        self.tabWidget.addTab(self.tab_params_main, "")
        self.tab_params_ext = QWidget()
        self.tab_params_ext.setObjectName(u"tab_params_ext")
        self.params_ext_vlayout = QVBoxLayout(self.tab_params_ext)
        self.params_ext_vlayout.setObjectName(u"params_ext_vlayout")
        self.tabWidget.addTab(self.tab_params_ext, "")
        self.tab_firmware = QWidget()
        self.tab_firmware.setObjectName(u"tab_firmware")
        self.firmware_vlayout = QVBoxLayout(self.tab_firmware)
        self.firmware_vlayout.setObjectName(u"firmware_vlayout")
        self.tabWidget.addTab(self.tab_firmware, "")
        self.tab_settings = QWidget()
        self.tab_settings.setObjectName(u"tab_settings")
        self.settings_vlayout = QVBoxLayout(self.tab_settings)
        self.settings_vlayout.setObjectName(u"settings_vlayout")
        self.tabWidget.addTab(self.tab_settings, "")
        self.tab_about = QWidget()
        self.tab_about.setObjectName(u"tab_about")
        self.about_vlayout = QVBoxLayout(self.tab_about)
        self.about_vlayout.setObjectName(u"about_vlayout")
        self.tabWidget.addTab(self.tab_about, "")

        self.main_vlayout.addWidget(self.tabWidget)

        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"MainWindow", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_conn), QCoreApplication.translate("MainWindow", u"Connection", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_params_main), QCoreApplication.translate("MainWindow", u"Main params", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_params_ext), QCoreApplication.translate("MainWindow", u"Extended params", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_firmware), QCoreApplication.translate("MainWindow", u"Firmware", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_settings), QCoreApplication.translate("MainWindow", u"App settings", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_about), QCoreApplication.translate("MainWindow", u"About...", None))
    # retranslateUi


#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Fri Feb 22 09:03:27 2019
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from ieee802_15_4_oqpsk_phy import ieee802_15_4_oqpsk_phy  # grc-generated hier_block
from optparse import OptionParser
import foo
import ieee802_15_4
import time
from gnuradio import qtgui


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 4e6
        self.window_size = window_size = 48
        self.tran_wid = tran_wid = 300000
        self.sync_length = sync_length = 320
        self.rx_gain = rx_gain = 0.80
        self.freq = freq = 2.435e9
        self.decim = decim = int(samp_rate/4e6)
        self.center_offset_2 = center_offset_2 = -7e6
        self.center_offset_1 = center_offset_1 = 8e6

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(('', "num_recv_frames=512")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(2),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(freq, 0)
        self.uhd_usrp_source_0.set_normalized_gain(rx_gain, 0)
        self.uhd_usrp_source_0.set_antenna('TX/RX', 0)
        self.uhd_usrp_source_0.set_center_freq(freq, 1)
        self.uhd_usrp_source_0.set_normalized_gain(rx_gain, 1)
        self.uhd_usrp_source_0.set_antenna('TX/RX', 1)
        self.ieee802_15_4_oqpsk_phy_0_0_1 = ieee802_15_4_oqpsk_phy()
        self.ieee802_15_4_oqpsk_phy_0_0 = ieee802_15_4_oqpsk_phy()
        self.ieee802_15_4_mac_0_0_1 = ieee802_15_4.mac(True,0x8841,0,0x1aaa,0xffff,0x3344)
        self.ieee802_15_4_mac_0_0 = ieee802_15_4.mac(True,0x8841,0,0x1aaa,0xffff,0x3344)
        self.foo_wireshark_connector_0_0_0_1 = foo.wireshark_connector(195, False)
        self.foo_wireshark_connector_0_0_0 = foo.wireshark_connector(195, False)
        self.blocks_null_sink_0_0_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_file_sink_0_0_0_1 = blocks.file_sink(gr.sizeof_char*1, '/home/ubuntu/zigbee_branch_2.pcap', False)
        self.blocks_file_sink_0_0_0_1.set_unbuffered(True)
        self.blocks_file_sink_0_0_0 = blocks.file_sink(gr.sizeof_char*1, '/home/ubuntu/zigbee_branch_1.pcap', False)
        self.blocks_file_sink_0_0_0.set_unbuffered(True)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.ieee802_15_4_oqpsk_phy_0_0, 'rxout'), (self.foo_wireshark_connector_0_0_0, 'in'))
        self.msg_connect((self.ieee802_15_4_oqpsk_phy_0_0, 'rxout'), (self.ieee802_15_4_mac_0_0, 'pdu in'))
        self.msg_connect((self.ieee802_15_4_oqpsk_phy_0_0_1, 'rxout'), (self.foo_wireshark_connector_0_0_0_1, 'in'))
        self.msg_connect((self.ieee802_15_4_oqpsk_phy_0_0_1, 'rxout'), (self.ieee802_15_4_mac_0_0_1, 'pdu in'))
        self.connect((self.foo_wireshark_connector_0_0_0, 0), (self.blocks_file_sink_0_0_0, 0))
        self.connect((self.foo_wireshark_connector_0_0_0_1, 0), (self.blocks_file_sink_0_0_0_1, 0))
        self.connect((self.ieee802_15_4_oqpsk_phy_0_0, 0), (self.blocks_null_sink_0_0, 0))
        self.connect((self.ieee802_15_4_oqpsk_phy_0_0_1, 0), (self.blocks_null_sink_0_0_1, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.ieee802_15_4_oqpsk_phy_0_0, 0))
        self.connect((self.uhd_usrp_source_0, 1), (self.ieee802_15_4_oqpsk_phy_0_0_1, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
        self.set_decim(int(self.samp_rate/4e6))

    def get_window_size(self):
        return self.window_size

    def set_window_size(self, window_size):
        self.window_size = window_size

    def get_tran_wid(self):
        return self.tran_wid

    def set_tran_wid(self, tran_wid):
        self.tran_wid = tran_wid

    def get_sync_length(self):
        return self.sync_length

    def set_sync_length(self, sync_length):
        self.sync_length = sync_length

    def get_rx_gain(self):
        return self.rx_gain

    def set_rx_gain(self, rx_gain):
        self.rx_gain = rx_gain
        self.uhd_usrp_source_0.set_normalized_gain(self.rx_gain, 0)

        self.uhd_usrp_source_0.set_normalized_gain(self.rx_gain, 1)


    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.uhd_usrp_source_0.set_center_freq(self.freq, 0)
        self.uhd_usrp_source_0.set_center_freq(self.freq, 1)

    def get_decim(self):
        return self.decim

    def set_decim(self, decim):
        self.decim = decim

    def get_center_offset_2(self):
        return self.center_offset_2

    def set_center_offset_2(self, center_offset_2):
        self.center_offset_2 = center_offset_2

    def get_center_offset_1(self):
        return self.center_offset_1

    def set_center_offset_1(self, center_offset_1):
        self.center_offset_1 = center_offset_1


def main(top_block_cls=top_block, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()

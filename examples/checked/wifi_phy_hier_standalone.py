#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: WiFi PHY Hier
# Generated: Tue Sep 25 12:21:45 2018
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

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import channels
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import foo
import ieee802_11
import pmt
import sip
import sys
from gnuradio import qtgui


class wifi_phy_hier_standalone(gr.top_block, Qt.QWidget):

    def __init__(self, bandwidth=20e6, chan_est=ieee802_11.LS, encoding=ieee802_11.BPSK_1_2, frequency=2.437e9, sensitivity=0.56):
        gr.top_block.__init__(self, "WiFi PHY Hier")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("WiFi PHY Hier")
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

        self.settings = Qt.QSettings("GNU Radio", "wifi_phy_hier_standalone")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Parameters
        ##################################################
        self.bandwidth = bandwidth
        self.chan_est = chan_est
        self.encoding = encoding
        self.frequency = frequency
        self.sensitivity = sensitivity

        ##################################################
        # Variables
        ##################################################
        self.window_size = window_size = 48
        self.sync_length = sync_length = 320
        self.snr = snr = 1
        self.samp_rate = samp_rate = 20e6
        self.pdu_length = pdu_length = 100
        self.out_buf_size = out_buf_size = 96000
        self.max_symbols = max_symbols = 5 + 1 + ((16 + 1540 * 8 + 6) * 2) / 24
        self.interval = interval = 15
        self.header_formatter = header_formatter = ieee802_11.signal_field()
        self.freq = freq = 2.484e9
        self.epsilon = epsilon = 0

        ##################################################
        # Blocks
        ##################################################
        self._snr_range = Range(-15, 100, 0.1, 1, 200)
        self._snr_win = RangeWidget(self._snr_range, self.set_snr, "snr", "counter_slider", float)
        self.top_grid_layout.addWidget(self._snr_win)
        self._epsilon_range = Range(-20e-6, 20e-6, 1e-6, 0, 200)
        self._epsilon_win = RangeWidget(self._epsilon_range, self.set_epsilon, "epsilon", "counter_slider", float)
        self.top_grid_layout.addWidget(self._epsilon_win)
        self.sync_short = ieee802_11.sync_short(sensitivity, 2, False, False)
        self.sync_long = ieee802_11.sync_long(sync_length, False, False)
        self.qtgui_number_sink_0_1 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_NONE,
            1
        )
        self.qtgui_number_sink_0_1.set_update_time(0.10)
        self.qtgui_number_sink_0_1.set_title("")

        labels = ['Total Packets Received', '', '', '', '',
                  '', '', '', '', '']
        units = ['', '', '', '', '',
                 '', '', '', '', '']
        colors = [("black", "white"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_0_1.set_min(i, 0)
            self.qtgui_number_sink_0_1.set_max(i, 100)
            self.qtgui_number_sink_0_1.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0_1.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0_1.set_label(i, labels[i])
            self.qtgui_number_sink_0_1.set_unit(i, units[i])
            self.qtgui_number_sink_0_1.set_factor(i, factor[i])

        self.qtgui_number_sink_0_1.enable_autoscale(False)
        self._qtgui_number_sink_0_1_win = sip.wrapinstance(self.qtgui_number_sink_0_1.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_number_sink_0_1_win)
        self.qtgui_number_sink_0 = qtgui.number_sink(
            gr.sizeof_float,
            0,
            qtgui.NUM_GRAPH_NONE,
            1
        )
        self.qtgui_number_sink_0.set_update_time(0.10)
        self.qtgui_number_sink_0.set_title("")

        labels = ['Error Rate', '', '', '', '',
                  '', '', '', '', '']
        units = ['Percent', '', '', '', '',
                 '', '', '', '', '']
        colors = [("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"),
                  ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black"), ("black", "black")]
        factor = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        for i in xrange(1):
            self.qtgui_number_sink_0.set_min(i, 0)
            self.qtgui_number_sink_0.set_max(i, 100)
            self.qtgui_number_sink_0.set_color(i, colors[i][0], colors[i][1])
            if len(labels[i]) == 0:
                self.qtgui_number_sink_0.set_label(i, "Data {0}".format(i))
            else:
                self.qtgui_number_sink_0.set_label(i, labels[i])
            self.qtgui_number_sink_0.set_unit(i, units[i])
            self.qtgui_number_sink_0.set_factor(i, factor[i])

        self.qtgui_number_sink_0.enable_autoscale(False)
        self._qtgui_number_sink_0_win = sip.wrapinstance(self.qtgui_number_sink_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_number_sink_0_win)
        self.qtgui_const_sink_x_0 = qtgui.const_sink_c(
        	48*10, #size
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_const_sink_x_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_x_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, "")
        self.qtgui_const_sink_x_0.enable_autoscale(False)
        self.qtgui_const_sink_x_0.enable_grid(False)
        self.qtgui_const_sink_x_0.enable_axis_labels(True)

        if not True:
          self.qtgui_const_sink_x_0.disable_legend()

        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "red", "red", "red",
                  "red", "red", "red", "red", "red"]
        styles = [0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0,
                   0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_const_sink_x_0_win)
        self.pfb_arb_resampler_xxx_0 = pfb.arb_resampler_ccf(
        	  1+epsilon,
                  taps=None,
        	  flt_size=32)
        self.pfb_arb_resampler_xxx_0.declare_sample_delay(0)

        self.ieee802_11_soft_frame_equalizer_0 = ieee802_11.soft_frame_equalizer(ieee802_11.LS, 2.437e9, 20e6, 0, 0, False, False)
        self.ieee802_11_parse_mac_0 = ieee802_11.parse_mac(False, False)
        self.ieee802_11_moving_average_xx_1 = ieee802_11.moving_average_cc(window_size)
        self.ieee802_11_moving_average_xx_0 = ieee802_11.moving_average_ff(window_size + 16)
        self.ieee802_11_mapper_0 = ieee802_11.mapper(encoding, False)
        self.ieee802_11_mac_0 = ieee802_11.mac(([0x23, 0x23, 0x23, 0x23, 0x23, 0x23]), ([0x42, 0x42, 0x42, 0x42, 0x42, 0x42]), ([0xff, 0xff, 0xff, 0xff, 0xff, 0xff]))
        self.ieee802_11_decode_mac_soft_1 = ieee802_11.decode_mac_soft(False, False)
        self.ieee802_11_chunks_to_symbols_xx_0 = ieee802_11.chunks_to_symbols()
        (self.ieee802_11_chunks_to_symbols_xx_0).set_min_output_buffer(397056)
        self.foo_wireshark_connector_0_0 = foo.wireshark_connector(127, False)
        self.foo_wireshark_connector_0 = foo.wireshark_connector(127, False)
        self.foo_packet_pad2_0 = foo.packet_pad2(False, False, 0.001, 500, 0)
        (self.foo_packet_pad2_0).set_min_output_buffer(96000)
        self.fft_vxx_0_1 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.fft_vxx_0_0 = fft.fft_vcc(64, False, (tuple([1/52**.5] * 64)), True, 1)
        (self.fft_vxx_0_0).set_min_output_buffer(397056)
        self.digital_packet_headergenerator_bb_0 = digital.packet_headergenerator_bb(header_formatter.formatter(), "packet_len")
        self.digital_ofdm_cyclic_prefixer_0_0 = digital.ofdm_cyclic_prefixer(64, 64+16, 2, "packet_len")
        (self.digital_ofdm_cyclic_prefixer_0_0).set_min_output_buffer(397056)
        self.digital_ofdm_carrier_allocator_cvc_0_0_0 = digital.ofdm_carrier_allocator_cvc(64, (range(-26, -21) + range(-20, -7) + range(-6, 0) + range(1, 7) + range(8, 21) + range(22, 27),), ((-21, -7, 7, 21), ), ((1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1)), ((0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0), (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0), (0, 0j, 0, 0j, 0, 0j, -1, 1j, -1, 1j, -1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, (-0-1j), 1, -1j, -1, 1j, 0, -1j, 1, (-0-1j), 1, -1j, 1, 1j, -1, -1j, 1, (-0-1j), -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 0j, 0, 0j, 0, 0j), (0, 0, 0, 0, 0, 0, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 0, 0, 0, 0)), "packet_len")
        (self.digital_ofdm_carrier_allocator_cvc_0_0_0).set_min_output_buffer(397056)
        self.digital_chunks_to_symbols_xx_0 = digital.chunks_to_symbols_bc(([-1, 1]), 1)
        self.channels_channel_model_0 = channels.channel_model(
        	noise_voltage=1,
        	frequency_offset=epsilon * freq/ samp_rate,
        	epsilon=1.0,
        	taps=(1.0,0.09),
        	noise_seed=0,
        	block_tags=False
        )
        self.blocks_tagged_stream_mux_0 = blocks.tagged_stream_mux(gr.sizeof_gr_complex*1, "packet_len", 1)
        (self.blocks_tagged_stream_mux_0).set_min_output_buffer(397056)
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(gr.sizeof_gr_complex*1, 64)
        self.blocks_pdu_to_tagged_stream_0_2 = blocks.pdu_to_tagged_stream(blocks.float_t, 'packet_len')
        self.blocks_pdu_to_tagged_stream_0_0 = blocks.pdu_to_tagged_stream(blocks.complex_t, 'packet_len')
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.float_t, 'packet_len')
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_char*48)
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc(((10**(snr/10.0))**.5, ))
        self.blocks_message_strobe_0 = blocks.message_strobe(pmt.intern("".join("x" for i in range(pdu_length))), interval)
        self.blocks_file_sink_0_0 = blocks.file_sink(gr.sizeof_char*1, '/home/john/t0+25/null_llr_testing/transmitted_wifi.pcap', False)
        self.blocks_file_sink_0_0.set_unbuffered(True)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_char*1, '/home/john/t0+25/null_llr_testing/received_wifi.pcap', False)
        self.blocks_file_sink_0.set_unbuffered(True)
        self.blocks_divide_xx_0 = blocks.divide_ff(1)
        self.blocks_delay_0_0 = blocks.delay(gr.sizeof_gr_complex*1, 16)
        self.blocks_delay_0 = blocks.delay(gr.sizeof_gr_complex*1, sync_length)
        self.blocks_conjugate_cc_0 = blocks.conjugate_cc()
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_message_strobe_0, 'strobe'), (self.ieee802_11_mac_0, 'app in'))
        self.msg_connect((self.ieee802_11_decode_mac_soft_1, 'out'), (self.foo_wireshark_connector_0, 'in'))
        self.msg_connect((self.ieee802_11_decode_mac_soft_1, 'out'), (self.ieee802_11_parse_mac_0, 'in'))
        self.msg_connect((self.ieee802_11_mac_0, 'phy out'), (self.foo_wireshark_connector_0_0, 'in'))
        self.msg_connect((self.ieee802_11_mac_0, 'phy out'), (self.ieee802_11_mapper_0, 'in'))
        self.msg_connect((self.ieee802_11_parse_mac_0, 'fer'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))
        self.msg_connect((self.ieee802_11_parse_mac_0, 'tpr'), (self.blocks_pdu_to_tagged_stream_0_2, 'pdus'))
        self.msg_connect((self.ieee802_11_soft_frame_equalizer_0, 'symbols'), (self.blocks_pdu_to_tagged_stream_0_0, 'pdus'))
        self.connect((self.blocks_complex_to_mag_0, 0), (self.blocks_divide_xx_0, 0))
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.ieee802_11_moving_average_xx_0, 0))
        self.connect((self.blocks_conjugate_cc_0, 0), (self.blocks_multiply_xx_0, 1))
        self.connect((self.blocks_delay_0, 0), (self.sync_long, 1))
        self.connect((self.blocks_delay_0_0, 0), (self.blocks_conjugate_cc_0, 0))
        self.connect((self.blocks_delay_0_0, 0), (self.sync_short, 0))
        self.connect((self.blocks_divide_xx_0, 0), (self.sync_short, 2))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.channels_channel_model_0, 0))
        self.connect((self.blocks_multiply_xx_0, 0), (self.ieee802_11_moving_average_xx_1, 0))
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.qtgui_number_sink_0, 0))
        self.connect((self.blocks_pdu_to_tagged_stream_0_0, 0), (self.qtgui_const_sink_x_0, 0))
        self.connect((self.blocks_pdu_to_tagged_stream_0_2, 0), (self.qtgui_number_sink_0_1, 0))
        self.connect((self.blocks_stream_to_vector_0, 0), (self.fft_vxx_0_1, 0))
        self.connect((self.blocks_tagged_stream_mux_0, 0), (self.digital_ofdm_carrier_allocator_cvc_0_0_0, 0))
        self.connect((self.channels_channel_model_0, 0), (self.pfb_arb_resampler_xxx_0, 0))
        self.connect((self.digital_chunks_to_symbols_xx_0, 0), (self.blocks_tagged_stream_mux_0, 0))
        self.connect((self.digital_ofdm_carrier_allocator_cvc_0_0_0, 0), (self.fft_vxx_0_0, 0))
        self.connect((self.digital_ofdm_cyclic_prefixer_0_0, 0), (self.foo_packet_pad2_0, 0))
        self.connect((self.digital_packet_headergenerator_bb_0, 0), (self.digital_chunks_to_symbols_xx_0, 0))
        self.connect((self.fft_vxx_0_0, 0), (self.digital_ofdm_cyclic_prefixer_0_0, 0))
        self.connect((self.fft_vxx_0_1, 0), (self.ieee802_11_soft_frame_equalizer_0, 0))
        self.connect((self.foo_packet_pad2_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.foo_wireshark_connector_0, 0), (self.blocks_file_sink_0, 0))
        self.connect((self.foo_wireshark_connector_0_0, 0), (self.blocks_file_sink_0_0, 0))
        self.connect((self.ieee802_11_chunks_to_symbols_xx_0, 0), (self.blocks_tagged_stream_mux_0, 1))
        self.connect((self.ieee802_11_mapper_0, 0), (self.digital_packet_headergenerator_bb_0, 0))
        self.connect((self.ieee802_11_mapper_0, 0), (self.ieee802_11_chunks_to_symbols_xx_0, 0))
        self.connect((self.ieee802_11_moving_average_xx_0, 0), (self.blocks_divide_xx_0, 1))
        self.connect((self.ieee802_11_moving_average_xx_1, 0), (self.blocks_complex_to_mag_0, 0))
        self.connect((self.ieee802_11_moving_average_xx_1, 0), (self.sync_short, 1))
        self.connect((self.ieee802_11_soft_frame_equalizer_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.ieee802_11_soft_frame_equalizer_0, 1), (self.ieee802_11_decode_mac_soft_1, 0))
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.blocks_complex_to_mag_squared_0, 0))
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.blocks_delay_0_0, 0))
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.blocks_multiply_xx_0, 0))
        self.connect((self.sync_long, 0), (self.blocks_stream_to_vector_0, 0))
        self.connect((self.sync_short, 0), (self.blocks_delay_0, 0))
        self.connect((self.sync_short, 0), (self.sync_long, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "wifi_phy_hier_standalone")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_bandwidth(self):
        return self.bandwidth

    def set_bandwidth(self, bandwidth):
        self.bandwidth = bandwidth

    def get_chan_est(self):
        return self.chan_est

    def set_chan_est(self, chan_est):
        self.chan_est = chan_est

    def get_encoding(self):
        return self.encoding

    def set_encoding(self, encoding):
        self.encoding = encoding
        self.ieee802_11_mapper_0.set_encoding(self.encoding)

    def get_frequency(self):
        return self.frequency

    def set_frequency(self, frequency):
        self.frequency = frequency

    def get_sensitivity(self):
        return self.sensitivity

    def set_sensitivity(self, sensitivity):
        self.sensitivity = sensitivity

    def get_window_size(self):
        return self.window_size

    def set_window_size(self, window_size):
        self.window_size = window_size
        self.ieee802_11_moving_average_xx_1.set_length(self.window_size)
        self.ieee802_11_moving_average_xx_0.set_length(self.window_size + 16)

    def get_sync_length(self):
        return self.sync_length

    def set_sync_length(self, sync_length):
        self.sync_length = sync_length
        self.blocks_delay_0.set_dly(self.sync_length)

    def get_snr(self):
        return self.snr

    def set_snr(self, snr):
        self.snr = snr
        self.blocks_multiply_const_vxx_0.set_k(((10**(self.snr/10.0))**.5, ))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.channels_channel_model_0.set_frequency_offset(self.epsilon * self.freq/ self.samp_rate)

    def get_pdu_length(self):
        return self.pdu_length

    def set_pdu_length(self, pdu_length):
        self.pdu_length = pdu_length
        self.blocks_message_strobe_0.set_msg(pmt.intern("".join("x" for i in range(self.pdu_length))))

    def get_out_buf_size(self):
        return self.out_buf_size

    def set_out_buf_size(self, out_buf_size):
        self.out_buf_size = out_buf_size

    def get_max_symbols(self):
        return self.max_symbols

    def set_max_symbols(self, max_symbols):
        self.max_symbols = max_symbols

    def get_interval(self):
        return self.interval

    def set_interval(self, interval):
        self.interval = interval
        self.blocks_message_strobe_0.set_period(self.interval)

    def get_header_formatter(self):
        return self.header_formatter

    def set_header_formatter(self, header_formatter):
        self.header_formatter = header_formatter

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.channels_channel_model_0.set_frequency_offset(self.epsilon * self.freq/ self.samp_rate)

    def get_epsilon(self):
        return self.epsilon

    def set_epsilon(self, epsilon):
        self.epsilon = epsilon
        self.pfb_arb_resampler_xxx_0.set_rate(1+self.epsilon)
        self.channels_channel_model_0.set_frequency_offset(self.epsilon * self.freq/ self.samp_rate)


def argument_parser():
    parser = OptionParser(usage="%prog: [options]", option_class=eng_option)
    parser.add_option(
        "", "--bandwidth", dest="bandwidth", type="eng_float", default=eng_notation.num_to_str(20e6),
        help="Set bandwidth [default=%default]")
    parser.add_option(
        "", "--encoding", dest="encoding", type="intx", default=ieee802_11.BPSK_1_2,
        help="Set encoding [default=%default]")
    parser.add_option(
        "", "--frequency", dest="frequency", type="eng_float", default=eng_notation.num_to_str(2.437e9),
        help="Set frequency [default=%default]")
    parser.add_option(
        "", "--sensitivity", dest="sensitivity", type="eng_float", default=eng_notation.num_to_str(0.56),
        help="Set sensitivity [default=%default]")
    return parser


def main(top_block_cls=wifi_phy_hier_standalone, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(bandwidth=options.bandwidth, encoding=options.encoding, frequency=options.frequency, sensitivity=options.sensitivity)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()

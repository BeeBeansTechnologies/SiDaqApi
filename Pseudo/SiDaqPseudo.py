# -*- coding:utf-8 -*-
"""
SiDaq Pseudo Device
"""


from __future__ import print_function

import argparse
from logging import getLogger, StreamHandler, INFO
import select
import socket

from sitcpy import to_bytearray
import sitcpy
from sitcpy.cui import SessionThread, CuiServer

from sitcpy.rbcp_server import RbcpCommandHandler, DataGenerator, RbcpServer, PseudoDevice

LOGGER = getLogger(__name__)
HANDLER = StreamHandler()
HANDLER.setLevel(INFO)
LOGGER.setLevel(INFO)
LOGGER.addHandler(HANDLER)

class PseudoDataGenerator(DataGenerator):
    """
    Data Generator. create data for emulation
    """

    def __init__(self, header, send_unit):
        """
        Constructor.

        :type header: int
        :param header: header length(byte).

        :type send_unit: int
        :param send_unit: Unit send in once sock.send(). send_unit * 8 bytes


        """
        super(PseudoDataGenerator, self).__init__()

        MAX_UNIT_COUNT = 4096

        self._data_buffer = bytearray([0x1F, 0x3E, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB])
        self._data_unit = len(self._data_buffer)# bytes.
        self._header = 0 if header < 0 else header
        self._send_unit = 1 if send_unit < 1 else MAX_UNIT_COUNT if send_unit > MAX_UNIT_COUNT else send_unit
        self._check_header_length()

    def create_fixed_pattern(self):
        """
        :return: Created fixed pattern data (byte array).
        """
        data = self._data_buffer * self._send_unit
        if self._header != 0:
            data_len = len(data)
            if data_len.bit_length() <= self._header * 8:
                data_count_header = bytearray((len(data)).to_bytes(self._header)) # not include header in data length
                data = data_count_header + data
        return data

    def create_data(self):
        """
        Create unit data
        """
        return self.create_fixed_pattern()
    
    def _check_header_length(self):
        """
        Check if it is long enough
        """

        data_len = self._data_unit * self._send_unit
        if data_len.bit_length() > self._header * 8:
            LOGGER.warning("Header length is not long enough. header is not added.")
            self._header = 0

        return
    
class PseudoRbcpCommandHandler(RbcpCommandHandler):
    """
    Command handler
    """

def on_cmd_mycmd(self, session, cmd_list):
    """
    This is sample command. display received arguments.

    :usage: mycmd [arg1 [arg2 ...]]: sample command.
    """
    self.reply_text(session, "mycmd received arguments: %s" % str(cmd_list))
    return True

def on_cmd_set_generate_mode(self, generate_mode):
    pass

class SiDaqSessionThreadGen(SessionThread):
    """
    SessionThread class for data generation.
    from sitcpy.cuisvr import CuiSvr
    from sitcpy.rbcp_server import SessionThreadGen, DataGenerator
    srv = CuiSvr(8888,SessionThreadGen, DataGenerator())
    srv.run()
    """

    def __init__(self, server, data_generator, sock, client_address, max_buff=1024 * 1024):
        """
        Constructor.

        :type server: CuiServer
        :param server: The server that owns this session.

        :type data_generator: DataGenerator
        :param data_generator: Pass the received data to this data generator.

        :type sock: socket.socket or None
        :param sock: Client socket.

        :type client_address: str
        :param client_address: Client IP address.

        :type max_buff: int
        :param max_buff: maximum receive buffer for sock.recv().
        """
        super(SiDaqSessionThreadGen, self).__init__(server, data_generator, sock, client_address, max_buff)

    def run(self):
        """
        Send generated data by the DataGenerator to the session
        """
        # pylint: disable=unused-variable
        try:
            print("starting session from client " + str(self._client_address))
            self._data_handler.on_start(self)
            write_list = [self._sock]

            self._state.transit(sitcpy.THREAD_RUNNING)
            while self._state() == sitcpy.THREAD_RUNNING:
                try:
                    _, writable, _ = select.select([], write_list, [], 0.1)
                    if self._sock in writable:
                        data = self._data_handler.create_data()
                        self._sock.send(data)
                except (OSError, socket.error) as exc:
                    LOGGER.debug("Exception at SessionThreadGen.run : %s" %
                                 str(exc))
                    LOGGER.debug("Pseudo Data Session Closed")
                    break

            self._state.transit(sitcpy.THREAD_STOPPING)
            del write_list[:]
            self.close()
        finally:
            self._state.transit(sitcpy.THREAD_STOPPED)


class SiDaqPseudoDevice(PseudoDevice):
    """
    Pseudo device with command handler, data generator, and bcp emulator
    """

    def __init__(self, rbcp_command_handler, data_generator, rbcp_server,
                 command_port=9090, data_port=24242):
        """
        Constructor.

        :type rbcp_command_handler: RbcpCommandHandler
        :type data_generator: DataGenerator
        :type rbcp_server: RbcpServer
        :type command_port: int
        :type data_port: int
        """
        self._cuisvr = CuiServer(SessionThread, rbcp_command_handler,
                                 command_port)
        self._rbcp_server = rbcp_server
        self._pseudo_generator = CuiServer(SiDaqSessionThreadGen, data_generator,
                                           data_port)
        # TODO: Stateへ置き換え
        self._continue = True
        self._thread = None  # for start() -- thread mode.

def sidaq_pseudo_arg_parser():
    """
    Create ArgumentParser for pseudo device.

    :rtype: argparse.ArgumentParser
    :return: Default argument parser.
    """
    arg_parser = argparse.ArgumentParser(
        description="pseudo device main.")

    arg_parser.add_argument("--port", "--dataport", type=int,
                            default=24242, help="emulation data port number")
    arg_parser.add_argument("--header", "--header", type=int,
                            default=2, help="data header length")
    
    arg_parser.add_argument("--unit", "--sendunit", type=int,
                            default= 4096, help="send data unit count")
    return arg_parser

def main(args=None):
    """
    Sample pseudo SiTCP Device.
    """
    args = sidaq_pseudo_arg_parser().parse_args(args)

    command_port = 9090
    data_port = args.port
    header = args.header
    send_unit = args.unit
    udp_port = 4660

    rbcp_server = RbcpServer(udp_port=udp_port)
    rbcp_server.write_registers(0xFFFFFF22, udp_port.to_bytes(2,'big')) # Set SiTCP Register
    rbcp_server.write_registers(0xFFFFFF1C, data_port.to_bytes(2,'big')) # Set SiTCP Register
    data_generator = PseudoDataGenerator(header, send_unit)
    rbcp_command_handler = PseudoRbcpCommandHandler("pdev$ ")
    rbcp_command_handler.bind(rbcp_server, data_generator)
    pdev = SiDaqPseudoDevice(rbcp_command_handler, data_generator, rbcp_server,
                        command_port, data_port)
    pdev.run_loop()


if __name__ == "__main__":
    main()
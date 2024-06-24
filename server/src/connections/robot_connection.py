from typing import Any
from src.model.robot import Robot
from src.connections.connection import Connection
from src.model.user import User
from src.model.command_type import CommandType
import time

from strategies.strategies import Strategy2


class RobotConnection(Connection):
    def __init__(self, ws: Any) -> None:
        super().__init__(ws)
        self.last_transmission_time = time.time()
    
    def create_user(self) -> Robot:
        return Robot()
    
    def set_command_handlers(self)-> None:
        super().set_command_handlers()
        self.command_handlers[CommandType.FRAME] = self.handle_retransmit
        self.command_handlers[CommandType.DISTANCE_DATA] = self.handle_distance_data
        self.command_handlers[CommandType.RING_EDGE_DATA] = self.handle_ring_edge_data

    def handle_ring_edge_data(self, command:bytes)-> None:
        robot:Robot = self.user
        robot.sensorData.edgeData = command[1::]
        self.user.send_message_to_linked_user(command)
    
    def handle_distance_data(self, command:bytes)-> None:
        robot:Robot = self.user
        robot.sensorData.distanceData = command[1::]
        self.user.send_message_to_linked_user(command)
    
    def handle_retransmit(self, command:bytes) -> None:
        self.user.send_message_to_linked_user(command)

    def loop(self):
        robot:Robot = self.user
        message = robot.get_next_message()
        while message :
            if message[0] == CommandType.STRATEGY.value:
                if message[1] == 0:
                    robot.strategy = None
                    robot.isIdle = True
                    self.transmit_command(message)
                elif message[1] == 1:
                    robot.strategy = None
                    robot.isIdle = False
                elif message[1] == 2:
                    robot.isIdle = False
                    robot.strategy = Strategy2()
            elif message[0] == CommandType.MOTOR_POWER.value:
                robot.isIdle = False
                robot.controlData.motorPowers = message[1:]
            else:
                self.transmit_command(message)
            # print(f"Robot {self.user.uid} received message: {message}")
            message = robot.get_next_message()


        # If connected User is not connected stop
        if robot.hasLinkedUserConnected() == False or robot.isIdle:
            return
        
        robot.runStrategy()
        
        # Call sendControlData fuction
        current_time = time.time()
        if (current_time - self.last_transmission_time) * 1000 >= 50:  # Convert to milliseconds
            self.sendControlData()
            self.last_transmission_time = current_time  

    def sendControlData(self):
        robot:Robot = self.user
        powers = robot.controlData.motorPowers
        if powers:
            command = CommandType.MOTOR_POWER.toBytes(powers)
            self.transmit_command(command)

            
        
        

        
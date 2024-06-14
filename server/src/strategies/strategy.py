from src.model.robot_sensors_data import RobotControlData, RobotSensorsData
import struct

class Strategy:
    def run(self, robotSensorsData :RobotSensorsData) -> RobotControlData:
        pass

class Strategy2(Strategy):
    def run(self, robotSensorsData: RobotSensorsData) -> RobotControlData:
        # Call getControlData with the instance (self) and pass the arguments
        return self.getControlData(10, 10)
    
    def getControlData(self, powerLeft: int, powerRight: int) -> RobotControlData:
        # Create a new instance of RobotControlData
        robotData = RobotControlData()
        # Pack powerLeft and powerRight as signed bytes and assign them to motorPowers
        robotData.motorPowers = bytearray(struct.pack('bb', powerLeft, powerRight))
        return robotData
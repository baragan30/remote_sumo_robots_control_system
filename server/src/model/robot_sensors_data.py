from typing import Optional
import struct

class RobotSensorsData:
    def __init__(self) -> None:
        self._frame:Optional[bytes] = None
        self._distanceData:Optional[bytes] = None
        self._edgeData:Optional[bytes] = None

    def reset(self) -> None:
        self.distanceData = None
        self.edgeData = None

    @property
    def frame(self) -> bytes:
        return self._frame

    @frame.setter
    def frame(self, value: bytes) -> None:
        self._frame = value

    @property
    def distanceData(self) -> bytes:
        return self._distanceData

    @distanceData.setter
    def distanceData(self, value: bytes) -> None:
        self._distanceData = value

    @property
    def edgeData(self) -> bytes:
        return self._edgeData

    @edgeData.setter
    def edgeData(self, value: bytes) -> None:
        self._edgeData = value

class RobotControlData:
    def __init__(self) -> None:
        self._motorPowers: Optional[bytes] = None

    @property
    def motorPowers(self) -> bytes:
        return self._motorPowers

    @motorPowers.setter
    def motorPowers(self, value: bytes) -> None:
        self._motorPowers = value

    @staticmethod
    def from_powers(powerLeft: int, powerRight: int) -> 'RobotControlData':
        # Create a new instance of RobotControlData
        robotData = RobotControlData()
        # Pack powerLeft and powerRight as signed bytes and assign them to motorPowers
        robotData.motorPowers = bytearray(struct.pack('bb', powerLeft, powerRight))
        return robotData
    
    SLOW_POWER = 40
    
    @staticmethod
    def stop() -> 'RobotControlData':
         return RobotControlData.from_powers(0, 0)
    
    @staticmethod
    def left_rotate() -> 'RobotControlData':
         return RobotControlData.from_powers(-100, 100)
    
    @staticmethod
    def left_slow_rotate() -> 'RobotControlData':
         return RobotControlData.from_powers(-RobotControlData.SLOW_POWER, RobotControlData.SLOW_POWER)
    
    @staticmethod
    def right_rotate() -> 'RobotControlData':
         return RobotControlData.from_powers(100, -100)
    
    @staticmethod
    def right_slow_rotate() -> 'RobotControlData':
         return RobotControlData.from_powers(RobotControlData.SLOW_POWER, -RobotControlData.SLOW_POWER)
    
    @staticmethod
    def forward() -> 'RobotControlData':
         return RobotControlData.from_powers(100, 100)
    
    @staticmethod
    def forward_left() -> 'RobotControlData':
         return RobotControlData.from_powers(2, 100)
    
    @staticmethod
    def forward_slow() -> 'RobotControlData':
         return RobotControlData.from_powers(RobotControlData.SLOW_POWER, RobotControlData.SLOW_POWER)
    
    @staticmethod
    def forward_right() -> 'RobotControlData':
         return RobotControlData.from_powers(100, 2)
    
    @staticmethod
    def backward() -> 'RobotControlData':
         return RobotControlData.from_powers(-100, -100)
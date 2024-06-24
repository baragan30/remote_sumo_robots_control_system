from typing import Optional
import struct

class RobotSensorsData:
    def __init__(self) -> None:
        self._frame:Optional[bytes] = None
        self._distanceData:Optional[bytes] = None
        self._edgeData:Optional[bytes] = None

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
        self._motorPowers:Optional[bytes] = None

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
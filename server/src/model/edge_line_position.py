from enum import Enum
from typing import Optional

from src.model.robot_sensors_data import RobotSensorsData

class SimplePreprocesedData:
    def __init__(self, robotSensorsData:RobotSensorsData) -> None:
        self.edgePosition = EdgeLinePosition.from_sensors_bytes(robotSensorsData.edgeData)


class EdgeLinePosition(Enum):
    EXTREME_LEFT = -2
    LEFT = -1
    CENTER = 0
    RIGHT = 1
    EXTREME_RIGHT = 2
    NO_POSITION = 100

    @staticmethod
    def from_sensors_bytes(sensor_byte: Optional[bytes]) -> 'EdgeLinePosition':
        if sensor_byte is None or len(sensor_byte) == 0:
            return EdgeLinePosition.NO_POSITION

        # We are interested in the first and single byte of the sensor_byte
        byte = sensor_byte[0]

        # Extract sensor states from the byte
        s1 = byte & 0b00001
        s2 = (byte >> 1) & 0b00001
        s3 = (byte >> 2) & 0b00001
        s4 = (byte >> 3) & 0b00001
        s5 = (byte >> 4) & 0b00001

        # Count the number of sensors that detected the line
        nr = s1 + s2 + s3 + s4 + s5

        if nr == 0:
            return EdgeLinePosition.NO_POSITION

        # Compute a weighted arithmetic mean
        relative_position = ((s5 * 2 + s4) - (s2 + s1 * 2)) / nr

        # Convert continuous position to discrete enum based on value ranges
        if relative_position <= -1:
            return EdgeLinePosition.EXTREME_LEFT
        elif -1 < relative_position <= -0.5:
            return EdgeLinePosition.LEFT
        elif -0.5 < relative_position < 0.5:
            return EdgeLinePosition.CENTER
        elif 0.5 <= relative_position < 1:
            return EdgeLinePosition.RIGHT
        elif relative_position >= 1:
            return EdgeLinePosition.EXTREME_RIGHT


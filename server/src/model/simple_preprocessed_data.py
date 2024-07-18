from enum import Enum
from typing import Optional, Tuple

from src.model.robot_sensors_data import RobotSensorsData

class SimplePreprocessedData:
    def __init__(self, robotSensorsData:RobotSensorsData) -> None:
        self.edgePosition = EdgeLinePosition.from_sensors_bytes(robotSensorsData.edgeData)
        self.enemyPosition = EnemyPosition.from_distance_data(robotSensorsData.distanceData)


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
        if nr != 0:
            # Calculate relative position and immediately convert to integer
            relative_position = int(((s5 * 2 + s4) - (s2 + s1 * 2)) / nr)
        else:
            return EdgeLinePosition.NO_POSITION

        # Use direct integer comparison
        if relative_position == -2:
            return EdgeLinePosition.EXTREME_LEFT
        elif relative_position == -1:
            return EdgeLinePosition.LEFT
        elif relative_position == 0:
            return EdgeLinePosition.CENTER
        elif relative_position == 1:
            return EdgeLinePosition.RIGHT
        elif relative_position == 2:
            return EdgeLinePosition.EXTREME_RIGHT

class EnemyPosition:
    # Define a constant for the maximum threshold
    THRESHOLD_MAX = 40

    def __init__(self) -> None:
        self.angle: Optional[float] = None
        self.distance: Optional[float] = None

    def isDetected(self) -> bool:
        return self.angle != None and self.distance != None
    
    @staticmethod
    def weighted_mean_and_angle(values: Tuple[int, int, int], start_index: int) -> Tuple[float, float]:
        # Handle the case where the middle value is zero or too high
        if values[1] == 0 or values[1] > EnemyPosition.THRESHOLD_MAX:
            return 0, (start_index + 1) * 15

        # Filter values to include only those within the acceptable range
        valid_values = [val if 1 <= val <= EnemyPosition.THRESHOLD_MAX else 0 for val in values]

        # Compute inverted weights, where lower values give higher weights, and normalize them
        inverted_distances = [1 / val if val > 0 else 0 for val in valid_values]
        sum_inverted_distances = sum(inverted_distances)
        
        # Normalize weights so that their sum is 1
        if sum_inverted_distances == 0:
            return 0, (start_index + 1) * 15  # If all weights are zero, return default angle
        
        normalized_weights = [weight / sum_inverted_distances for weight in inverted_distances]

       
        angles = [start_index * 15, (start_index + 1) * 15, (start_index + 2) * 15]
        
        # Compute weighted average for distance and angle
        weighted_angles = [angle * weight for angle, weight in zip(angles, normalized_weights)]
        weighted_distances = [value * weight for value, weight in zip(valid_values, normalized_weights)]

        weighted_mean_distance = sum(weighted_distances)
        weighted_mean_angle = sum(weighted_angles)

        return weighted_mean_distance, weighted_mean_angle
    
    @staticmethod
    def from_distance_data(data: bytes) -> 'EnemyPosition':
        if data == None:
            return EnemyPosition()
        # Calculate weighted means and associated angles
       # Calculate weighted means and corresponding angles for each set of three adjacent values
        weighted_distances_and_angles = [
            EnemyPosition.weighted_mean_and_angle((data[i], data[i+1], data[i+2]), i)
            for i in range(len(data) - 2)
        ]

        # Filter out entries where the distance is zero
        valid_entries = [(distance, angle) for distance, angle in weighted_distances_and_angles if distance > 0]

        # Find the angle and distance where the minimum weighted mean distance is found
        if valid_entries:
            min_distance, min_angle = min(valid_entries, key=lambda x: x[0])

            # Create an instance with the computed angle and distance
            position = EnemyPosition()
            position.angle = min_angle
            position.distance = min_distance
            return position

        return EnemyPosition()

        

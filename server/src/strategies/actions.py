import time
from src.model.simple_preprocessed_data import EdgeLinePosition, SimplePreprocessedData
from src.model.robot_sensors_data import RobotControlData


class Action:
    def __init__(self,robotControlData:RobotControlData) -> None:
        self.robotControlData = robotControlData
    
    def isUnfolding(data:SimplePreprocessedData) -> bool:
        pass

class TimeAction(Action):
    def __init__(self, robotControlData: RobotControlData, duration_ms: int) -> None:
        super().__init__(robotControlData)
        self.duration_ms = duration_ms / 1000.0
        self.end_time = None

    def start(self):
        """Start the timer for the action."""
        self.end_time = time.time() + self.duration_ms

    def isUnfolding(self, data: SimplePreprocessedData) -> bool:
        """Check if the action is still unfolding."""
        if self.end_time is None:
            self.start()  # Initialize the timer when first checked, if not started explicitly.
        return time.time() < self.end_time

class EdgeDetectionAction(Action):
    def isUnfolding(self, data: SimplePreprocessedData) -> bool:
        return data.edgePosition != EdgeLinePosition.NO_POSITION
import time
from src.model.edge_line_position import EdgeLinePosition, SimplePreprocesedData
from src.model.robot_sensors_data import RobotControlData


class Action:
    def __init__(self,robotControlData:RobotControlData) -> None:
        self.robotControlData = robotControlData

    def start():
        pass
    
    def isUnfolding(data:SimplePreprocesedData) -> bool:
        pass

class TimeAction(Action):
    def __init__(self, robotControlData: RobotControlData, duration_ms: int) -> None:
        super().__init__(robotControlData)
        self.duration = (duration_ms / 1000.0)
        self.end_time = None  # To store the end time of the action

    def start(self):
        # Set the end time based on current time and duration
        self.end_time = time.time() + self.duration

    def isUnfolding(self, data: SimplePreprocesedData) -> bool:
        # Check if current time is less than the end time
        return time.time() < self.end_time

class EdgeDetectionAction(Action):
    def isUnfolding(self, data: SimplePreprocesedData) -> bool:
        return data.edgePosition != EdgeLinePosition.NO_POSITION
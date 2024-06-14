from typing import Optional
from src.model.robot_sensors_data import RobotControlData, RobotSensorsData
from src.model.user import User
from src.strategies.strategy import Strategy


class Robot(User):
    def __init__(self) -> None:
        super().__init__()
        self.sensorData = RobotSensorsData()
        self.controlData = RobotControlData()
        self.strategy:Optional[Strategy] = None
        self.isIdle = True

    def getName(self) -> str:
        return f"Robot {self.uid}"
    
    def runStrategy(self) ->None:
        if self.strategy:
            self.controlData = self.strategy.run(self.sensorData)
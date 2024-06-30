from typing import Optional
from src.model.robot_sensors_data import RobotControlData, RobotSensorsData
from src.model.user import User
from src.strategies.strategies import Strategy


class Robot(User):
    def __init__(self) -> None:
        super().__init__()
        self.sensorData = RobotSensorsData()
        self.controlData = RobotControlData()
        self.strategy:Optional[Strategy] = None
        self.isIdle = True

    def getName(self) -> str:
        return f"Robot {self.uid}"
    
    def setStrategy(self, strategy:Strategy):
        """Sets the strategy for the robot if it is of a different class type than the current strategy."""
        if not isinstance(self.strategy, strategy.__class__):
            self.sensorData.reset()
            self.strategy = strategy

    def removeStrategy(self):
        """Sets the strategy for the robot if it is of a different class type than the current strategy."""
        self.strategy = None
    
    def runStrategy(self) ->None:
        if self.strategy:
            self.controlData = self.strategy.run(self.sensorData)
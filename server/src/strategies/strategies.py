from typing import Optional,TypeVar, Generic, Optional
from abc import ABC, abstractmethod

from src.model.edge_line_position import EdgeLinePosition, SimplePreprocesedData
from src.model.robot_sensors_data import RobotControlData, RobotSensorsData
from strategies.tactics import Tactic,AdvanceAndPivotTactic,EdgeReflexTurnBackTactic

T = TypeVar('T')

class Strategy(Generic[T], ABC):
    def __init__(self) ->None:
        self.tactic:Optional[Tactic] = None

    def run(self, robotSensorsData :RobotSensorsData) -> RobotControlData:
        preprocessedData = self.preprocessData(robotSensorsData)
        if self.tactic == None:
            self.tactic = self.chooseTactic(preprocessedData)
        controlData = None
        while controlData == None:
            controlData = self.tactic.run(preprocessedData)
            if controlData == None:
                self.tactic = self.chooseTactic(preprocessedData)
            
        return controlData
            
    def chooseTactic(self, data :T) -> Tactic:
        pass

    def preprocessData(self, robotSensorsData :RobotSensorsData) -> T:
        pass

class SimplePreprocessedDataStrategy(Strategy[SimplePreprocesedData]):
    def preprocessData(self, robotSensorsData: RobotSensorsData) -> SimplePreprocesedData:
        return SimplePreprocesedData(robotSensorsData)

class Strategy2(SimplePreprocessedDataStrategy):
    def chooseTactic(self, data :SimplePreprocesedData) -> Tactic:
        edgeLinePosition = data.edgePosition
        if edgeLinePosition == EdgeLinePosition.NO_POSITION :
            return AdvanceAndPivotTactic()
        return EdgeReflexTurnBackTactic()
    
    
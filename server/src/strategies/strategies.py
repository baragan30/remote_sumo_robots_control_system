from typing import Optional,TypeVar, Generic, Optional
from abc import ABC, abstractmethod

from src.model.simple_preprocessed_data import EdgeLinePosition, SimplePreprocessedData
from src.model.robot_sensors_data import RobotControlData, RobotSensorsData
from src.strategies.tactics import Tactic,AdvanceAndPivotTactic,EdgeReflexTurnBackTactic,StraightAttack

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
                print(f"Chage tactic {self.tactic}")
        return controlData
            
    def chooseTactic(self, data :T) -> Tactic:
        pass

    def preprocessData(self, robotSensorsData :RobotSensorsData) -> T:
        pass

class SimplePreprocessedDataStrategy(Strategy[SimplePreprocessedData]):
    def preprocessData(self, robotSensorsData: RobotSensorsData) -> SimplePreprocessedData:
        return SimplePreprocessedData(robotSensorsData)

class Strategy2(SimplePreprocessedDataStrategy):
    def chooseTactic(self, data :SimplePreprocessedData) -> Tactic:
        edgeLinePosition = data.edgePosition
        if edgeLinePosition != EdgeLinePosition.NO_POSITION :
             return EdgeReflexTurnBackTactic()
        if data.enemyPosition.isDetected():
            return StraightAttack()
        return AdvanceAndPivotTactic()
       
    
    
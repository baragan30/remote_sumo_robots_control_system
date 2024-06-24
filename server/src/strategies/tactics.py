from src.model.edge_line_position import EdgeLinePosition, SimplePreprocesedData
from src.model.robot_sensors_data import RobotControlData
from src.strategies.actions import Action, EdgeDetectionAction

from queue import Queue
from typing import TypeVar, Generic, Optional
from abc import ABC, abstractmethod

# Create a type variable that can be any type
T = TypeVar('T')

# Base class using generics
class Tactic(Generic[T], ABC):
    @abstractmethod
    def run(self, param: T) -> Optional[RobotControlData]:
        """
        This method should be implemented by all subclasses.
        It takes a parameter 'param' of type T.
        """
        pass

class QueueActionTactic(Tactic[SimplePreprocesedData]):
    def __init__(self) -> None:
        self.queue:Queue[Action] = Queue()
        self.hadFirstIteration = False
    
    def firstIteration(self, param: SimplePreprocesedData) -> None:
        # Initialization or setup the actions queue
        pass

    def run(self, param: SimplePreprocesedData) -> Optional[RobotControlData]:
        if not self.hadFirstIteration:
            self.firstIteration(param)
            self.hadFirstIteration = True

        while self.queue.qsize() != 0:
            current_action = self.queue.queue[0]  # Peek at the front of the queue without removing it
            if current_action.isUnfolding(param):
                return current_action.robotControlData
            else:
                self.queue.get_nowait()  # Remove the action since it's no longer unfolding

        return None  # If the queue is empty or all actions have been processed

class EdgeReflexTurnBackTactic(Tactic[SimplePreprocesedData]):
    def run(self, param: SimplePreprocesedData) -> Optional[RobotControlData]:
        edgeData = param.edgePosition
        if edgeData != EdgeLinePosition.NO_POSITION:
            return RobotControlData.from_powers(-100, -100)
        return None

class EdgeReflexTurnBackTactic2(QueueActionTactic):
    def firstIteration(self, param: SimplePreprocesedData) -> None:
        edgeData = param.edgePosition
        if edgeData != EdgeLinePosition.NO_POSITION :
            self.queue.put_nowait(EdgeDetectionAction(RobotControlData.from_powers(-100,-100)))
    
class AdvanceAndPivotTactic(Tactic[SimplePreprocesedData]):
    def run(self, param: SimplePreprocesedData) -> Optional[RobotControlData]:
        edgeData = param.edgePosition
        if edgeData == EdgeLinePosition.NO_POSITION:
            return RobotControlData.from_powers(40, 40)
        return None

import time
from src.model.simple_preprocessed_data import EdgeLinePosition, SimplePreprocessedData
from src.model.robot_sensors_data import RobotControlData
from src.strategies.actions import Action, EdgeDetectionAction, TimeAction

from queue import Queue
from typing import TypeVar, Generic, Optional
from abc import ABC, abstractmethod

class Durations:
    turn360around = 1900  # duration in milliseconds for a full 360-degree turn
    turn180around = turn360around // 2
    turn165around = turn360around * 11 // 24
    turn150around = turn360around // 2
    turn120around = turn360around // 3
    turn90around = turn360around // 4
    turn60around = turn360around // 6
    turn45around = turn360around // 8
    turn30around = turn360around // 12
    pushBackTime = 300
    waitingTime = 300
    forwardTime = 300

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

class QueueActionTactic(Tactic[SimplePreprocessedData]):
    def __init__(self) -> None:
        self.queue:Queue[Action] = Queue()
        self.hadFirstIteration = False
        self.current_action:Optional[Action] = None

    def add(self, action:Action):
        self.queue.put_nowait(action)
    
    def firstIteration(self, param: SimplePreprocessedData) -> None:
        # Initialization or setup the actions queue
        pass

    def run(self, param: SimplePreprocessedData) -> Optional[RobotControlData]:
        if not self.hadFirstIteration:
            self.firstIteration(param)
            self.hadFirstIteration = True

        if self.current_action and self.current_action.isUnfolding(param):
            return self.current_action.robotControlData
        
        while self.queue.qsize() != 0 :
            self.current_action = self.queue.get_nowait()
            if self.current_action and self.current_action.isUnfolding(param):
                return self.current_action.robotControlData

        return None  # If the queue is empty or all actions have been processed

class EdgeReflexTurnBackTactic(QueueActionTactic):
    def firstIteration(self, param: SimplePreprocessedData) -> None:
        edgeData = param.edgePosition
        if edgeData != EdgeLinePosition.NO_POSITION :
            self.add(TimeAction(RobotControlData.backward(), Durations.pushBackTime))
            self.add(EdgeDetectionAction(RobotControlData.backward()))
            actions = {
                EdgeLinePosition.EXTREME_LEFT: lambda: self.add(TimeAction(RobotControlData.right_rotate(), Durations.turn120around)),
                EdgeLinePosition.LEFT: lambda:         self.add(TimeAction(RobotControlData.right_rotate(), Durations.turn150around)),
                EdgeLinePosition.CENTER: lambda:       self.add(TimeAction(RobotControlData.right_rotate(), Durations.turn180around)),
                EdgeLinePosition.RIGHT: lambda:        self.add(TimeAction(RobotControlData.left_rotate(), Durations.turn150around)),
                EdgeLinePosition.EXTREME_RIGHT: lambda: self.add(TimeAction(RobotControlData.left_rotate(), Durations.turn120around))
            }
            # Add the action based on the current edge position
            action = actions.get(edgeData, lambda: None)  # No action for NO_POSITION or undefined
            action()
    
class AdvanceAndPivotTactic(QueueActionTactic):
    def firstIteration(self, param: SimplePreprocessedData) -> None:
        self.add(TimeAction(RobotControlData.forward(), Durations.forwardTime))
        self.add(TimeAction(RobotControlData.stop(), Durations.waitingTime))
        self.add(TimeAction(RobotControlData.left_rotate(), Durations.turn60around))
        self.add(TimeAction(RobotControlData.stop(), Durations.waitingTime))
        self.add(TimeAction(RobotControlData.right_rotate(), Durations.turn90around))
        self.add(TimeAction(RobotControlData.stop(), Durations.waitingTime))
        self.add(TimeAction(RobotControlData.left_rotate(), Durations.turn60around))

    def run(self, param: SimplePreprocessedData) -> Optional[RobotControlData]:
        if param.edgePosition != EdgeLinePosition.NO_POSITION :
            return None
        if param.enemyPosition.isDetected():
            return None
        return super().run(param)

class StraightAttack(Tactic[SimplePreprocessedData]):
    def run(self, param: SimplePreprocessedData) -> Optional[RobotControlData]:
        if param.edgePosition != EdgeLinePosition.NO_POSITION :
            return None
        if not param.enemyPosition.isDetected() :
            return None
        
        pos = param.enemyPosition.angle
        enemyDistance = param.enemyPosition.distance
        # print(f"{pos} - {enemyDistance}")
        engageDistance = 8
        if 0 <= pos < 20:  # Right
            return RobotControlData.right_rotate()
        if 20 <= pos < 60 :
            return RobotControlData.right_slow_rotate()
        elif 60 <= pos < 80:
            if enemyDistance <= engageDistance:
                return RobotControlData.forward_right()
            else:
                return RobotControlData.right_slow_rotate()
        elif 80 <= pos <= 100:  # Forward
            if enemyDistance <= engageDistance :
                return RobotControlData.forward()
            else :
                return RobotControlData.forward_slow()
        elif 100 < pos <= 120:
            if enemyDistance <= engageDistance:
                return RobotControlData.forward_left()
            else:
                return RobotControlData.left_slow_rotate()
        elif 120 < pos <= 160 :
            return RobotControlData.left_slow_rotate()
        elif 160 < pos <= 180:  # Left
            return RobotControlData.left_rotate()
        
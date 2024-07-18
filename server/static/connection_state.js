class CommandType {
  static REGISTER = 0x00;
  static LINK = 0x01;
  static CONNECTION = 0x02;
  static STRATEGY = 0x03;
  static VIDEO_STREAM = 0x04;
  static FRAME = 0x05;
  static MOTOR_POWER = 0x06;
  static DISTANCE_DATA = 0x07;
  static RING_EDGE_DATA = 0x08;
  // Additional methods related to CommandType can be added here
}

class ConnectionState {
  constructor(robotController) {
    this.robotController = robotController;
  }
  start() {
    this.robotController.stopLoop();
    this.robotController.uiController.displayNone();
  }
  onOpenConnection() {
  }
  onLostConnection() {
    this.robotController.setState(ConnectingServerState);
  }
  loop() { }
  onMessage(command, data) { }
  onUserInput(input) {  }
}

class ConnectingServerState extends ConnectionState {
  constructor(robotController) {
    super(robotController);
  }
  onOpenConnection() {
    this.robotController.setState(RegisteringToServerState)
  }
  start() {
    super.start();
    this.robotController.uiController.displayTryConnectingToServer();
    this.robotController.startLoop(5000);
  }
  loop() {
    this.robotController.server.init();
  }
}

class RegisteringToServerState extends ConnectionState {
  constructor(robotController) {
    super(robotController);
  }
  start() {
    super.start();
    this.robotController.uiController.displayTryRegistering();
    this.robotController.startLoop(2500);

  }

  loop() {
    this.robotController.server.send(CommandType.REGISTER);
  }
  onMessage(command, data) {
    if (command == CommandType.REGISTER) {
      this.robotController.setState(ConnectingToRobotState)
    }
  }
}

class ConnectingToRobotState extends ConnectionState {
  constructor(robotController) {
    super(robotController);
  }
  start() {
    super.start();
    this.robotController.uiController.displayTryConnectingToRobot();
    this.robotController.startLoop(3000);
  }
  loop() {
    let uuid = Helper.uuidStringToUint8Array(window.robotId);
    this.robotController.server.send(CommandType.LINK, uuid);
  }
  onMessage(command, data) {
    if (CommandType.LINK) {
      if (data.length == 16) {
        this.robotController.setState(RobotIdleState)
      } else {
        this.robotController.disconnect();
      }
    }
  }
}

class RobotLostConnectionState extends ConnectionState {
  constructor(robotController) {
    super(robotController);
  }
  start() {
    super.start();
    this.robotController.uiController.displayLostConnectionToRobot();
  }
  onMessage(command, data) {
    if (command == CommandType.CONNECTION && data[0] != 0x00) {
      this.robotController.setState(RobotIdleState);
    }
  }
}

class RobotIdleState extends ConnectionState {
  constructor(robotController) {
    super(robotController);
    this.commandHandlers = {
      // [CommandType.REGISTER]: this.handleRegister.bind(this),
      // [CommandType.LINK]: this.handleLink.bind(this),
      [CommandType.CONNECTION]: this.handleConnection.bind(this),
      // [CommandType.VIDEO_STREAM]: this.handleVideoStream.bind(this),
      [CommandType.FRAME]: this.handleFrame.bind(this),
      // [CommandType.MOTOR_POWER]: this.handleMotorPower.bind(this)
      [CommandType.DISTANCE_DATA]: this.handleDistanceData.bind(this),
      [CommandType.RING_EDGE_DATA]: this.handleRingEdgeData.bind(this),
    };
  }
  start() {
    super.start();
    this.robotController.uiController.displayFullConnection();
    if(this.robotController.hasIdleState){
      this.robotController.selectStrategyPad.setStartButton();
      this.robotController.startLoop(250);
    }
    else 
      this.changeState();
  }
  loop(){
    let strategyValue = new Uint8Array([0x00]);
    this.robotController.server.send(CommandType.STRATEGY,strategyValue);
  }
  onMessage(command, data) {
    const handler = this.commandHandlers[command] || this.handleDefault;
    handler(command, data);
  }
  onUserInput(input) {
    if (input == 'w' || input == 's' || input == 'a' || input == 'd') {
      this.robotController.setState(RobotOperationalState);
    }else if (input == 'q') {
      this.handleQUserInput();
    }else if(input == ' '){
      this.robotController.hasIdleState = false;
      this.changeState();
    }
  }

  changeState(){
    let value = this.robotController.selectStrategyPad.getSelectValue();
    if(value == 1){
      this.robotController.setState(RobotFullRemoteControlState);
      return;
    }
    if(value == 2){
      this.robotController.setState(RobotOperationalState);
      return;
     }
    if (value == 3){
      this.robotController.setState(RobotAutonomousState);
      return;
    }
  }

  handleConnection(command, data) {
    if (data[0] == 0) {
      this.robotController.setState(RobotLostConnectionState);
    }
  }

  handleFrame(command, data) {
    if(this.robotController.IsReceivingVideoStream)
      this.robotController.uiController.drawImage(data);
    else 
      this.robotController.server.send(CommandType.VIDEO_STREAM, new Uint8Array([this.robotController.IsReceivingVideoStream]))
  }
  handleDistanceData(command, data) {
    this.robotController.uiController.displayDistanceData(data);
  }
  handleRingEdgeData(command, data) {
    let dataBytes = Helper.byteToBitArray(data[0]);
    this.robotController.uiController.drawRingData(dataBytes);
  }
  handleDefault(command, data) {
    console.log(`Received unrecognized command ${command} with data:`, data);
  }
  handleQUserInput(){
    this.lastPowersTransmissionTime = new Date();
    this.robotController.IsReceivingVideoStream = !this.robotController.IsReceivingVideoStream;
    this.robotController.server.send(CommandType.VIDEO_STREAM, new Uint8Array([this.robotController.IsReceivingVideoStream]))
    if(!this.robotController.IsReceivingVideoStream)
      this.robotController.uiController.canvasController.clearImage();
  }

}

class RobotOperationalState extends RobotIdleState {
  static MAX_POWERS_TRANSMISSION_TIME = 50;
  static MAX_STRATEGY_TRANSMISSION_TIME = 300;

  constructor(robotController) {
    super(robotController);
    this.lastPowersTransmissionTime = new Date();
    this.lastStrategyTransmissionTime = new Date();
  }
  start() {
    this.robotController.uiController.displayText("Remote Control");
    this.robotController.selectStrategyPad.setStopButton();
    this.robotController.selectStrategyPad.setSelectValue(1);
    this.robotController.hasIdleState = false;
    this.robotController.startLoop(RobotOperationalState.MAX_POWERS_TRANSMISSION_TIME);
  }
  loop() {
    //it past 300 ms
    if((new Date()) - this.lastPowersTransmissionTime >= RobotOperationalState.MAX_POWERS_TRANSMISSION_TIME){
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([0,0]));
      this.lastPowersTransmissionTime = new Date();
    }
    if((new Date()) - this.lastStrategyTransmissionTime >= RobotOperationalState.MAX_STRATEGY_TRANSMISSION_TIME){
      let strategyValue = new Uint8Array([0x02]);
      this.robotController.server.send(CommandType.STRATEGY,strategyValue);
      this.lastStrategyTransmissionTime = new Date();
    }
  }

  onUserInput(input) {
    if (input == 'w') {
      this.lastPowersTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([100,100]))
    }else if (input == 's') {
      this.lastPowersTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([-100,-100]))
    }else if (input == 'a') {
      this.lastPowersTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([-100,100]))
    }else if (input == 'd') {
      this.lastPowersTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([100,-100]))
    }else if (input == 'q') {
      this.handleQUserInput();

    }else if(input == ' '){
      this.robotController.hasIdleState = true;
      this.robotController.setState(RobotIdleState);
    }
    // console.log(`User Input ${input}`);
  }
}

class RobotFullRemoteControlState extends RobotOperationalState {
  loop() {
    if((new Date()) - this.lastPowersTransmissionTime >= RobotOperationalState.MAX_POWERS_TRANSMISSION_TIME){
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([0,0]));
      this.lastPowersTransmissionTime = new Date();
    }
    if((new Date()) - this.lastStrategyTransmissionTime >= RobotOperationalState.MAX_STRATEGY_TRANSMISSION_TIME){
      let strategyValue = new Uint8Array([0x01]);
      this.robotController.server.send(CommandType.STRATEGY,strategyValue);
      this.lastStrategyTransmissionTime = new Date();
    }
  }
  start() {
    super.start();
    this.robotController.uiController.displayText("Full Remote Control");
  }

}




class RobotAutonomousState extends RobotIdleState {

  constructor(robotController) {
    super(robotController);
    this.lastPowersTransmissionTime = new Date();
  }
  start() {
    this.robotController.uiController.displayText("Strategy " + (this.robotController.selectStrategyPad.getSelectValue() - 2));
    this.robotController.selectStrategyPad.setStopButton();
    this.robotController.hasIdleState = false;
    this.robotController.startLoop(250);
  }
  loop() {
    let strategyValue = new Uint8Array([this.robotController.selectStrategyPad.getSelectValue()]);
    this.robotController.server.send(CommandType.STRATEGY,strategyValue);
  }

  onUserInput(input) {
    if (input == 'w' || input == 's' || input == 'a' || input == 'd') {
      this.robotController.hasIdleState = false;
      this.robotController.setState(RobotOperationalState);
    }else if (input == 'q') {
      this.handleQUserInput();
    }else if(input == ' '){
      this.robotController.hasIdleState = true;
      this.robotController.setState(RobotIdleState);
    }
  }
}
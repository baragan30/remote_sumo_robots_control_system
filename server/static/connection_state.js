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
    this.IsReceivingVideoStream = true;
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
      this.lastTransmissionTime = new Date();
      this.IsReceivingVideoStream = !this.IsReceivingVideoStream;
      this.robotController.server.send(CommandType.VIDEO_STREAM, new Uint8Array([this.IsReceivingVideoStream]))
    }else if(input == ' '){
      this.robotController.hasIdleState = false;
      this.changeState();
    }
  }

  changeState(){
    let value = this.robotController.selectStrategyPad.getSelectValue();
    if(value == 1){
        this.robotController.setState(RobotOperationalState);
      }else if (value == 2){
        this.robotController.setState(RobotAutonomousState);
      }
  }

  handleConnection(command, data) {
    if (data[0] == 0) {
      this.robotController.setState(RobotLostConnectionState);
    }
  }

  handleConnection(command, data) {
    if (data[0] == 0) {
      this.robotController.setState(RobotLostConnectionState);
    }
  }

  handleFrame(command, data) {
    this.robotController.uiController.drawImage(data);
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
}

class RobotOperationalState extends RobotIdleState {
  static MAX_TRANSMISSION_TIME = 60;

  constructor(robotController) {
    super(robotController);
    this.lastTransmissionTime = new Date();
  }
  start() {
    this.robotController.uiController.displayText("Remote Control");
    this.robotController.selectStrategyPad.setStopButton();
    this.robotController.selectStrategyPad.setSelectValue(1);
    this.robotController.hasIdleState = false;

    let strategyValue = new Uint8Array([0x01]);
    this.robotController.server.send(CommandType.STRATEGY,strategyValue);

    this.robotController.startLoop(RobotOperationalState.MAX_TRANSMISSION_TIME);
  }
  loop() {
    //it past 300 ms
    if((new Date()) - this.lastTransmissionTime >= RobotOperationalState.MAX_TRANSMISSION_TIME){
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([0,0]));
      this.lastTransmissionTime = new Date();
    }
  }

  onUserInput(input) {
    if (input == 'w') {
      this.lastTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([100,100]))
    }else if (input == 's') {
      this.lastTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([-100,-100]))
    }else if (input == 'a') {
      this.lastTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([-100,100]))
    }else if (input == 'd') {
      this.lastTransmissionTime = new Date();
      this.robotController.server.send(CommandType.MOTOR_POWER, new Int8Array([100,-100]))
    }else if (input == 'q') {
      this.lastTransmissionTime = new Date();
      this.IsReceivingVideoStream = !this.IsReceivingVideoStream;
      this.robotController.server.send(CommandType.VIDEO_STREAM, new Uint8Array([this.IsReceivingVideoStream]))
    }else if(input == ' '){
      this.robotController.hasIdleState = true;
      this.robotController.setState(RobotIdleState);
    }
    // console.log(`User Input ${input}`);
  }
}


class RobotAutonomousState extends RobotIdleState {

  constructor(robotController) {
    super(robotController);
    this.lastTransmissionTime = new Date();
  }
  start() {
    
    this.robotController.uiController.displayText("Strategy " + this.robotController.selectStrategyPad.getSelectValue());
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
      this.lastTransmissionTime = new Date();
      this.IsReceivingVideoStream = !this.IsReceivingVideoStream;
      this.robotController.server.send(CommandType.VIDEO_STREAM, new Uint8Array([this.IsReceivingVideoStream]))
    }else if(input == ' '){
      this.robotController.hasIdleState = true;
      this.robotController.setState(RobotIdleState);
    }
  }
}
enum OperationModes {bootup, configuration, operational};

void SetState(OperationModes newState);
OperationModes GetState();

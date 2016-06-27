#include "states.h"

OperationModes CurrentMode = bootup;

void SetState(OperationModes newState) {
  CurrentMode = newState;
}

OperationModes GetState() {
  return CurrentMode;
}


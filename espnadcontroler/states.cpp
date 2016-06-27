#include "states.h"
#include "debug.h"

OperationModes CurrentMode = bootup;

void SetState(OperationModes newState) {
  DEBUGLOG("changing state to: ");
  DEBUGLOG(newState);
  CurrentMode = newState;
}

OperationModes GetState() {
  return CurrentMode;
}


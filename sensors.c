//This is the sensors module of the PT control system.

int sensorLoop(int* run)
{
	if (!initialize()) {
		return 1;
	}
	while (*run) {

	}
	return 0;
}

int initialize() {
	return 0;
}
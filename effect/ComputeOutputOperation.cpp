#include "ComputeOutputOperation.h"

namespace pixelpart {
bool applyComputeOutputOperation(bool v1, bool v2, ComputeOutputOperation op) {
	switch(op) {
		case ComputeOutputOperation::add:
			return v1 || v2;
		case ComputeOutputOperation::multiply:
			return v1 && v2;
		default:
			return v2;
	}
}
}

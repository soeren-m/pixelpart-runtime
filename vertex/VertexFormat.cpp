#include "VertexFormat.h"

namespace pixelpart {
VertexFormat::VertexFormat(const std::vector<VertexAttribute>& attribs, VertexWindingOrder winding) :
	formatAttributes(attribs), formatWindingOrder(winding) {

}
VertexFormat::VertexFormat(std::initializer_list<VertexAttribute> attribs, VertexWindingOrder winding) :
	formatAttributes(attribs), formatWindingOrder(winding) {

}

const std::vector<VertexAttribute>& VertexFormat::attributes() const {
	return formatAttributes;
}

VertexWindingOrder VertexFormat::windingOrder() const {
	return formatWindingOrder;
}
}

#pragma once

#include "VertexAttribute.h"
#include "VertexWindingOrder.h"
#include <vector>
#include <initializer_list>

namespace pixelpart {
class VertexFormat {
public:
	VertexFormat() = default;
	VertexFormat(const std::vector<VertexAttribute>& attribs, VertexWindingOrder winding);
	VertexFormat(std::initializer_list<VertexAttribute> attribs, VertexWindingOrder winding);

	const std::vector<VertexAttribute>& attributes() const;

	VertexWindingOrder windingOrder() const;

private:
	std::vector<VertexAttribute> formatAttributes;
	VertexWindingOrder formatWindingOrder = VertexWindingOrder::cw;
};
}

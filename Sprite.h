#pragma once

#include "Node.h"
#include "Curve.h"
#include "ColorMode.h"
#include "BlendMode.h"
#include "ImageReference.h"
#include "ImageAnimation.h"

namespace pixelpart {
struct Sprite : public Node {
	Curve<vec2d> motionPath = Curve<vec2d>(0.0, vec2d(0.0));
	Curve<floatd> width = Curve<floatd>(0.5);
	Curve<floatd> height = Curve<floatd>(0.5);
	Curve<floatd> orientation = Curve<floatd>(0.0);
	bool alignWithPath = false;

	ImageReference image;
	ImageAnimation imageAnimation;
	Curve<vec4d> color = Curve<vec4d>(vec4d(1.0));
	Curve<floatd> opacity = Curve<floatd>(1.0);
	ColorMode colorMode = ColorMode::multiply;
	BlendMode blendMode = BlendMode::normal;
	uint32_t layer = 0;
	bool visible = true;
};

void to_json(nlohmann::ordered_json& j, const Sprite& sprite);
void from_json(const nlohmann::ordered_json& j, Sprite& sprite);
}
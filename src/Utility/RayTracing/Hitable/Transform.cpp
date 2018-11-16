#include <RayTracing/Transform.h>
#include <Utility/Math.h>
#include <bitset>

using namespace RayTracing;
using namespace CppUtility::Other;
using namespace glm;
using namespace std;

Transform::Transform(const mat4 & transform, const Hitable::CPtr & hitable, const Material::CPtr & material)
	: transform(transform), hitable(hitable), Hitable(material) {
	if (hitable == NULL) {
		box = AABB::InValid;
		return;
	}

	auto childBox = hitable->GetBoundingBox();
	if (!childBox.IsValid()) {
		box = AABB::InValid;
		return;
	}

	vec3 srcP[2] = { childBox.GetMinP(), childBox.GetMaxP() };
	vector<vec3> tfmCornerPs;
	for (size_t i = 0; i < 8; i++) {
		bitset<3> binVal(i);
		vec3 cornerP = vec3(srcP[binVal[2]].x, srcP[binVal[1]].y, srcP[binVal[0]].z);
		vec4 tfmCornerPQ = transform * vec4(cornerP, 1);
		tfmCornerPs.push_back(vec3(tfmCornerPQ) / tfmCornerPQ.w);
	}

	vec3 minP = Math::min(tfmCornerPs);
	vec3 maxP = Math::max(tfmCornerPs);
	box = AABB(minP, maxP);
}

HitRst Transform::RayIn(Ray::Ptr & ray) const {
	if (hitable == NULL)
		return HitRst::FALSE;

	ray->Transform(inverse(transform));

	auto hitRst = hitable->RayIn(ray);

	if (hitRst.hit) {
		if (hitRst.isMatCoverable && material != NULL) {
			hitRst.material = material;
			hitRst.isMatCoverable = isMatCoverable;
		}

		hitRst.record.vertex.Transform(transform);
	}

	ray->Transform(transform);

	return hitRst;
}

AABB Transform::GetBoundingBox() const {
	if (!box.IsValid()) {
		printf("hehe\n");
	}
	return box;
}
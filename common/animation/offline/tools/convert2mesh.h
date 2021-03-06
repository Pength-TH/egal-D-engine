
#include "common/animation/offline/mesh.h"
#include "common/animation/offline/fbx/fbx.h"
#include "common/animation/offline/fbx/fbx_base.h"
#include "common/animation/skeleton.h"
#include "common/animation/maths/math_ex.h"
#include "common/animation/maths/simd_math.h"
#include "common/animation/maths/vec_float.h"
#include <algorithm>
#include <limits>
#include <vector>

#include "fbxsdk/utils/fbxgeometryconverter.h"

#include "common/animation/offline/tools/convert2material.h"

#include "common/egal_string.h"

namespace egal
{
	// Control point to vertex buffer remapping.
	typedef std::vector<uint16_t>			 ControlPointRemap;
	typedef std::vector<ControlPointRemap>	 ControlPointsRemap;
	typedef std::vector<std::vector<size_t>> BuckedVertices;

	// Triangle indices naive sort function.
	int SortTriangles(const void* _left, const void* _right)
	{
		const uint16_t* left = static_cast<const uint16_t*>(_left);
		const uint16_t* right = static_cast<const uint16_t*>(_right);
		return (left[0] + left[1] + left[2]) - (right[0] + right[1] + right[2]);
	}

	// Generic function that gets an element from a layer.
	template <typename _Element>
	bool GetElement(const _Element& _layer, int _vertex_id, int _control_point,
		typename _Element::ArrayElementType* _out)
	{
		assert(_out);

		int direct_array_id;
		switch (_layer.GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
		{
			switch (_layer.GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				direct_array_id = _control_point;
				break;
			}
			case FbxGeometryElement::eIndexToDirect:
			{
				direct_array_id = _layer.GetIndexArray().GetAt(_control_point);
				break;
			}
			default:
				return false;  // Unhandled reference mode.
			}
			break;
		}
		case FbxGeometryElement::eByPolygonVertex:
		{
			switch (_layer.GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
			{
				direct_array_id = _vertex_id;
				break;
			}
			case FbxGeometryElement::eIndexToDirect:
			{
				direct_array_id = _layer.GetIndexArray().GetAt(_vertex_id);
				break;
			}
			default:
				return false;  // Unhandled reference mode.
			}
			break;
		}
		default:
			return false;  // Unhandled mapping mode.
		}

		// Extract data from the layer direct array.
		*_out = _layer.GetDirectArray().GetAt(direct_array_id);

		return true;
	}

	// Compare raw elements. Returns true if all elements from _a and _b are equals.
	template <typename _T>
	bool Compare(const _T* _a, const _T* _b, size_t _count)
	{
		size_t i = 0;
		for (; i < _count && _a[i] == _b[i]; ++i)
			;
		return i == _count;
	}

	bool BuildVertices(FbxMesh* _fbx_mesh,
		egal::animation::offline::fbx::FbxSystemConverter* _converter,
		ControlPointsRemap* _remap,
		import_mesh* _output_mesh)
	{
		// This function treat all layers like if they were using mapping mode
		// eByPolygonVertex. This allow to use a single code path for all mapping
		// modes. It requires one more pass (compare to eByControlPoint mode), which
		// is to weld vertices with identical positions, normals, uvs...

		// Allocates control point to polygon remapping.
		const int ctrl_point_count = _fbx_mesh->GetControlPointsCount();
		_remap->resize(ctrl_point_count);

		// Regenerate normals if they're not available.
		if (!_fbx_mesh->GenerateNormals(false,     // overwrite
			true,      // by ctrl point
			false))
		{  // clockwise
			return false;
		}

		assert(_fbx_mesh->GetElementNormalCount() > 0);
		const FbxGeometryElementNormal* element_normals =
			_fbx_mesh->GetElementNormal(0);
		assert(element_normals);

		// Checks uvs availability.
		const FbxGeometryElementUV* element_uvs = NULL;
		if (_fbx_mesh->GetElementUVCount() > 0)
		{
			element_uvs = _fbx_mesh->GetElementUV(0);
		}

		// Checks tangents availability.
		const FbxGeometryElementTangent* element_tangents = NULL;
		if (element_uvs)
		{  // UVs are needed to generate tangents.
			  // Regenerate tagents if they're not available.
			if (!_fbx_mesh->GenerateTangentsData(0, false))
			{
				return false;
			}
		}
		if (_fbx_mesh->GetElementTangentCount() > 0)
		{
			element_tangents = _fbx_mesh->GetElementTangent(0);
		}

		// Checks vertex colors availability.
		const FbxGeometryElementVertexColor* element_colors = NULL;
		if (_fbx_mesh->GetElementVertexColorCount() > 0)
		{
			element_colors = _fbx_mesh->GetElementVertexColor(0);
		}

		// Computes worst vertex count case. Needs to allocate 3 vertices per polygon,
		// as they should all be triangles.
		const int polygon_count = _fbx_mesh->GetPolygonCount();
		int vertex_count = _fbx_mesh->GetPolygonCount() * 3;

		// Reserve vertex buffers. Real size is unknown as redundant vertices will be
		// rejected.
		import_mesh::Part& part = _output_mesh->parts[0];
		part.positions.reserve(vertex_count *
			import_mesh::Part::kPositionsCpnts);
		part.normals.reserve(vertex_count * import_mesh::Part::kNormalsCpnts);
		if (element_tangents)
		{
			part.tangents.reserve(vertex_count *
				import_mesh::Part::kTangentsCpnts);
		}
		if (element_uvs)
		{
			part.uvs.reserve(vertex_count * import_mesh::Part::kUVsCpnts);
		}
		if (element_colors)
		{
			part.colors.reserve(vertex_count * import_mesh::Part::kColorsCpnts);
		}

		// Resize triangle indices, as their size is known.
		_output_mesh->triangle_indices.resize(vertex_count);

		// Iterate all polygons and stores ctrl point to polygon mappings.
		int vertex_id = 0;
		for (int p = 0; p < polygon_count; ++p)
		{
			assert(_fbx_mesh->GetPolygonSize(p) == 3 &&
				"Mesh must have been triangulated.");

			for (int v = 0; v < 3; ++v, ++vertex_id)
			{
				// Get control point.
				const int ctrl_point = _fbx_mesh->GetPolygonVertex(p, v);
				assert(ctrl_point >= 0);
				ControlPointRemap& remap = _remap->at(ctrl_point);

				// Get vertex position.
				const egal::math::Float3 position =
					_converter->ConvertPoint(_fbx_mesh->GetControlPoints()[ctrl_point]);

				// Get vertex normal.
				FbxVector4 src_normal(0.f, 1.f, 0.f, 0.f);
				if (!GetElement(*element_normals, vertex_id, ctrl_point, &src_normal))
				{
					return false;
				}
				const egal::math::Float3 normal = NormalizeSafe(
					_converter->ConvertNormal(src_normal), egal::math::Float3::y_axis());

				// Get vertex tangent.
				FbxVector4 src_tangent(1.f, 0.f, 0.f, 0.f);
				if (element_tangents)
				{
					if (!GetElement(*element_tangents, vertex_id, ctrl_point,
						&src_tangent))
					{
						return false;
					}
				}
				const egal::math::Float3 tangent3 = NormalizeSafe(
					_converter->ConvertNormal(src_tangent), egal::math::Float3::x_axis());
				const egal::math::Float4 tangent(tangent3,
					static_cast<float>(src_tangent[3]));

				// Get vertex uv.
				FbxVector2 src_uv;
				if (element_uvs)
				{
					if (!GetElement(*element_uvs, vertex_id, ctrl_point, &src_uv))
					{
						return false;
					}
				}
				else
				{
					src_uv = FbxVector2(0., 0.);
				}
				const egal::math::Float2 uv(static_cast<float>(src_uv[0]),
					static_cast<float>(src_uv[1]));

				// Get vertex colors.
				FbxColor src_color;
				if (element_colors)
				{
					if (!GetElement(*element_colors, vertex_id, ctrl_point, &src_color))
					{
						return false;
					}
				}
				else
				{
					src_color = FbxColor(1., 1., 1., 1.);
				}
				const uint8_t color[4] =
				{
								static_cast<uint8_t>(
									egal::math::Clamp(0., src_color.mRed * 255., 255.)),
								static_cast<uint8_t>(
									egal::math::Clamp(0., src_color.mGreen * 255., 255.)),
								static_cast<uint8_t>(
									egal::math::Clamp(0., src_color.mBlue * 255., 255.)),
								static_cast<uint8_t>(
									egal::math::Clamp(0., src_color.mAlpha * 255., 255.)),
				};

				// Check for vertex redundancy, only with other points that share the same
				// control point.
				int redundant_with = -1;
				for (size_t r = 0; r < remap.size(); ++r)
				{
					const int to_test = remap[r];

					// Check for identical normals.
					if (!Compare(
						&normal.x,
						&part.normals[to_test * import_mesh::Part::kNormalsCpnts],
						import_mesh::Part::kNormalsCpnts))
					{
						continue;  // Next vertex.
					}

					// Check for identical uvs.
					if (element_uvs)
					{
						if (!Compare(&uv.x,
							&part.uvs[to_test * import_mesh::Part::kUVsCpnts],
							import_mesh::Part::kUVsCpnts))
						{
							continue;  // Next vertex.
						}
					}

					// Check for identical colors.
					if (element_colors)
					{
						if (!Compare(
							color,
							&part.colors[to_test * import_mesh::Part::kColorsCpnts],
							import_mesh::Part::kColorsCpnts))
						{
							continue;  // Next vertex.
						}
					}

					// Check for identical tangents.
					if (element_tangents)
					{
						if (!Compare(&tangent.x,
							&part.tangents[to_test *
							import_mesh::Part::kTangentsCpnts],
							import_mesh::Part::kColorsCpnts))
						{
							continue;  // Next vertex.
						}
					}

					// This vertex is redundant with an existing one.
					redundant_with = to_test;
					break;
				}

				if (redundant_with >= 0)
				{
					assert(redundant_with <= (std::numeric_limits<uint16_t>::max)());

					// Reuse existing vertex.
					_output_mesh->triangle_indices[p * 3 + v] =
						static_cast<uint16_t>(redundant_with);
				}
				else
				{
					// Detect triangle indices overflow.
					if ((part.positions.size() / 3) >
						(std::numeric_limits<uint16_t>::max)())
					{
						log_error("Mesh uses too many vertices (> %d) to fit in the index buffer.", (std::numeric_limits<uint16_t>::max)());
						return false;
					}

					// Deduce this vertex offset in the output vertex buffer.
					uint16_t vertex_index = static_cast<uint16_t>(
						part.positions.size() / import_mesh::Part::kPositionsCpnts);

					// Build triangle indices.
					_output_mesh->triangle_indices[p * 3 + v] = vertex_index;

					// Stores vertex offset in the output vertex buffer.
					_remap->at(ctrl_point).push_back(vertex_index);

					// Push vertex data.
					part.positions.push_back(position.x);
					part.positions.push_back(position.y);
					part.positions.push_back(position.z);
					part.normals.push_back(normal.x);
					part.normals.push_back(normal.y);
					part.normals.push_back(normal.z);
					if (element_uvs)
					{
						part.uvs.push_back(uv.x);
						part.uvs.push_back(uv.y);
					}
					if (element_tangents)
					{
						part.tangents.push_back(tangent.x);
						part.tangents.push_back(tangent.y);
						part.tangents.push_back(tangent.z);
						part.tangents.push_back(tangent.w);
					}
					if (element_colors)
					{
						part.colors.push_back(color[0]);
						part.colors.push_back(color[1]);
						part.colors.push_back(color[2]);
						part.colors.push_back(color[3]);
					}
				}
			}
		}

		// Sorts triangle indices to optimize vertex cache.
		std::qsort(&_output_mesh->triangle_indices[0],
			_output_mesh->triangle_indices.size() / 3, sizeof(uint16_t) * 3,
			&SortTriangles);

		return true;
	}

	// Define a per vertex skin attributes mapping.
	struct SkinMapping
	{
		uint16_t index;
		float weight;
	};

	typedef std::vector<SkinMapping> SkinMappings;
	typedef std::vector<SkinMappings> VertexSkinMappings;

	// Sort highest weight first.
	bool SortInfluenceWeights(const SkinMapping& _left, const SkinMapping& _right)
	{
		return _left.weight > _right.weight;
	}

	bool BuildSkin(FbxMesh* _fbx_mesh,
		egal::animation::offline::fbx::FbxSystemConverter* _converter,
		const ControlPointsRemap& _remap,
		const egal::animation::Skeleton& _skeleton,
		import_mesh* _output_mesh)
	{
		assert(_output_mesh->parts.size() == 1 &&
			_output_mesh->parts[0].vertex_count() != 0);
		import_mesh::Part& part = _output_mesh->parts[0];

		const int skin_count = _fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
		if (skin_count == 0)
		{
			log_error("No skin found.");
			return false;
		}
		if (skin_count > 1)
		{
			log_info("More than one skin found, only the first one will be processed.");
		}

		// Get skinning indices and weights.
		FbxSkin* deformer =
			static_cast<FbxSkin*>(_fbx_mesh->GetDeformer(0, FbxDeformer::eSkin));
		FbxSkin::EType skinning_type = deformer->GetSkinningType();
		if (skinning_type != FbxSkin::eRigid && skinning_type != FbxSkin::eLinear)
		{
			log_error("Unsupported skinning type");
			return false;
		}

		// Builds joints names map.
		typedef std::map<const e_char* ,uint16_t> JointsMap;
		JointsMap joints_map;
		for (int i = 0; i < _skeleton.num_joints(); ++i)
		{
			joints_map[_skeleton.joint_names()[i]] = static_cast<uint16_t>(i);
		}

		// Resize inverse bind pose matrices and set all to identity.
		_output_mesh->inverse_bind_poses.resize(_skeleton.num_joints());
		for (int i = 0; i < _skeleton.num_joints(); ++i)
		{
			_output_mesh->inverse_bind_poses[i] = egal::math::Float4x4::identity();
		}

		// Resize to the number of vertices
		const size_t vertex_count = part.vertex_count();
		VertexSkinMappings vertex_skin_mappings;
		vertex_skin_mappings.resize(vertex_count);

		// Computes geometry matrix.
		const FbxAMatrix geometry_matrix(
			_fbx_mesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot),
			_fbx_mesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot),
			_fbx_mesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot));

		const int cluster_count = deformer->GetClusterCount();
		for (int cl = 0; cl < cluster_count; ++cl)
		{
			const FbxCluster* cluster = deformer->GetCluster(cl);
			const FbxNode* node = cluster->GetLink();
			if (!node)
			{
				log_info("No node linked to cluster %s.", cluster->GetName());
				continue;
			}

			const FbxCluster::ELinkMode mode = cluster->GetLinkMode();
			if (mode != FbxCluster::eNormalize)
			{
				log_error("Unsupported link mode for joint %s.", node->GetName());
				return false;
			}

			// Get corresponding joint index;
			JointsMap::const_iterator it = joints_map.find(node->GetName());
			if (it == joints_map.end())
			{
				log_error("Required joint %s not found in provided skeleton.", node->GetName());
				return false;
			}
			const uint16_t joint = it->second;
			// Computes joint's inverse bind-pose matrix.
			FbxAMatrix transform_matrix;
			cluster->GetTransformMatrix(transform_matrix);
			transform_matrix *= geometry_matrix;

			FbxAMatrix transform_link_matrix;
			cluster->GetTransformLinkMatrix(transform_link_matrix);

			const FbxAMatrix inverse_bind_pose =
				transform_link_matrix.Inverse() * transform_matrix;

			// Stores inverse transformation.
			_output_mesh->inverse_bind_poses[joint] =
				_converter->ConvertMatrix(inverse_bind_pose);

			// Affect joint to all vertices of the cluster.
			const int ctrl_point_index_count = cluster->GetControlPointIndicesCount();

			const int* ctrl_point_indices = cluster->GetControlPointIndices();
			const double* ctrl_point_weights = cluster->GetControlPointWeights();
			for (int cpi = 0; cpi < ctrl_point_index_count; ++cpi)
			{
				const SkinMapping mapping = { joint,static_cast<float>(ctrl_point_weights[cpi]) };

				// Sometimes, the mesh can have less points than at the time of the
				// skinning because a smooth operator was active when skinning but has
				// been deactivated during export.
				const int ctrl_point = ctrl_point_indices[cpi];
				if (ctrl_point < _fbx_mesh->GetControlPointsCount() &&
					mapping.weight > 0.f)
				{
					const ControlPointRemap& remap = _remap[ctrl_point];
					assert(remap.size() >= 1);  // At least a 1-1 mapping.
					for (size_t v = 0; v < remap.size(); ++v)
					{
						vertex_skin_mappings[remap[v]].push_back(mapping);
					}
				}
			}
		}

		// Sort joint indexes according to weights.
		// Also deduce max number of indices per vertex.
		size_t max_influences = 0;
		for (size_t i = 0; i < vertex_count; ++i)
		{
			SkinMappings& inv = vertex_skin_mappings.at(i);

			// Updates max_influences.
			max_influences = egal::math::_Max(max_influences, (size_t)inv.size());

			// Normalize weights.
			float sum = 0.f;
			for (size_t j = 0; j < inv.size(); ++j)
			{
				sum += inv[j].weight;
			}
			const float inv_sum = 1.f / (sum != 0.f ? sum : 1.f);
			for (size_t j = 0; j < inv.size(); ++j)
			{
				inv[j].weight *= inv_sum;
			}

			// Sort weights, bigger ones first, so that lowest one can be filtered out.
			std::sort(inv.begin(), inv.end(), &SortInfluenceWeights);
		}

		// Allocates indices and weights.
		part.joint_indices.resize(vertex_count * max_influences);
		part.joint_weights.resize(vertex_count * max_influences);

		// Build output vertices data.
		bool vertex_isnt_influenced = false;
		for (size_t i = 0; i < vertex_count; ++i)
		{
			SkinMappings& inv = vertex_skin_mappings.at(i);
			uint16_t* indices = &part.joint_indices[i * max_influences];
			float* weights = &part.joint_weights[i * max_influences];

			// Stores joint's indices and weights.
			size_t influence_count = inv.size();
			if (influence_count > 0)
			{
				size_t j = 0;
				for (; j < influence_count; ++j)
				{
					indices[j] = inv[j].index;
					weights[j] = inv[j].weight;
				}
			}
			else
			{
				// No joint influencing this vertex.
				vertex_isnt_influenced = true;
			}

			// Set unused indices and weights.
			for (size_t j = influence_count; j < max_influences; ++j)
			{
				indices[j] = 0;
				weights[j] = 0.f;
			}
		}

		if (vertex_isnt_influenced)
		{
			log_error("At least one vertex isn't influenced by any joints.");
		}

		return !vertex_isnt_influenced;
	}

	// Limits the number of joints influencing a vertex.
	bool LimitInfluences(import_mesh& _skinned_mesh, int _limit)
	{
		assert(_skinned_mesh.parts.size() == 1);

		import_mesh::Part& in_part = _skinned_mesh.parts.front();

		// Check if it's actualluy needed to limit the number of influences.
		const int max_influences = in_part.influences_count();
		assert(max_influences > 0);
		if (max_influences <= _limit)
		{
			return true;
		}

		// Iterate all vertices to remove unwanted weights and renormalize.
		// Note that weights are already sorted, so the last ones are the less
		// influencing.
		const size_t vertex_count = in_part.vertex_count();
		for (size_t i = 0, offset = 0; i < vertex_count; ++i, offset += _limit)
		{
			// Remove exceeding influences
			for (int j = 0; j < _limit; ++j)
			{
				in_part.joint_indices[offset + j] =
					in_part.joint_indices[i * max_influences + j];
				in_part.joint_weights[offset + j] =
					in_part.joint_weights[i * max_influences + j];
			}
			// Renormalize weights.
			float sum = 0.f;
			for (int j = 0; j < _limit; ++j)
			{
				sum += in_part.joint_weights[offset + j];
			}
			for (int j = 0; j < _limit; ++j)
			{
				in_part.joint_weights[offset + j] *= 1.f / sum;
			}
		}

		// Resizes data
		in_part.joint_indices.resize(vertex_count * _limit);
		in_part.joint_weights.resize(vertex_count * _limit);
		return true;
	}

	// Split the skinned mesh into parts. For each part, all vertices has the same
	// number of influencing joints.
	bool SplitParts(const import_mesh& _skinned_mesh,
		import_mesh* _partitionned_mesh)
	{
		assert(_skinned_mesh.parts.size() == 1);
		assert(_partitionned_mesh->parts.size() == 0);

		const import_mesh::Part& in_part = _skinned_mesh.parts.front();
		const size_t vertex_count = in_part.vertex_count();

		// Creates one mesh part per influence.
		const int max_influences = in_part.influences_count();
		assert(max_influences > 0);

		// Bucket-sort vertices per influence count.
		BuckedVertices bucked_vertices;
		bucked_vertices.resize(max_influences);
		if (max_influences > 1)
		{
			for (size_t i = 0; i < vertex_count; ++i)
			{
				const float* weights = &in_part.joint_weights[i * max_influences];
				int j = 0;
				for (; j < max_influences && weights[j] > 0.f; ++j)
				{
				}
				const int influences = j - 1;
				bucked_vertices[influences].push_back(i);
			}
		}
		else
		{
			for (size_t i = 0; i < vertex_count; ++i)
			{
				bucked_vertices[0].push_back(i);
			}
		}

		// Group vertices if there's not enough of them for a given part. This allows
		// to
		// limit SkinningJob fix cost overhead.
		const size_t kMinBucketSize = 32;

		for (size_t i = 0; i < bucked_vertices.size() - 1; ++i)
		{
			std::vector<size_t>& bucket = bucked_vertices[i];
			if (bucket.size() < kMinBucketSize)
			{
				// Transfers vertices to next bucket if there aren't enough.
				std::vector<size_t>& next_bucket = bucked_vertices[i + 1];
				next_bucket.reserve(next_bucket.size() + bucket.size());
				for (size_t j = 0; j < bucket.size(); ++j)
				{
					next_bucket.push_back(bucket[j]);
				}
				bucket.clear();
			}
		}

		// Fills mesh parts.
		_partitionned_mesh->parts.reserve(max_influences);
		for (int i = 0; i < max_influences; ++i)
		{
			const std::vector<size_t>& bucket = bucked_vertices[i];
			const size_t bucket_vertex_count = bucket.size();
			if (bucket_vertex_count == 0)
			{
				// No Mesh part if no vertices.
				continue;
			}

			// Adds a new part.
			_partitionned_mesh->parts.resize(_partitionned_mesh->parts.size() + 1);
			import_mesh::Part& out_part = _partitionned_mesh->parts.back();

			// Resize output part.
			const int influences = i + 1;
			out_part.positions.resize(bucket_vertex_count *
				import_mesh::Part::kPositionsCpnts);
			out_part.normals.resize(bucket_vertex_count *
				import_mesh::Part::kNormalsCpnts);
			if (in_part.uvs.size())
			{
				out_part.uvs.resize(bucket_vertex_count *
					import_mesh::Part::kUVsCpnts);
			}
			if (in_part.colors.size())
			{
				out_part.colors.resize(bucket_vertex_count *
					import_mesh::Part::kColorsCpnts);
			}
			if (in_part.tangents.size())
			{
				out_part.tangents.resize(bucket_vertex_count *
					import_mesh::Part::kTangentsCpnts);
			}
			out_part.joint_indices.resize(bucket_vertex_count * influences);
			out_part.joint_weights.resize(bucket_vertex_count * influences);

			// Fills output of this part.
			for (size_t j = 0; j < bucket_vertex_count; ++j)
			{
				const size_t bucket_vertex_index = bucket[j];

				// Fills positions.
				float* out_pos =
					&out_part.positions[j * import_mesh::Part::kPositionsCpnts];
				const float* in_pos =
					&in_part.positions[bucket_vertex_index *
					import_mesh::Part::kPositionsCpnts];
				out_pos[0] = in_pos[0];
				out_pos[1] = in_pos[1];
				out_pos[2] = in_pos[2];

				// Fills normals.
				float* out_normal =
					&out_part.normals[j * import_mesh::Part::kNormalsCpnts];
				const float* in_normal =
					&in_part.normals[bucket_vertex_index *
					import_mesh::Part::kNormalsCpnts];
				out_normal[0] = in_normal[0];
				out_normal[1] = in_normal[1];
				out_normal[2] = in_normal[2];

				// Fills uvs.
				if (in_part.uvs.size())
				{
					float* out_uv = &out_part.uvs[j * import_mesh::Part::kUVsCpnts];
					const float* in_uv =
						&in_part
						.uvs[bucket_vertex_index * import_mesh::Part::kUVsCpnts];
					out_uv[0] = in_uv[0];
					out_uv[1] = in_uv[1];
				}
				// Fills colors.
				if (in_part.colors.size())
				{
					uint8_t* out_color =
						&out_part.colors[j * import_mesh::Part::kColorsCpnts];
					const uint8_t* in_color =
						&in_part.colors[bucket_vertex_index *
						import_mesh::Part::kColorsCpnts];
					out_color[0] = in_color[0];
					out_color[1] = in_color[1];
					out_color[2] = in_color[2];
					out_color[3] = in_color[3];
				}
				// Fills tangents.
				if (in_part.tangents.size())
				{
					float* out_tangent =
						&out_part.tangents[j * import_mesh::Part::kTangentsCpnts];
					const float* in_tangent =
						&in_part.tangents[bucket_vertex_index *
						import_mesh::Part::kTangentsCpnts];
					out_tangent[0] = in_tangent[0];
					out_tangent[1] = in_tangent[1];
					out_tangent[2] = in_tangent[2];
					out_tangent[3] = in_tangent[3];
				}

				// Fills joints indices.
				const uint16_t* in_indices =
					&in_part.joint_indices[bucket_vertex_index * max_influences];
				uint16_t* out_indices = &out_part.joint_indices[j * influences];
				for (int k = 0; k < influences; ++k)
				{
					out_indices[k] = in_indices[k];
				}

				// Fills weights. Note that there's no weight if there's only one joint
				// influencing a vertex.
				if (influences > 1)
				{
					const float* in_weights =
						&in_part.joint_weights[bucket_vertex_index * max_influences];
					float* out_weights = &out_part.joint_weights[j * influences];
					for (int k = 0; k < influences; ++k)
					{
						out_weights[k] = in_weights[k];
					}
				}
			}
		}

		// Builds a vertex remapping table to help rebuild triangle indices.
		std::vector<uint16_t> vertices_remap;
		vertices_remap.resize(vertex_count);
		uint16_t processed_vertices = 0;
		for (size_t i = 0; i < bucked_vertices.size(); ++i)
		{
			const std::vector<size_t>& bucket = bucked_vertices[i];
			const uint16_t bucket_vertex_count = static_cast<uint16_t>(bucket.size());
			for (uint16_t j = 0; j < bucket_vertex_count; ++j)
			{
				vertices_remap[bucket[j]] = j + processed_vertices;
			}
			processed_vertices += bucket_vertex_count;
		}

		// Remaps triangle indices, using vertex mapping table.
		const size_t index_count = _skinned_mesh.triangle_indices.size();
		_partitionned_mesh->triangle_indices.resize(index_count);
		for (size_t i = 0; i < index_count; ++i)
		{
			_partitionned_mesh->triangle_indices[i] =
				vertices_remap[_skinned_mesh.triangle_indices[i]];
		}

		// Copy bind pose matrices
		_partitionned_mesh->inverse_bind_poses = _skinned_mesh.inverse_bind_poses;

		return true;
	}

	// Removes the less significant weight, which is recomputed at runtime (sum of
	// weights equals 1).
	bool StripWeights(import_mesh* _mesh)
	{
		for (size_t i = 0; i < _mesh->parts.size(); ++i)
		{
			import_mesh::Part& part = _mesh->parts[i];
			const int influence_count = part.influences_count();
			const int vertex_count = part.vertex_count();
			if (influence_count <= 1)
			{
				part.joint_weights.clear();
			}
			else
			{
				const std::vector<float>& copy = part.joint_weights;
				part.joint_weights.clear();
				part.joint_weights.reserve(vertex_count * (influence_count - 1));

				for (int j = 0; j < vertex_count; ++j)
				{
					for (int k = 0; k < influence_count - 1; ++k)
					{
						part.joint_weights.push_back(copy[j * influence_count + k]);
					}
				}
			}
			assert(static_cast<int>(part.joint_weights.size()) ==
				vertex_count * (influence_count - 1));
		}

		return true;
	}

	//export mesh
	bool convert2mesh(import_mesh& output_mesh, FbxNode* pNode, animation::offline::fbx::FbxSceneLoader& scene_loader, animation::Skeleton* skeleton, ImportOption &option)
	{
		FbxMesh* mesh = (FbxMesh*)pNode->GetNodeAttribute();
		
		StringUnitl::copyString(output_mesh.m_name, TlengthOf(output_mesh.m_name), (char *)pNode->GetName());

		if (!mesh)
		{
			log_error("No mesh to process in this file: %s.", option.file_path_name);
			return false;
		}

		mesh->RemoveBadPolygons();

		log_info("Reading vertices.");
		ControlPointsRemap remap;
		if (!BuildVertices(mesh, scene_loader.converter(), &remap, &output_mesh))
		{
			log_error("Failed to read vertices.");
			return false;
		}

		// Finds skinning informations
		if (mesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
		{
			log_info("Reading skinning data.");
			if (skeleton)
			{
				if (!BuildSkin(mesh, scene_loader.converter(), remap, *skeleton, &output_mesh))
				{
					log_error("Failed to read skinning data.");
					return false;
				}
			}

			// Limiting number of joint influences per vertex.
			if (option.m_influences > 0)
			{
				log_info("Limiting number of joint influences per vertex to %d.", option.m_influences);
				import_mesh partitioned_meshes;
				if (!LimitInfluences(output_mesh, option.m_influences))
				{
					log_error("Failed to limit number of joint influences.");
					return false;
				}
			}

			// Split the mesh if option is true (default)
			if (option.m_split)
			{
				log_info("Partitioning meshes.");
				import_mesh partitioned_meshes;
				if (!SplitParts(output_mesh, &partitioned_meshes))
				{
					log_error("Failed to partitioned meshes.");
					return false;
				}

				// Copy partitioned mesh back to the output.
				output_mesh = partitioned_meshes;
			}

			log_info("Stripping skinning weights.");
			if (!StripWeights(&output_mesh))
			{
				log_error("Failed to strip weights.");
				return false;
			}
			assert(option.m_influences <= 0 || output_mesh.max_influences_count() <= option.m_influences);
		}

		LoadMaterial(mesh, output_mesh.m_material);

		option.m_materials.push_back(output_mesh.m_material);
		option.m_meshs.push_back(output_mesh);
	}
}

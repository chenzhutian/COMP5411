void Mesh::computeVertexNormals() {
	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*
	/* Compute per-vertex normal using neighboring
	/* facet information. (Hint: remember using a
	/* weighting scheme. Plus, do you notice any
	/* disadvantages of your weighting scheme?)
	/**********************************************/

	for (Vertex* vert : mVertexList) {
		Eigen::Vector3f vertNormal = Eigen::Vector3f::Zero();

		std::vector< HEdge* > adjHEdgeList;
		OneRingHEdge orhe(vert);
		HEdge* curr = nullptr;
		while (curr = orhe.nextHEdge()) {
			adjHEdgeList.push_back(curr);
		}

		if (adjHEdgeList.size() >= 2) {
			const Eigen::Vector3f& p0 = vert->position();
			// Compute normals and areas of adjacent triangles.
			std::vector< Eigen::Vector3f > adjNormals;
			std::vector< double > adjAreas;
			double areaSum = 0;
			for (HEdge* adjHEdge : adjHEdgeList) {
				if (!adjHEdge->isBoundary()) {
					const Eigen::Vector3f& p1 = adjHEdge->end()->position();
					const Eigen::Vector3f& p2 = adjHEdge->next()->end()->position();

					adjNormals.push_back(triangleNormal(p0, p1, p2));

					double area = triangleArea(p0, p1, p2);
					adjAreas.push_back(area);
					areaSum += area;
				}
			}
			for (int aidx = 0; aidx < adjNormals.size(); ++aidx) {
				vertNormal += adjNormals[aidx] * adjAreas[aidx] / areaSum;
			}
		}

		vert->setNormal(vertNormal);
	}

	/*====== Programming Assignment 0 ======*/

	// Notify mesh shaders
	setVertexNormalDirty(true);
}
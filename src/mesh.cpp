#include "mesh.h"
#include <iostream>
#include <igl/read_triangle_mesh.h>
#include <Eigen/Sparse>

HEdge::HEdge(bool b)
{
	mBoundary = b;

	mTwin = nullptr;
	mPrev = nullptr;
	mNext = nullptr;

	mStart = nullptr;
	mFace = nullptr;

	mFlag = false;
	mValid = true;
}

HEdge *HEdge::twin() const
{
	return mTwin;
}

HEdge *HEdge::setTwin(HEdge *e)
{
	mTwin = e;
	return mTwin;
}

HEdge *HEdge::prev() const
{
	return mPrev;
}

HEdge *HEdge::setPrev(HEdge *e)
{
	mPrev = e;
	return mPrev;
}

HEdge *HEdge::next() const
{
	return mNext;
}

HEdge *HEdge::setNext(HEdge *e)
{
	mNext = e;
	return mNext;
}

Vertex *HEdge::start() const
{
	return mStart;
}

Vertex *HEdge::setStart(Vertex *v)
{
	mStart = v;
	return mStart;
}

Vertex *HEdge::end() const
{
	return mNext->start();
}

Face *HEdge::leftFace() const
{
	return mFace;
}

Face *HEdge::setFace(Face *f)
{
	mFace = f;
	return mFace;
}

bool HEdge::flag() const
{
	return mFlag;
}

bool HEdge::setFlag(bool b)
{
	mFlag = b;
	return mFlag;
}

bool HEdge::isBoundary() const
{
	return mBoundary;
}

bool HEdge::isValid() const
{
	return mValid;
}

bool HEdge::setValid(bool b)
{
	mValid = b;
	return mValid;
}

OneRingHEdge::OneRingHEdge(const Vertex *v)
{
	if (v == nullptr)
	{
		mStart = nullptr;
		mNext = nullptr;
	}
	else
	{
		mStart = v->halfEdge();
		mNext = v->halfEdge();
	}
}

HEdge *OneRingHEdge::nextHEdge()
{
	HEdge *ret = mNext;
	if (mNext != nullptr && mNext->prev()->twin() != mStart)
	{
		mNext = mNext->prev()->twin();
	}
	else
	{
		mNext = nullptr;
	}
	return ret;
}

OneRingVertex::OneRingVertex(const Vertex *v) : ring(v)
{
}

Vertex *OneRingVertex::nextVertex()
{
	HEdge *he = ring.nextHEdge();
	return he != nullptr ? he->end() : nullptr;
}

Vertex::Vertex() : mHEdge(nullptr), mFlag(0)
{
	mPosition = Eigen::Vector3f::Zero();
	mColor = VCOLOR_BLUE;
	mNormal = Eigen::Vector3f::Zero();
}

Vertex::Vertex(const Eigen::Vector3f &v) : mPosition(v), mHEdge(nullptr), mFlag(0)
{
	mColor = VCOLOR_BLUE;
	mNormal = Eigen::Vector3f::Zero();
}

Vertex::Vertex(float x, float y, float z) : mHEdge(nullptr), mFlag(0)
{
	mPosition = Eigen::Vector3f(x, y, z);
	mColor = VCOLOR_BLUE;
	mNormal = Eigen::Vector3f::Zero();
}

const Eigen::Vector3f &Vertex::position() const
{
	return mPosition;
}

const Eigen::Vector3f &Vertex::setPosition(const Eigen::Vector3f &p)
{
	mPosition = p;
	return mPosition;
}

const Eigen::Vector3f &Vertex::normal() const
{
	return mNormal;
}

const Eigen::Vector3f &Vertex::setNormal(const Eigen::Vector3f &n)
{
	mNormal = n;
	return mNormal;
}

const Eigen::Vector3f &Vertex::color() const
{
	return mColor;
}

const Eigen::Vector3f &Vertex::setColor(const Eigen::Vector3f &c)
{
	mColor = c;
	return mColor;
}

HEdge *Vertex::halfEdge() const
{
	return mHEdge;
}

HEdge *Vertex::setHalfEdge(HEdge *he)
{
	mHEdge = he;
	return mHEdge;
}

int Vertex::index() const
{
	return mIndex;
}

int Vertex::setIndex(int i)
{
	mIndex = i;
	return mIndex;
}

int Vertex::flag() const
{
	return mFlag;
}

int Vertex::setFlag(int f)
{
	mFlag = f;
	return mFlag;
}

bool Vertex::isValid() const
{
	return mValid;
}

bool Vertex::setValid(bool b)
{
	mValid = b;
	return mValid;
}

bool Vertex::isBoundary() const
{
	OneRingHEdge ring(this);
	HEdge *curr = nullptr;
	while (curr = ring.nextHEdge())
	{
		if (curr->isBoundary())
		{
			return true;
		}
	}
	return false;
}

int Vertex::valence() const
{
	int count = 0;
	OneRingVertex ring(this);
	Vertex *curr = nullptr;
	while (curr = ring.nextVertex())
	{
		++count;
	}
	return count;
}

Face::Face() : mHEdge(nullptr), mValid(true)
{
}

HEdge *Face::halfEdge() const
{
	return mHEdge;
}

HEdge *Face::setHalfEdge(HEdge *he)
{
	mHEdge = he;
	return mHEdge;
}

bool Face::isBoundary() const
{
	HEdge *curr = mHEdge;
	do
	{
		if (curr->twin()->isBoundary())
		{
			return true;
		}
		curr = curr->next();
	} while (curr != mHEdge);
	return false;
}

bool Face::isValid() const
{
	return mValid;
}

bool Face::setValid(bool b)
{
	mValid = b;
	return mValid;
}

Mesh::Mesh()
{
	mVertexPosFlag = true;
	mVertexNormalFlag = true;
	mVertexColorFlag = true;
}

Mesh::~Mesh()
{
	clear();
}

const std::vector<HEdge *> &Mesh::edges() const
{
	return mHEdgeList;
}

const std::vector<HEdge *> &Mesh::boundaryEdges() const
{
	return mBHEdgeList;
}

const std::vector<Vertex *> &Mesh::vertices() const
{
	return mVertexList;
}

const std::vector<Face *> &Mesh::faces() const
{
	return mFaceList;
}

bool Mesh::isVertexPosDirty() const
{
	return mVertexPosFlag;
}

void Mesh::setVertexPosDirty(bool b)
{
	mVertexPosFlag = b;
}

bool Mesh::isVertexNormalDirty() const
{
	return mVertexNormalFlag;
}

void Mesh::setVertexNormalDirty(bool b)
{
	mVertexNormalFlag = b;
}

bool Mesh::isVertexColorDirty() const
{
	return mVertexColorFlag;
}

void Mesh::setVertexColorDirty(bool b)
{
	mVertexColorFlag = b;
}

bool Mesh::loadMeshFile(const std::string filename)
{
	// Use libigl to parse the mesh file
	bool iglFlag = igl::read_triangle_mesh(filename, mVertexMat, mFaceMat);
	if (iglFlag)
	{
		clear();

		// Construct the half-edge data structure.
		int numVertices = mVertexMat.rows();
		int numFaces = mFaceMat.rows();

		// Fill in the vertex list
		for (int vidx = 0; vidx < numVertices; ++vidx)
		{
			mVertexList.push_back(new Vertex(mVertexMat(vidx, 0),
											 mVertexMat(vidx, 1),
											 mVertexMat(vidx, 2)));
		}
		// Fill in the face list
		for (int fidx = 0; fidx < numFaces; ++fidx)
		{
			addFace(mFaceMat(fidx, 0), mFaceMat(fidx, 1), mFaceMat(fidx, 2));
		}

		std::vector<HEdge *> hedgeList;
		for (int i = 0; i < mBHEdgeList.size(); ++i)
		{
			if (mBHEdgeList[i]->start())
			{
				hedgeList.push_back(mBHEdgeList[i]);
			}
			// TODO
		}
		mBHEdgeList = hedgeList;

		for (int i = 0; i < mVertexList.size(); ++i)
		{
			mVertexList[i]->adjHEdges.clear();
			mVertexList[i]->setIndex(i);
			mVertexList[i]->setFlag(0);
		}
	}
	else
	{
		std::cout << __FUNCTION__ << ": mesh file loading failed!\n";
	}
	return iglFlag;
}

static void _setPrevNext(HEdge *e1, HEdge *e2)
{
	e1->setNext(e2);
	e2->setPrev(e1);
}

static void _setTwin(HEdge *e1, HEdge *e2)
{
	e1->setTwin(e2);
	e2->setTwin(e1);
}

static void _setFace(Face *f, HEdge *e)
{
	f->setHalfEdge(e);
	e->setFace(f);
}

void Mesh::addFace(int v1, int v2, int v3)
{
	Face *face = new Face();

	HEdge *hedge[3];
	HEdge *bhedge[3]; // Boundary half-edges
	Vertex *vert[3];

	for (int i = 0; i < 3; ++i)
	{
		hedge[i] = new HEdge();
		bhedge[i] = new HEdge(true);
	}
	vert[0] = mVertexList[v1];
	vert[1] = mVertexList[v2];
	vert[2] = mVertexList[v3];

	// Connect prev-next pointers
	for (int i = 0; i < 3; ++i)
	{
		_setPrevNext(hedge[i], hedge[(i + 1) % 3]);
		_setPrevNext(bhedge[i], bhedge[(i + 1) % 3]);
	}

	// Connect twin pointers
	_setTwin(hedge[0], bhedge[0]);
	_setTwin(hedge[1], bhedge[2]);
	_setTwin(hedge[2], bhedge[1]);

	// Connect start pointers for bhedge
	bhedge[0]->setStart(vert[1]);
	bhedge[1]->setStart(vert[0]);
	bhedge[2]->setStart(vert[2]);
	for (int i = 0; i < 3; ++i)
	{
		hedge[i]->setStart(vert[i]);
	}

	// Connect start pointers
	// Connect face-hedge pointers
	for (int i = 0; i < 3; ++i)
	{
		vert[i]->setHalfEdge(hedge[i]);
		vert[i]->adjHEdges.push_back(hedge[i]);
		_setFace(face, hedge[i]);
	}
	vert[0]->adjHEdges.push_back(bhedge[1]);
	vert[1]->adjHEdges.push_back(bhedge[0]);
	vert[2]->adjHEdges.push_back(bhedge[2]);

	// Merge boundary if needed
	for (int i = 0; i < 3; ++i)
	{
		Vertex *start = bhedge[i]->start();
		Vertex *end = bhedge[i]->end();

		for (int j = 0; j < end->adjHEdges.size(); ++j)
		{
			HEdge *curr = end->adjHEdges[j];
			if (curr->isBoundary() && curr->end() == start)
			{
				_setPrevNext(bhedge[i]->prev(), curr->next());
				_setPrevNext(curr->prev(), bhedge[i]->next());
				_setTwin(bhedge[i]->twin(), curr->twin());
				bhedge[i]->setStart(nullptr); // Mark as unused
				curr->setStart(nullptr);	  // Mark as unused
				break;
			}
		}
	}

	// Finally add hedges and faces to list
	for (int i = 0; i < 3; ++i)
	{
		mHEdgeList.push_back(hedge[i]);
		mBHEdgeList.push_back(bhedge[i]);
	}
	mFaceList.push_back(face);
}

Eigen::Vector3f Mesh::initBboxMin() const
{
	return (mVertexMat.colwise().minCoeff()).transpose();
}

Eigen::Vector3f Mesh::initBboxMax() const
{
	return (mVertexMat.colwise().maxCoeff()).transpose();
}

void Mesh::groupingVertexFlags()
{
	// Init to 255
	for (Vertex *vert : mVertexList)
	{
		if (vert->flag() != 0)
		{
			vert->setFlag(255);
		}
	}
	// Group handles
	int id = 0;
	std::vector<Vertex *> tmpList;
	for (Vertex *vert : mVertexList)
	{
		if (vert->flag() == 255)
		{
			++id;
			vert->setFlag(id);

			// Do search
			tmpList.push_back(vert);
			while (!tmpList.empty())
			{
				Vertex *v = tmpList.back();
				tmpList.pop_back();

				OneRingVertex orv = OneRingVertex(v);
				while (Vertex *v2 = orv.nextVertex())
				{
					if (v2->flag() == 255)
					{
						v2->setFlag(id);
						tmpList.push_back(v2);
					}
				}
			}
		}
	}
}

void Mesh::clear()
{
	for (int i = 0; i < mHEdgeList.size(); ++i)
	{
		delete mHEdgeList[i];
	}
	for (int i = 0; i < mBHEdgeList.size(); ++i)
	{
		delete mBHEdgeList[i];
	}
	for (int i = 0; i < mVertexList.size(); ++i)
	{
		delete mVertexList[i];
	}
	for (int i = 0; i < mFaceList.size(); ++i)
	{
		delete mFaceList[i];
	}

	mHEdgeList.clear();
	mBHEdgeList.clear();
	mVertexList.clear();
	mFaceList.clear();
}

std::vector<int> Mesh::collectMeshStats()
{
	int V = 0; // # of vertices
	int E = 0; // # of half-edges
	int F = 0; // # of faces
	int B = 0; // # of boundary loops
	int C = 0; // # of connected components
	int G = 0; // # of genus

	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*
	/* Collect mesh information as listed above.
	/**********************************************/
	V = this->mVertexList.size();
	E = this->mHEdgeList.size() + this->mBHEdgeList.size();
	F = this->mFaceList.size();
	B = this->countBoundaryLoops();

	/*====== Programming Assignment 0 ======*/

	std::vector<int> stats;
	stats.push_back(V);
	stats.push_back(E);
	stats.push_back(F);
	stats.push_back(B);
	stats.push_back(C);
	stats.push_back(G);
	return stats;
}

int Mesh::countBoundaryLoops()
{
	int count = 0;
	for (int i = 0; i < this->mBHEdgeList.size(); ++i)
	{
		HEdge *edge = this->mBHEdgeList[i];
		edge->setFlag(true);
	}

	for (int i = 0; i < this->mBHEdgeList.size(); ++i)
	{
		HEdge *edge = this->mBHEdgeList[i];
		if (edge->flag())
		{
			continue;
		}
		HEdge *nextEdge = edge->next();
		while (edge != nextEdge)
		{
			if (std::find(this->mBHEdgeList.begin(), this->mBHEdgeList.end(), nextEdge) != this->mBHEdgeList.end())
			{
				nextEdge->setFlag(true);
			}
			nextEdge = nextEdge->next();
		}

		++count;
		edge->setFlag(true);
	}
	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*
	/* Helper function for Mesh::collectMeshStats()
	/**********************************************/

	/*====== Programming Assignment 0 ======*/

	for (int i = 0; i < this->mBHEdgeList.size(); ++i)
	{
		HEdge *edge = this->mBHEdgeList[i];
		edge->setFlag(false);
	}

	return count;
}

int Mesh::countConnectedComponents()
{
	int count = 0;

	/*====== Programming Assignment 0 ======*/

	/**********************************************/
	/*          Insert your code here.            */
	/**********************************************/
	/*
	/* Helper function for Mesh::collectMeshStats()
	/* Count the number of connected components of
	/* the mesh. (Hint: use a stack)
	/**********************************************/

	/*====== Programming Assignment 0 ======*/

	return count;
}

void Mesh::computeVertexNormals()
{
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

	/*====== Programming Assignment 0 ======*/
	for (Vertex *v : mVertexList)
	{
		Eigen::Vector3f vertNormal = Eigen::Vector3f::Zero();

		std::vector<HEdge *> adjHEdgeList;
		OneRingHEdge orhe(v);
		HEdge *curr = nullptr;
		while (curr = orhe.nextHEdge())
		{
			adjHEdgeList.push_back(curr);
		}

		if (adjHEdgeList.size() >= 2)
		{
			const Eigen::Vector3f &p0 = v->position();
			std::vector<Eigen::Vector3f> adjNormals;
			std::vector<double> adjAreas;
			double areaSum = 0;
			for (HEdge *adjHedge : adjHEdgeList)
			{
				if (!adjHedge->isBoundary())
				{
					const Eigen::Vector3f &p1 = adjHedge->end()->position();
					const Eigen::Vector3f &p2 = adjHedge->next()->end()->position();

					adjNormals.push_back(triangleNormal(p0, p1, p2));

					double area = triangleArea(p0, p1, p2);
					adjAreas.push_back(area);
					areaSum += area;
				}
			}
			for (int aidx = 0; aidx < adjNormals.size(); ++aidx)
			{
				vertNormal += adjNormals[aidx] * adjAreas[aidx] / areaSum;
			}
		}

		v->setNormal(vertNormal);
	}
	// Notify mesh shaders
	setVertexNormalDirty(true);
}

void Mesh::umbrellaSmooth(bool cotangentWeights)
{
	/*====== Programming Assignment 1 ======*/

	if (cotangentWeights)
	{
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 1: Implement the cotangent weighting 
		/* scheme for explicit mesh smoothing. 
		/*
		/* Hint:
		/* It is advised to double type to store the 
		/* weights to avoid numerical issues.
		/**********************************************/
		for (Vertex *v : mVertexList)
		{
			Eigen::Vector3f vNewPos = Eigen::Vector3f::Zero();

			OneRingVertex orv(v);
			Vertex *curr = nullptr;
			double sumW = 0.0;
			while (curr = orv.nextVertex())
			{
				// alpha
				double count = 0.0;
				double w = 0.0;
				Vertex *next = nullptr;
				if (!curr->halfEdge()->isBoundary() && curr->halfEdge()->end() != v)
				{
					next = curr->halfEdge()->end();
					w += cosVectors(v->position(), curr->position(), next->position());
					count += 1.0;
				}

				// beta
				if (!curr->halfEdge()->prev()->twin()->isBoundary() && curr->halfEdge()->prev()->twin()->prev()->start() != v)
				{
					next = curr->halfEdge()->prev()->twin()->prev()->start();
					w += cosVectors(v->position(), curr->position(), next->position());
					count += 1.0;
				}

				if (count == 0.0)
				{
					continue;
				}
				w /= count;
				vNewPos += w * curr->position();
				sumW += w;
			}

			vNewPos /= sumW;
			v->setPosition(vNewPos);
		}
	}
	else
	{
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 2: Implement the uniform weighting 
		/* scheme for explicit mesh smoothing.
		/**********************************************/
		for (Vertex *v : mVertexList)
		{
			Eigen::Vector3f vNewPos = Eigen::Vector3f::Zero();

			OneRingVertex orv(v);
			Vertex *curr = nullptr;
			while (curr = orv.nextVertex())
			{
				// adjVertexs.push_back(curr);
				vNewPos += curr->position();
			}

			vNewPos /= float(v->valence());
			v->setPosition(vNewPos);
		}
	}

	/*====== Programming Assignment 1 ======*/

	computeVertexNormals();
	// Notify mesh shaders
	setVertexPosDirty(true);
}

void Mesh::implicitUmbrellaSmooth(bool cotangentWeights)
{
	/*====== Programming Assignment 1 ======*/
	Eigen::VectorXd bx(mVertexList.size()), by(mVertexList.size()), bz(mVertexList.size()), x_sol(mVertexList.size()), y_sol(mVertexList.size()), z_sol(mVertexList.size());
	for (int k = 0; k < mVertexList.size(); k++) {
		bx[k] = mVertexList[k]->position()[0];
		by[k] = mVertexList[k]->position()[1];
		bz[k] = mVertexList[k]->position()[2];
		x_sol[k] = y_sol[k] = z_sol[k] = 0;
	}

	/* A sparse linear system Ax=b solver using the conjugate gradient method. */
	auto fnConjugateGradient = [](const Eigen::SparseMatrix<double> &A,
								  const Eigen::VectorXd &b,
								  int maxIterations,
								  float errorTolerance,
								  Eigen::VectorXd &x) {
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Params:
		/*  A: 
		/*  b: 
		/*  maxIterations:	Max number of iterations
		/*  errorTolerance: Error tolerance for the early stopping condition
		/*  x:				Stores the final solution, but should be initialized. 
		/**********************************************/
		/*
		/* Step 1: Implement the biconjugate gradient
		/* method.
		/* Hint: https://en.wikipedia.org/wiki/Biconjugate_gradient_method
		/**********************************************/
		// x = Eigen::VectorXf.zero(A.cols());

		Eigen::VectorXd r = b - A * x;
		Eigen::VectorXd p(r);
		int i = 0;
		while(i < maxIterations) {
			double alpha = ((r.transpose() * r) / (p.transpose() * A * p)).value();
			x = x + alpha * p;
			Eigen::VectorXd r1 = r - alpha * A * p;
			
			if(r1.norm() <= errorTolerance ) break;

			double beta = ((r1.transpose() * r1) / (r.transpose() * r)).value();
			p = r1 + beta * p;
			r = r1;
			++i;
		}
	};

	/* IMPORTANT:
	/* Please refer to the following link about the sparse matrix construction in Eigen. */
	/* http://eigen.tuxfamily.org/dox/group__TutorialSparse.html#title3 */

	Eigen::SparseMatrix<double> mat(mVertexList.size(),mVertexList.size());
	typedef Eigen::Triplet<double> T;
	std::vector<T> tripletList;
	tripletList.reserve(0);
		
	if (cotangentWeights)
	{
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 2: Implement the cotangent weighting 
		/* scheme for implicit mesh smoothing. Use
		/* the above fnConjugateGradient for solving
		/* sparse linear systems.
		/*
		/* Hint:
		/* It is advised to double type to store the
		/* weights to avoid numerical issues.
		/**********************************************/
		for(int i = 0; i < mVertexList.size(); ++i)
		{
			Vertex *v = mVertexList[i];
			std::vector<T> tmpTripletList;

			OneRingVertex orv(v);
			Vertex *curr = nullptr;
			double sumW = 0.0;
			while (curr = orv.nextVertex())
			{
				// alpha
				double count = 0.0;
				double w = 0.0;
				Vertex *next = nullptr;
				if (!curr->halfEdge()->isBoundary() && curr->halfEdge()->end() != v)
				{
					next = curr->halfEdge()->end();
					w += cosVectors(v->position(), curr->position(), next->position());
					count += 1.0;
				}

				// beta
				if (!curr->halfEdge()->prev()->twin()->isBoundary() && curr->halfEdge()->prev()->twin()->prev()->start() != v)
				{
					next = curr->halfEdge()->prev()->twin()->prev()->start();
					w += cosVectors(v->position(), curr->position(), next->position());
					count += 1.0;
				}

				if (count == 0.0)
				{
					continue;
				}
				w /= count;
				tmpTripletList.push_back(T(v->index(), curr->index(), w));
				sumW += w;
			}
			tripletList.push_back(T(v->index(), v->index(), -1.0));
			for(int j = 0; j < tmpTripletList.size(); ++j){
				T t = tmpTripletList[j];
				tripletList.push_back(T(t.row(), t.col(), t.value() / sumW));
			}
		}
	}
	else
	{
		/**********************************************/
		/*          Insert your code here.            */
		/**********************************************/
		/*
		/* Step 3: Implement the uniform weighting 
		/* scheme for implicit mesh smoothing. Use
		/* the above fnConjugateGradient for solving
		/* sparse linear systems.
		/**********************************************/
		for(int i = 0; i < mVertexList.size(); ++i)
		{
			Vertex *v = mVertexList[i];
			tripletList.push_back(T(v->index(), v->index(), -1.0));
			OneRingVertex orv(v);
			Vertex *curr = nullptr;
			while (curr = orv.nextVertex())
			{
				tripletList.push_back(T(v->index(), curr->index(), 1/ float(v->valence())));
			}
		}
	}

	mat.setFromTriplets(tripletList.begin(), tripletList.end());
	Eigen::SparseMatrix<double> idMat(mVertexList.size(),mVertexList.size());
	idMat.setIdentity();
	fnConjugateGradient(idMat - mat, bx, 100, 1e-6, x_sol);
	fnConjugateGradient(idMat - mat, by, 100, 1e-6, y_sol);
	fnConjugateGradient(idMat - mat, bz, 100, 1e-6, z_sol);

	for (int k = 0; k < mVertexList.size(); k++) {
		mVertexList[k]->setPosition(Eigen::Vector3f((float)x_sol[k], (float)y_sol[k], (float)z_sol[k]));
	}

	/*====== Programming Assignment 1 ======*/

	computeVertexNormals();
	// Notify mesh shaders
	setVertexPosDirty(true);
}

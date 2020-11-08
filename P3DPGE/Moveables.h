/*#pragma once
#include "olcPixelGameEngine.h"
#include "Math.h"

using namespace olc;

class Rect : public Moveable {
public:

	int w;
	int h;
	int l;
	
	Rect(vd2d recVector, float mass, int id, vd2d pv = vd2d(0, 0), vd2d vv = vd2d(0, 0), vd2d av = vd2d(0, 0), 
			double rot = 0, double rotv = 0, double rota = 0) {
		this->recVector = recVector;
		this->id = id;
		this->pv = pv;
		this->vv = vv;
		this->av = av;
		this->rot = rot;
		this->rotv = rotv;
		this->rota = rota;
		this->mass = mass;
	}
	
	void draw(PixelGameEngine* p, Pixel color = WHITE) { 

		p->FillRect(pv.x, pv.y, 2*(), h, color);
	}
	
	void checkCollide(std::vector<Rect> rv) {
		for (auto& t : rv) {
			if (id != t.id) {
				if (pv.y <= t.pv.y + t.h || pv.y + h >= t.pv.y && pv.x <= t.pv.x + t.w || pv.x + w >= t.pv.x) {
					pv -= t.pv;
				}
			}
		}
	}
	
};

struct Circ : public Moveable {
	
	int r;
	
	Circ(int r, double mass, vd2d pv = vd2d(0, 0), vd2d vv = vd2d(0, 0), vd2d av = vd2d(0, 0), double rot = 0, double rotv = 0, double rota = 0) {
		this->r = r;
		this->pv = pv;
		this->vv = vv;
		this->av = av;
		this->rot = rot;
		this->rotv = rotv;
		this->rota = rota;
		this->mass = mass;
	}

	void draw(PixelGameEngine* p, Pixel color = WHITE) {
		p->FillCircle(pv, r, color);
	}
};*/
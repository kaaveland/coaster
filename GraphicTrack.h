#ifndef __GraphicTrack_h_
#define __GraphicTrack_h_

#include "BaseApplication.h"
#include "Track.h"
#include "Vector3d.h"

class GraphicTrack
{
public:
    //GraphicTrack(void);
    //virtual ~GraphicTrack(void);
	static void createRailMesh(Track* track, const bool export_mesh);
};










#endif //__GraphicTrack_h_
#include "os/compat.h"

#include "erosion.h"
#include <math.h>
#include <iostream>


namespace reaper 
{
namespace gfx
{
namespace mesh
{

	using std::cout;
	using std::cin;
	using std::endl;

void Erosion::thermal(float talus, float ct, int iter)
{

	int xi, yi, i;
	talus = 3.14 * talus / 180.0;

	int step = 0;

	float ang[8];

	HeightField hft(hf.get_width(), hf.get_height());

	float ofa;

	while(step < iter){

		hft = hf;

		for( xi = 1; xi < hf.get_width() - 1; ++xi)
			for( yi = 1; yi < hf.get_height() - 1; ++yi){
				
				float height = sfz*hf.get(xi, yi);

				//cout << "height: " << height << "\n";
				
				//cout << sfz*hf.get( (xi - 1), (yi - 1) ) << endl;

				ang[0] = atan( (height - sfz*hf.get( (xi - 1), (yi - 1) )) * diagdif_inv);
				ang[1] = atan( (height - sfz*hf.get( (xi - 0), (yi - 1) )) * ydif_inv);
				ang[2] = atan( (height - sfz*hf.get( (xi + 1), (yi - 1) )) * diagdif_inv);

				ang[3] = atan( (height - sfz*hf.get( (xi - 1), (yi - 0) )) * xdif_inv);
				ang[4] = atan( (height - sfz*hf.get( (xi + 1), (yi - 0) )) * xdif_inv);

				ang[5] = atan( (height - sfz*hf.get( (xi - 1), (yi + 1) )) * diagdif_inv);
				ang[6] = atan( (height - sfz*hf.get( (xi - 0), (yi + 1) )) * ydif_inv);
				ang[7] = atan( (height - sfz*hf.get( (xi + 1), (yi + 1) )) * diagdif_inv);

				ofa = 0;
				for( i = 0; i < 8; ++i){
					//if( ang[i] != 0.0) cout << "bu\n";
					if( ang[i] > talus) ofa += ang[i] - talus;
				}

				//if( ofa > 0.0) cout << "oj, det funkar.\n";

				if(ang[0] > talus)hft.add( (xi - 1), (yi - 1), ct* ( ( ang[0] - talus) / ofa) );
				if(ang[1] > talus)hft.add( (xi - 0), (yi - 1), ct* ( ( ang[1] - talus) / ofa) );
				if(ang[2] > talus)hft.add( (xi + 1), (yi - 1), ct* ( ( ang[2] - talus) / ofa) );

				if(ang[3] > talus)hft.add( (xi - 1), (yi - 0), ct* ( ( ang[3] - talus) / ofa) );
				if(ang[4] > talus)hft.add( (xi + 1), (yi - 0), ct* ( ( ang[4] - talus) / ofa) );

				if(ang[5] > talus)hft.add( (xi - 1), (yi + 1), ct* ( ( ang[5] - talus) / ofa) );
				if(ang[6] > talus)hft.add( (xi - 0), (yi + 1), ct* ( ( ang[6] - talus) / ofa) );
				if(ang[7] > talus)hft.add( (xi + 1), (yi + 1), ct* ( ( ang[7] - talus) / ofa) );


				//cout << ct*ofa << "\n";

				hft.sub(xi,yi,ct);
			}
		
		//Edges and corners
		//First erode corners
		float angc[3];		int xi = 0;yi = 0;
		
		//leftbottom
						
		float height = sfz*hf.get(xi, yi);
		
		angc[0] = atan( (height - sfz*hf.get( (xi - 0), (yi + 1) )) * ydif_inv);
		angc[1] = atan( (height - sfz*hf.get( (xi + 1), (yi + 1) )) * diagdif_inv);
		angc[2] = atan( (height - sfz*hf.get( (xi + 1), (yi - 0) )) * xdif_inv);
		
		ofa = 0;
		for( i = 0; i < 3; ++i){
			if( angc[i] > talus) ofa += angc[i] - talus;
		}
		
		if(angc[0] > talus)hft.add( (xi - 0), (yi + 1), ct* ( ( angc[0] - talus) / ofa) );
		if(angc[1] > talus)hft.add( (xi + 1), (yi + 1), ct* ( ( angc[1] - talus) / ofa) );
		if(angc[2] > talus)hft.add( (xi + 1), (yi - 0), ct* ( ( angc[2] - talus) / ofa) );
		
		
		hft.sub(xi,yi,ct);
		
						
		//rightbottom
		xi = hf.get_width() - 1;
						
		height = sfz*hf.get(xi, yi);
		
		angc[0] = atan( (height - sfz*hf.get( (xi - 1), (yi + 1) )) * ydif_inv);
		angc[1] = atan( (height - sfz*hf.get( (xi + 0), (yi + 1) )) * diagdif_inv);
		angc[2] = atan( (height - sfz*hf.get( (xi - 1), (yi - 0) )) * xdif_inv);
		
		ofa = 0;
		for( i = 0; i < 3; ++i){
			if( angc[i] > talus) ofa += angc[i] - talus;
		}
		
		if(angc[0] > talus)hft.add( (xi - 1), (yi + 1), ct* ( ( angc[0] - talus) / ofa) );
		if(angc[1] > talus)hft.add( (xi + 0), (yi + 1), ct* ( ( angc[1] - talus) / ofa) );
		if(angc[2] > talus)hft.add( (xi - 1), (yi - 0), ct* ( ( angc[2] - talus) / ofa) );
		
		
		hft.sub(xi,yi,ct);

		//lefttop
		xi = 0;
		yi = hf.get_height() - 1;
						
		height = sfz*hf.get(xi, yi);
		
		angc[0] = atan( (height - sfz*hf.get( (xi + 1), (yi - 0) )) * ydif_inv);
		angc[1] = atan( (height - sfz*hf.get( (xi - 0), (yi - 1) )) * diagdif_inv);
		angc[2] = atan( (height - sfz*hf.get( (xi + 1), (yi - 1) )) * xdif_inv);
		
		ofa = 0;
		for( i = 0; i < 3; ++i){
			if( angc[i] > talus) ofa += angc[i] - talus;
		}
		
		if(angc[0] > talus)hft.add( (xi + 1), (yi - 0), ct* ( ( angc[0] - talus) / ofa) );
		if(angc[1] > talus)hft.add( (xi - 0), (yi - 1), ct* ( ( angc[1] - talus) / ofa) );
		if(angc[2] > talus)hft.add( (xi + 1), (yi - 1), ct* ( ( angc[2] - talus) / ofa) );
		
		
		hft.sub(xi,yi,ct);

		
		//righttop
		xi = hf.get_width() -1;
		yi = hf.get_height() -1;
						
		height = sfz*hf.get(xi, yi);
		
		angc[0] = atan( (height - sfz*hf.get( (xi - 1), (yi - 0) )) * ydif_inv);
		angc[1] = atan( (height - sfz*hf.get( (xi - 1), (yi - 1) )) * diagdif_inv);
		angc[2] = atan( (height - sfz*hf.get( (xi - 0), (yi - 1) )) * xdif_inv);
		
		ofa = 0;
		for( i = 0; i < 3; ++i){
			if( angc[i] > talus) ofa += angc[i] - talus;
		}
		
		if(angc[0] > talus)hft.add( (xi - 1), (yi - 0), ct* ( ( angc[0] - talus) / ofa) );
		if(angc[1] > talus)hft.add( (xi - 1), (yi - 1), ct* ( ( angc[1] - talus) / ofa) );
		if(angc[2] > talus)hft.add( (xi - 0), (yi - 1), ct* ( ( angc[2] - talus) / ofa) );
		
		
		hft.sub(xi,yi,ct);


		//EDGES
		float ange[5];
		xi = 0;
		//left
			
		for( yi = 1; yi < hf.get_height() - 1; ++yi){
			
			float height = sfz*hf.get(xi, yi);
						
			ange[0] = atan( (height - sfz*hf.get( (xi - 0), (yi - 1) )) * ydif_inv);
			ange[1] = atan( (height - sfz*hf.get( (xi + 1), (yi - 1) )) * diagdif_inv);
			ange[2] = atan( (height - sfz*hf.get( (xi + 1), (yi - 0) )) * xdif_inv);
			ange[3] = atan( (height - sfz*hf.get( (xi - 0), (yi + 1) )) * ydif_inv);
			ange[4] = atan( (height - sfz*hf.get( (xi + 1), (yi + 1) )) * diagdif_inv);
			
			ofa = 0;
			for( i = 0; i < 5; ++i){
				if( ange[i] > talus) ofa += ange[i] - talus;
			}
			
			if(ange[0] > talus)hft.add( (xi - 0), (yi - 1), ct* ( ( ange[0] - talus) / ofa) );
			if(ange[1] > talus)hft.add( (xi + 1), (yi - 1), ct* ( ( ange[1] - talus) / ofa) );
			if(ange[2] > talus)hft.add( (xi + 1), (yi - 0), ct* ( ( ange[2] - talus) / ofa) );
			if(ange[3] > talus)hft.add( (xi - 0), (yi + 1), ct* ( ( ange[3] - talus) / ofa) );
			if(ange[4] > talus)hft.add( (xi + 1), (yi + 1), ct* ( ( ange[4] - talus) / ofa) );
			
			//cout << ct*ofa << "\n";
			
			hft.sub(xi,yi,ct);
		}
				
		//right
		xi = hf.get_width() - 1;
		for( yi = 1; yi < hf.get_height() - 1 ; ++yi){
			
			float height = sfz*hf.get(xi, yi);
			
			ange[0] = atan( (height - sfz*hf.get( (xi - 0), (yi - 1) )) * ydif_inv);
			ange[1] = atan( (height - sfz*hf.get( (xi - 1), (yi - 1) )) * diagdif_inv);
			ange[2] = atan( (height - sfz*hf.get( (xi - 1), (yi - 0) )) * xdif_inv);
			ange[3] = atan( (height - sfz*hf.get( (xi - 0), (yi + 1) )) * ydif_inv);
			ange[4] = atan( (height - sfz*hf.get( (xi - 1), (yi + 1) )) * diagdif_inv);
			
			ofa = 0;
			for( i = 0; i < 5; ++i){
				if( ange[i] > talus) ofa += ange[i] - talus;
			}
			
			if(ange[0] > talus)hft.add( (xi - 0), (yi - 1), ct* ( ( ange[0] - talus) / ofa) );
			if(ange[1] > talus)hft.add( (xi - 1), (yi - 1), ct* ( ( ange[1] - talus) / ofa) );
			if(ange[2] > talus)hft.add( (xi - 1), (yi - 0), ct* ( ( ange[2] - talus) / ofa) );
			if(ange[3] > talus)hft.add( (xi - 0), (yi + 1), ct* ( ( ange[3] - talus) / ofa) );
			if(ange[4] > talus)hft.add( (xi - 1), (yi + 1), ct* ( ( ange[4] - talus) / ofa) );
			
			//cout << ct*ofa << "\n";
			
			hft.sub(xi,yi,ct);
		}

		//top		
		int yi = hf.get_height() -1;
		for( xi = 1; xi < hf.get_width() - 1 ; ++xi){
			
			float height = sfz*hf.get(xi, yi);
			
			ange[0] = atan( (height - sfz*hf.get( (xi - 1), (yi - 0) )) * xdif_inv);
			ange[1] = atan( (height - sfz*hf.get( (xi + 1), (yi - 0) )) * xdif_inv);
			ange[2] = atan( (height - sfz*hf.get( (xi - 1), (yi - 1) )) * diagdif_inv);
			ange[3] = atan( (height - sfz*hf.get( (xi - 0), (yi - 1) )) * ydif_inv);
			ange[4] = atan( (height - sfz*hf.get( (xi + 1), (yi - 1) )) * diagdif_inv);
			
			ofa = 0;
			for( i = 0; i < 5; ++i){
				if( ange[i] > talus) ofa += ange[i] - talus;
			}
			
			if(ange[0] > talus)hft.add( (xi - 1), (yi - 0), ct* ( ( ange[0] - talus) / ofa) );
			if(ange[1] > talus)hft.add( (xi + 1), (yi - 0), ct* ( ( ange[1] - talus) / ofa) );
			if(ange[2] > talus)hft.add( (xi - 1), (yi - 1), ct* ( ( ange[2] - talus) / ofa) );
			if(ange[3] > talus)hft.add( (xi - 0), (yi - 1), ct* ( ( ange[3] - talus) / ofa) );
			if(ange[4] > talus)hft.add( (xi + 1), (yi - 1), ct* ( ( ange[4] - talus) / ofa) );
			
			//cout << ct*ofa << "\n";
			
			hft.sub(xi,yi,ct);
		}
		//bottom
		yi = 0;
		for( xi = 1; xi < hf.get_width() - 1 ; ++xi){
			
			float height = sfz*hf.get(xi, yi);
			
			ange[0] = atan( (height - sfz*hf.get( (xi - 1), (yi - 0) )) * xdif_inv);
			ange[1] = atan( (height - sfz*hf.get( (xi + 1), (yi - 0) )) * xdif_inv);
			ange[2] = atan( (height - sfz*hf.get( (xi - 1), (yi + 1) )) * diagdif_inv);
			ange[3] = atan( (height - sfz*hf.get( (xi - 0), (yi + 1) )) * ydif_inv);
			ange[4] = atan( (height - sfz*hf.get( (xi + 1), (yi + 1) )) * diagdif_inv);
			
			ofa = 0;
			for( i = 0; i < 5; ++i){
				if( ange[i] > talus) ofa += ange[i] - talus;
			}
			
			if(ange[0] > talus)hft.add( (xi - 1), (yi - 0), ct* ( ( ange[0] - talus) / ofa) );
			if(ange[1] > talus)hft.add( (xi + 1), (yi - 0), ct* ( ( ange[1] - talus) / ofa) );
			if(ange[2] > talus)hft.add( (xi - 1), (yi + 1), ct* ( ( ange[2] - talus) / ofa) );
			if(ange[3] > talus)hft.add( (xi - 0), (yi + 1), ct* ( ( ange[3] - talus) / ofa) );
			if(ange[4] > talus)hft.add( (xi + 1), (yi + 1), ct* ( ( ange[4] - talus) / ofa) );
			
			//cout << ct*ofa << "\n";
			
			hft.sub(xi,yi,ct);
		}


		hf = hft;
		++step;
		

	}

	
}

}
}
}
